/*
   Copyright (C) 2009 Andreas Franz Borchert
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
#include <astl/builtin-functions.hpp>
#include <astl/default-bindings.hpp>
#include <astl/prrules-function.hpp>
#include <astl/rules.hpp>
#include <astl/std-functions.hpp>
#include <astl/trrules-function.hpp>

namespace Astl {

BindingsPtr create_default_bindings(NodePtr root,
      const Rules* rulesp) throw(Exception){
   auto bindings = std::make_shared<Bindings>(rulesp);
   // add "root" and "graph"
   if (root) {
      bindings->define("root", std::make_shared<Attribute>(root));
      AttributePtr at = root->get_attribute();
      // create root.graph as dictionary, if it does not exist yet
      // and make graph an alias for root.graph
      if (!at->is_defined("graph")) {
	 at->update("graph", std::make_shared<Attribute>());
      }
      bindings->define("graph", at->get_value("graph"));
   } else {
      bindings->define("root", AttributePtr(nullptr));
      bindings->define("graph", AttributePtr(nullptr));
   }
   // add "true" and "false"
   bindings->define("true", std::make_shared<Attribute>(true));
   bindings->define("false", std::make_shared<Attribute>(false));

   // add standard functions to bindings
   BuiltinFunctions bfs;
   insert_std_functions(bfs);
   bfs.insert(bindings);

   // add global bindings
   if (rulesp) {
      const FunctionTable& ftab = rulesp->get_function_table();
      for (FunctionTable::const_iterator it = ftab.begin();
	    it != ftab.end(); ++it) {
	 FunctionPtr f =
	    std::make_shared<RegularFunction>(it->second, bindings);
	 if (!bindings->define(it->first, std::make_shared<Attribute>(f))) {
	    throw Exception(it->second->get_location(),
	       "multiply defined: " + it->first);
	 }
      }
      const NamedRulesTable& ntrtab =
	 rulesp->get_named_transformation_rule_tables();
      for (NamedRulesTable::const_iterator it = ntrtab.begin();
	    it != ntrtab.end(); ++it) {
	 FunctionPtr f =
	    std::make_shared<TransformationRuleSetFunction>(it->second,
	       bindings);
	 if (!bindings->define(it->first, std::make_shared<Attribute>(f))) {
	    throw Exception("multiply defined: " + it->first);
	 }
      }
      const NamedRulesTable& niptrtab =
	 rulesp->get_named_inplace_transformation_rule_tables();
      for (NamedRulesTable::const_iterator it = niptrtab.begin();
	    it != niptrtab.end(); ++it) {
	 FunctionPtr f = std::make_shared<InplaceTransformationRuleSetFunction>(
	    it->second, bindings);
	 if (!bindings->define(it->first, std::make_shared<Attribute>(f))) {
	    throw Exception("multiply defined: " + it->first);
	 }
      }
      const NamedRulesTable& nrtab =
	 rulesp->get_named_attribution_rule_tables();
      for (NamedRulesTable::const_iterator it = nrtab.begin();
	    it != nrtab.end(); ++it) {
	 FunctionPtr f =
	    std::make_shared<AttributionRuleSetFunction>(it->second, bindings);
	 if (!bindings->define(it->first, std::make_shared<Attribute>(f))) {
	    throw Exception("multiply defined: " + it->first);
	 }
      }
      const NamedRulesTable& nrptab =
	 rulesp->get_named_print_rule_tables();
      for (NamedRulesTable::const_iterator it = nrptab.begin();
	    it != nrptab.end(); ++it) {
	 FunctionPtr f =
	    std::make_shared<PrintRuleSetFunction>(it->second, bindings);
	 if (!bindings->define(it->first, std::make_shared<Attribute>(f))) {
	    throw Exception("multiply defined: " + it->first);
	 }
      }
   }

   // make all global identifiers constant
   bindings->mk_all_const();

   // and return a scope which is nested within the
   // scope with the default bindings to avoid conflicts
   return std::make_shared<Bindings>(bindings);
}

BindingsPtr create_default_bindings(NodePtr root) throw(Exception) {
   return create_default_bindings(root, nullptr);
}

BindingsPtr create_default_bindings() throw(Exception) {
   return create_default_bindings(NodePtr(nullptr), nullptr);
}

} // namespace Astl
