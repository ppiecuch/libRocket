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
#include "DecoratorAliasInstancer.h"
#include "StyleSheetFactory.h"

namespace Rocket {
namespace Core {

DecoratorAliasInstancer::DecoratorAliasInstancer()
{
}

DecoratorAliasInstancer::~DecoratorAliasInstancer()
{
}

// Instances a decorator given the property tag and attributes from the RCSS file.
Decorator* DecoratorAliasInstancer::InstanceDecorator(const String& name, const PropertyDictionary& properties)
{
	const Property* id_property = properties.GetProperty("decorator-id");
	if (id_property) {
	  String decorator_id = id_property->Get< String >();
	  Core::Log::Message(Core::Log::LT_INFO, "Aliasing decorator: '%s'", decorator_id.CString());

	  StringList parameter_list;
	  StringUtilities::ExpandString( parameter_list, decorator_id, ':' );

	  if (parameter_list.size() != 2) {
	    Core::Log::Message(Core::Log::LT_WARNING, "Invalid alias id: %s", decorator_id.CString());
	    return NULL;
	  }

	  const PropertyDictionary* decor_props = StyleSheetFactory::FindDecoratorPropertiesWithId(parameter_list[0], parameter_list[1]);
	  if (decor_props == NULL)
	    return NULL;
	  PropertyMap::const_iterator decor_type =  decor_props->GetProperties().find("decorator");
	  if (decor_type != decor_props->GetProperties().end()) {
	    if ((*decor_type).second.ToString() == "alias") {
	      Log::Message(Log::LT_WARNING, "Aliasing to alias decorator?");
	    } else {
	      // create requested decorator:
	      Decorator* decorator = Factory::InstanceDecorator( (*decor_type).second.ToString(), *decor_props);
	      if (decorator == NULL)
		Log::Message(Log::LT_WARNING, "Failed to instance decorator '%s' of type '%s'.", name.CString(), (*decor_type).second.ToString().CString());
	      return decorator;
	    }
	  } else
	    Log::Message(Log::LT_WARNING, "No decorator type found.");
	}
	return NULL;
}

// Releases the given decorator.
void DecoratorAliasInstancer::ReleaseDecorator(Decorator* decorator)
{
	decorator->RemoveReference();
}

// Releases the instancer.
void DecoratorAliasInstancer::Release()
{
	delete this;
}

}
}
