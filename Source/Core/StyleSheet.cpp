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
#include "../../Include/Rocket/Core/StyleSheet.h"
#include "../../Include/Rocket/Core/Element.h"
#include "../../Include/Rocket/Core/PropertyDefinition.h"
#include "../../Include/Rocket/Core/StyleSheetSpecification.h"
#include "ElementDefinition.h"
#include "StyleSheetFactory.h"
#include "StyleSheetNode.h"
#include "StyleSheetParser.h"
#include <algorithm>
#include <vector>

namespace Rocket {
namespace Core {

// Sorts style nodes based on specificity.
static bool StyleSheetNodeSort(const StyleSheetNode* lhs, const StyleSheetNode* rhs)
{
	return lhs->GetSpecificity() < rhs->GetSpecificity();
}

StyleSheet::StyleSheet()
{
	root = new StyleSheetNode("", StyleSheetNode::ROOT);
	specificity_offset = 0;
}

StyleSheet::~StyleSheet()
{
	delete root;

	// Release our reference count on the cached element definitions.
	for (ElementDefinitionCache::iterator cache_iterator = address_cache.begin(); cache_iterator != address_cache.end(); cache_iterator++)
		(*cache_iterator).second->RemoveReference();

	for (ElementDefinitionCache::iterator cache_iterator = node_cache.begin(); cache_iterator != node_cache.end(); cache_iterator++)
		(*cache_iterator).second->RemoveReference();
}

bool StyleSheet::LoadStyleSheet(Stream* stream)
{
	StyleSheetParser parser;
	specificity_offset = parser.Parse(root, stream);
	return specificity_offset >= 0;
}

/// Combines this style sheet with another one, producing a new sheet
StyleSheet* StyleSheet::CombineStyleSheet(const StyleSheet* other_sheet) const
{
	StyleSheet* new_sheet = new StyleSheet();
	if (!new_sheet->root->MergeHierarchy(root) ||
		!new_sheet->root->MergeHierarchy(other_sheet->root, specificity_offset))
	{
		delete new_sheet;
		return NULL;
	}

	new_sheet->specificity_offset = specificity_offset + other_sheet->specificity_offset;
	return new_sheet;
}

// Builds the node index for a combined style sheet.
void StyleSheet::BuildNodeIndex()
{
	if (complete_node_index.empty())
	{
		styled_node_index.clear();
		complete_node_index.clear();

		root->BuildIndex(styled_node_index, complete_node_index);
	}
}

// Returns the compiled element definition for a given element hierarchy.
ElementDefinition* StyleSheet::GetElementDefinition(const Element* element) const
{
	// Address cache is disabled for the time being; this doesn't work since the introduction of structural
	// pseudo-classes.
	ElementDefinitionCache::iterator cache_iterator;
#if 0
	String element_address = element->GetAddress();

	// Look the address up in the definition, see if we've processed a similar element before.
	cache_iterator = address_cache.find(element_address);
	if (cache_iterator != address_cache.end())
	{
		ElementDefinition* definition = (*cache_iterator).second;
		definition->AddReference();
		return definition;
	}
#endif
	// See if there are any styles defined for this element.
	std::vector< const StyleSheetNode* > applicable_nodes;

	String tags[] = {element->GetTagName(), ""};
	for (int i = 0; i < 2; i++)
	{
		NodeIndex::const_iterator iterator = styled_node_index.find(tags[i]);
		if (iterator != styled_node_index.end() && (*iterator).second.size())
		{
			const NodeList& nodes = (*iterator).second;

			NodeList::const_iterator end = nodes.end();

			// There are! Now see if we satisfy all of their parenting requirements. What this involves is traversing the style
			// nodes backwards, trying to match nodes in the element's hierarchy to nodes in the style hierarchy.
			for (NodeList::const_iterator iterator = nodes.begin(); iterator != end; iterator++)
			{
				if ((*iterator)->IsApplicable(element))
				{
					// Get the node to add any of its non-tag children that we match into our list.
					(*iterator)->GetApplicableDescendants(applicable_nodes, element);
				}
			}
		}
	}

	std::sort(applicable_nodes.begin(), applicable_nodes.end(), StyleSheetNodeSort);

	// Compile the list of volatile pseudo-classes for this element definition.
	PseudoClassList volatile_pseudo_classes;
	bool structurally_volatile = false;

	for (int i = 0; i < 2; ++i)
	{
		NodeIndex::const_iterator iterator = complete_node_index.find(tags[i]);
		if (iterator != complete_node_index.end() && (*iterator).second.size())
		{
			const NodeList& nodes = (*iterator).second;
			NodeList::const_iterator end = nodes.end();

			// See if we satisfy all of the parenting requirements for each of these nodes (as in the previous loop).
			for (NodeList::const_iterator iterator = nodes.begin(); iterator != end; iterator++)
			{
				structurally_volatile |= (*iterator)->IsStructurallyVolatile();

				if ((*iterator)->IsApplicable(element))
				{
					std::vector< const StyleSheetNode* >::Type volatile_nodes;
					(*iterator)->GetApplicableDescendants(volatile_nodes, element);

					for (size_t i = 0; i < volatile_nodes.size(); ++i)
						volatile_nodes[i]->GetVolatilePseudoClasses(volatile_pseudo_classes);
				}
			}
		}
	}

	// If this element definition won't actually store any information, don't bother with it.
	if (applicable_nodes.empty() &&
		volatile_pseudo_classes.empty() &&
		!structurally_volatile)
		return NULL;

	// Check if this puppy has already been cached in the node index; it may be that it has already been created by an
	// element with a different address but an identical output definition.
	String node_ids;
	for (size_t i = 0; i < applicable_nodes.size(); i++)
		node_ids += String(10, "%x ", applicable_nodes[i]);
	for (PseudoClassList::iterator i = volatile_pseudo_classes.begin(); i != volatile_pseudo_classes.end(); ++i)
		node_ids += String(32, ":%s", (*i).CString());

	cache_iterator = node_cache.find(node_ids);
	if (cache_iterator != node_cache.end())
	{
		ElementDefinition* definition = (*cache_iterator).second;
		definition->AddReference();
		return definition;
	}

	// Create the new definition and add it to our cache. One reference count is added, bringing the total to two; one
	// for the element that requested it, and one for the cache.
	ElementDefinition* new_definition = new ElementDefinition();
	new_definition->Initialise(applicable_nodes, volatile_pseudo_classes, structurally_volatile);

	// Add to the address cache.
#if 0
	address_cache[element_address] = new_definition;
	new_definition->AddReference();
#endif
	// Add to the node cache.
	node_cache[node_ids] = new_definition;
	new_definition->AddReference();

	return new_definition;
}

const PropertyDictionary* StyleSheet::FindDecoratorPropertiesWithId(const String& class_name, const String& decorator_id) const
{
    StyleSheetNode *node = root->GetChildNode(class_name, StyleSheetNode::CLASS, false);
    if (!node) {
      StyleSheetNode *global = root->GetChildNode("", StyleSheetNode::TAG, false);
      if (global)
            node = global->GetChildNode(class_name, StyleSheetNode::CLASS, false);
    }
    if (node) {
        // Core::Log::Message(Core::Log::LT_DEBUG, "class %s found", class_name.CString());
        PropertyGroupMap decorator_definitions;
        BuildPropertyGroup(decorator_definitions, "decorator", node->GetProperties());
        for (PropertyGroupMap::const_iterator i=decorator_definitions.begin(); i!=decorator_definitions.end(); ++i) {
            // Core::Log::Message(Core::Log::LT_DEBUG, " -- group: %s found",(*i).first.CString());
            const PropertyGroup &group = (*i).second;
            // Core::Log::Message(Core::Log::LT_DEBUG, "  -- prop: %s", group.first.CString());
            const PropertyMap &props = group.second.GetProperties();
            // for (PropertyMap::const_iterator ii=props.begin(); ii!=props.end(); ++ii)
            //   Core::Log::Message(Core::Log::LT_DEBUG, "   -- value: %s", (*ii).first.CString());
            PropertyMap::const_iterator deco_id = props.find("decorator-id");
            if (deco_id != props.end() && (*deco_id).second.ToString() == decorator_id) {
                // Core::Log::Message(Core::Log::LT_DEBUG, "decorator %s definiton found", decorator_id.CString());
                return new PropertyDictionary(group.second);
            }
        }
    }

    return NULL;
}

// Destroys the style sheet.
void StyleSheet::OnReferenceDeactivate()
{
	delete this;
}

}
}
