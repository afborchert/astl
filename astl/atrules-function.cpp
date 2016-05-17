/*
   Copyright (C) 2010 Andreas Franz Borchert
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
#include <astl/atrules-function.hpp>
#include <astl/attribute.hpp>
#include <astl/execution.hpp>

namespace Astl {

AttributionRuleSetFunction::AttributionRuleSetFunction(RuleTablePtr rt_param,
      BindingsPtr bindings_param) :
      Function(bindings_param), rt(rt_param) {
}

AttributePtr AttributionRuleSetFunction::eval(AttributePtr args)
      const throw(Exception) {
   NodePtr root;
   if (!args || args->size() == 0) {
      AttributePtr rootat = bindings->get("root");
      if (rootat && rootat->get_type() == Attribute::tree) {
	 root = rootat->get_node();
      }
   } else if (args->size() > 1) {
      throw Exception(
	 "wrong number of arguments for a set of attribution rules");
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
   execute(root, rtab, local_bindings);
   return AttributePtr(nullptr);
}

} // namespace Astl
