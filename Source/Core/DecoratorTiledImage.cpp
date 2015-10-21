/*
 * This source file is part of libRocket, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://www.librocket.com
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "precompiled.h"
#include "DecoratorTiledImage.h"
#include "../../Include/Rocket/Core/Element.h"
#include "../../Include/Rocket/Core/Geometry.h"
#include "../../Include/Rocket/Core/GeometryUtilities.h"

using std::max;
using std::min;

namespace Rocket {
namespace Core {

DecoratorTiledImage::DecoratorTiledImage()
{
}

DecoratorTiledImage::~DecoratorTiledImage()
{
}

// Initialises the tiles for the decorator.
bool DecoratorTiledImage::Initialise(const Tile& _tile, const String& _texture_name, const String& _rcss_path)
{
	// Load the texture.
	tile = _tile;
	tile.texture_index = LoadTexture(_texture_name, _rcss_path);
	if (tile.texture_index < 0)
		return false;

	return true;
}

// Called on a decorator to generate any required per-element data for a newly decorated element.
DecoratorDataHandle DecoratorTiledImage::GenerateElementData(Element* element)
{
	// Calculate the tile's dimensions for this element.
	tile.CalculateDimensions(element, *GetTexture(tile.texture_index));

	Geometry* data = new Geometry(element);
	data->SetTexture(GetTexture());

	Vector2f dest = element->GetBox().GetSize(Box::PADDING);
	Vector2f source = tile.GetDimensions(element);

	// Generate the geometry for the tile.
	switch(tile.scaling_mode) {
	case IGNORE /* default */: tile.GenerateGeometry(data->GetVertices(), data->GetIndices(), element, Vector2f(0, 0), dest, source, color_multiplier); break;
	case FILL:
	case FIT: {
	  Vector2f offset(0, 0); float f; 
	  switch(tile.scaling_mode) {
	  case FILL: {
	    RenderInterface* render_interface = element->GetRenderInterface();
	    Vector2i texture_dimensions = GetTexture(tile.texture_index)->GetDimensions(render_interface);
	    f = max(dest.y / source.y, dest.x / source.x); 
	  }; break;
	  case FIT: 
	    f = min(dest.y / source.y, dest.x / source.x); 
	    source *= f;
	    offset.x = (dest.x - source.x)/2;
	    offset.y = (dest.y - source.y)/2;
	    dest = source;
	    break;
	  }
	  tile.GenerateGeometry(data->GetVertices(), data->GetIndices(), element, offset, dest, source, color_multiplier); 
	}; break;
	case CENTER: 
	  Vector2f offset(0, 0);
	  Vector2i texture_dimension;
	  if (!tile.texcoords_absolute[0][0] || !tile.texcoords_absolute[1][0] || !tile.texcoords_absolute[0][1] || !tile.texcoords_absolute[1][1]) {
	    RenderInterface* render_interface = element->GetRenderInterface();
	    texture_dimension = GetTexture(tile.texture_index)->GetDimensions(render_interface);
	  }
	  if (source.x > dest.x) { // crop width of image
	    float diff_begin = (source.x - dest.x)/2, diff_end = diff_begin;
	    if (!tile.texcoords_absolute[0][0] || !tile.texcoords_absolute[1][0]) {
	      if (!tile.texcoords_absolute[0][0]) diff_begin /= texture_dimension.x;
	      if (!tile.texcoords_absolute[1][0]) diff_end /= texture_dimension.x;
	    }
	    tile.texcoords[0].x += diff_begin; // -s-begin
	    tile.texcoords[1].x -= diff_end; // -s-end
	    source.x = dest.x;
	  } else {                 // center image
	    offset.x = (dest.x - source.x)/2;
	    dest.x = source.x;
	  }
	  if (source.y > dest.y) { // crop height of image
	    float diff_begin = (source.y - dest.y)/2, diff_end = diff_begin;
	    if (!tile.texcoords_absolute[0][1] || !tile.texcoords_absolute[1][1]) {
	      if (!tile.texcoords_absolute[0][1]) diff_begin /= texture_dimension.y;
	      if (!tile.texcoords_absolute[1][1]) diff_end /= texture_dimension.y;
	    }
	    tile.texcoords[0].y += diff_begin; // -s-begin
	    tile.texcoords[1].y -= diff_end; // -s-end
	    source.y = dest.y;
	  } else {                 // center image
	    offset.y = (dest.y - source.y)/2;
	    dest.y = source.y;
	  }
	  tile.GenerateGeometry(data->GetVertices(), data->GetIndices(), element, offset, dest, source, color_multiplier); 
	  break;
	}

	return reinterpret_cast<DecoratorDataHandle>(data);
}

// Called to release element data generated by this decorator.
void DecoratorTiledImage::ReleaseElementData(DecoratorDataHandle element_data)
{
	delete reinterpret_cast< Geometry* >(element_data);
}

// Called to render the decorator on an element.
void DecoratorTiledImage::RenderElement(Element* element, DecoratorDataHandle element_data)
{
	Geometry* data = reinterpret_cast< Geometry* >(element_data);
	data->Render(element->GetAbsoluteOffset(Box::PADDING));
}

}
}
