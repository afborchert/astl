/*
   Copyright (C) 2009-2019 Andreas Franz Borchert
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

#include <cstring>
#include <memory>
#include <astl/atrules-function.hpp>
#include <astl/attribute.hpp>
#include <astl/builtin-functions.hpp>
#include <astl/default-bindings.hpp>
#include <astl/prrules-function.hpp>
#include <astl/rules.hpp>
#include <astl/std-functions.hpp>
#include <astl/stream.hpp>
#include <astl/trrules-function.hpp>

extern char** environ;

namespace Astl {

/* create dictionary with environment variables */
AttributePtr create_environment() {
   auto dict = std::make_shared<Attribute>(Attribute::dictionary);
   for (char** envp = environ; *envp; ++envp) {
      char* start = *envp;
      char* cp = std::strchr(start, '=');
      if (cp) {
	 std::string key(start, cp - start);
	 auto value = std::make_shared<Attribute>(std::string(cp + 1));
	 dict->update(key, value);
      }
   }
   return dict;
}

BindingsPtr create_default_bindings(NodePtr root,
      const Rules* rulesp, BindingsPtr extra_bindings) {
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
      /* allow root to be redefined */
      bindings = std::make_shared<Bindings>(bindings);
      bindings->define("graph", std::make_shared<Attribute>());
   }
   // add "true" and "false"
   bindings->define("true", std::make_shared<Attribute>(true));
   bindings->define("false", std::make_shared<Attribute>(false));

   // add standard streams "stdin", "stdout" and "stderr"
   bindings->define("stdin",
      std::make_shared<Attribute>(std::make_shared<InputStream>(std::cin,
	 "stdin")));
   bindings->define("stdout",
      std::make_shared<Attribute>(std::make_shared<OutputStream>(std::cout,
         "stdout")));
   bindings->define("stderr",
      std::make_shared<Attribute>(std::make_shared<OutputStream>(std::cerr,
	 "stderr")));

   // add "env" as dictionary of environment variables
   bindings->define("env", create_environment());

   // add standard functions to bindings
   BuiltinFunctions bfs;
   insert_std_functions(bfs);
   bfs.insert(bindings);

   // add extra bindings, if any (usually standard functions of extensions
   if (extra_bindings) {
      bindings->merge(extra_bindings);
   }

   // add global bindings
   if (rulesp) {
      const FunctionTable& ftab = rulesp->get_function_table();
      for (auto it = ftab.begin(); it != ftab.end(); ++it) {
	 auto fnode = it->second;
	 FunctionPtr f;
	 if (fnode->size() == 2) {
	    /* variable number of parameters */
	    auto block = fnode->get_operand(1);
	    f = std::make_shared<RegularFunction>(block, bindings);
	 } else {
	    /* fixed number of parameters */
	    auto params = fnode->get_operand(1);
	    auto block = fnode->get_operand(2);
	    f = std::make_shared<RegularFunction>(block, bindings, params);
	 }
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

BindingsPtr create_default_bindings(NodePtr root,
      const Rules* rulesp) {
   return create_default_bindings(root, rulesp, nullptr);
}

BindingsPtr create_default_bindings(NodePtr root) {
   return create_default_bindings(root, nullptr);
}

BindingsPtr create_default_bindings() {
   return create_default_bindings(NodePtr(nullptr), nullptr);
}

} // namespace Astl
