/*
   Copyright (C) 2010, 2011 Andreas Franz Borchert
   ----------------------------------------------------------------------------
   The Astl Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either version
   2 of the License, or (at your option) any later version.

   The Astl Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <memory>
#include <astl/attribute.hpp>
#include <astl/bindings.hpp>
#include <astl/exception.hpp>
#include <astl/execution.hpp>
#include <astl/syntax-tree.hpp>
#include <astl/trrules-function.hpp>

namespace Astl {

TransformationRuleSetFunction::TransformationRuleSetFunction(
      RuleTablePtr rt, BindingsPtr bindings) :
      Function(bindings), rt(rt) {
}

AttributePtr TransformationRuleSetFunction::eval(AttributePtr args) const {
   NodePtr root;
   if (!args || args->size() == 0) {
      AttributePtr rootat = bindings->get("root");
      if (rootat && rootat->get_type() == Attribute::tree) {
	 root = rootat->get_node();
      }
   } else if (args->size() > 1) {
      throw Exception(
	 "wrong number of arguments for a set of transformation rules");
   } else if (args->get_type() == Attribute::list &&
	 args->size() == 1) {
      AttributePtr element = args->get_value(0);
      if (element && element->get_type() == Attribute::tree) {
	 root = element->get_node();
      }
   }
   if (!root) {
      throw Exception("abstract syntax tree expected as parameter");
   }
   RuleTable& rtab(*rt);
   BindingsPtr local_bindings(bindings);
   local_bindings->define("root", std::make_shared<Attribute>(root));
   CandidateSet candidates(root, rtab, local_bindings);
   AttributePtr list = std::make_shared<Attribute>(Attribute::list);
   for (std::size_t i = 0; i < candidates.size(); ++i) {
      NodePtr newroot = candidates[i]->transform();
      list->push_back(std::make_shared<Attribute>(newroot));
   }
   return list;
}

InplaceTransformationRuleSetFunction::InplaceTransformationRuleSetFunction(
      RuleTablePtr rt, BindingsPtr bindings) :
      Function(bindings), rt(rt) {
}

AttributePtr InplaceTransformationRuleSetFunction::eval(AttributePtr args)
      const {
   NodePtr root;
   if (!args || args->size() == 0) {
      AttributePtr rootat = bindings->get("root");
      if (rootat && rootat->get_type() == Attribute::tree) {
	 root = rootat->get_node();
      }
   } else if (args->size() > 1) {
      throw Exception(
	 "wrong number of arguments for a set of transformation rules");
   } else if (args->get_type() == Attribute::list &&
	 args->size() == 1) {
      AttributePtr element = args->get_value(0);
      if (element && element->get_type() == Attribute::tree) {
	 root = element->get_node();
      }
   }
   if (!root) {
      throw Exception("abstract syntax tree expected as parameter");
   }
   RuleTable& rtab(*rt);
   BindingsPtr local_bindings(bindings);
   local_bindings->define("root", std::make_shared<Attribute>(root));
   CandidateSet candidates(root, rtab, local_bindings);
   candidates.suppress_transformation_conflicts();
   for (std::size_t i = 0; i < candidates.size(); ++i) {
      candidates[i]->transform_inplace();
   }
   return std::make_shared<Attribute>(candidates.size());
}

} // namespace Astl
