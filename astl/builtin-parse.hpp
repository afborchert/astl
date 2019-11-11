/*
   Copyright (C) 2019 Andreas Franz Borchert
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

#ifndef ASTL_BUILTIN_PARSE_HPP
#define ASTL_BUILTIN_PARSE_HPP

#include <memory>
#include <sstream>
#include <astl/attribute.hpp>
#include <astl/bindings.hpp>
#include <astl/exception.hpp>
#include <astl/execution.hpp>
#include <astl/rules.hpp>
#include <astl/sm-execution.hpp>
#include <astl/stream.hpp>
#include <astl/syntax-tree.hpp>
#include <astl/types.hpp>

namespace Astl {

/* helper function for builtin parse functions:
   a function is required for generating an AST from an input stream;
   example:

   return builtin_parse(bindings, args,
      [](NodePtr& root, InputStreamPtr istream) -> bool {
	 Scanner scanner(istream->get(), istream->get_name());
	 parser p(scanner, root);
	 return p.parse() == 0;
      });
*/

template<typename GenTreeFromInputStream>
AttributePtr builtin_parse(BindingsPtr bindings, AttributePtr args,
      GenTreeFromInputStream&& gentree) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for parse function");
   }
   AttributePtr at = args->get_value(0);
   if (at && at->get_type() == Attribute::istream) {
      auto istream = at->get_istream();
      NodePtr root;
      try {
	 if (!gentree(root, istream)) {
	    return nullptr;
	 }
	 return std::make_shared<Attribute>(root);
      } catch (Exception& e) {
	 return std::make_shared<Attribute>(std::string(e.what()));
      }
   } else {
      throw Exception("input stream expected as argument to parse");
   }
}

inline AttributePtr builtin_run_attribution_rules(BindingsPtr bindings,
      AttributePtr args) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for "
	 "run_attribution_rules function");
   }
   AttributePtr at = args->get_value(0);
   if (at && at->get_type() == Attribute::tree) {
      NodePtr root = at->get_node();
      if (bindings->rules_defined()) {
	 BindingsPtr local_bindings(bindings);
	 local_bindings->define("root", std::make_shared<Attribute>(root));
	 const Rules& rules(bindings->get_rules());
	 // execute global attribution rules, if defined
	 if (rules.attribution_rules_defined()) {
	    try {
	       execute(root, rules.get_attribution_rule_table(),
		  local_bindings);
	    } catch (Exception& e) {
	       throw Exception("error occurred while executing attribution "
		  "rules within the run_attribution_rules function", e);
	    }
	 }
      }
   } else {
      throw Exception("syntax tree expected as argument");
   }
   return nullptr;
}

inline AttributePtr builtin_run_state_machines(BindingsPtr bindings,
      AttributePtr args) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for "
	 "run_state_machines function");
   }
   AttributePtr at = args->get_value(0);
   if (at && at->get_type() == Attribute::tree) {
      if (bindings->rules_defined()) {
	 BindingsPtr local_bindings(bindings);
	 local_bindings->define("root", std::make_shared<Attribute>(at));
	 const Rules& rules(bindings->get_rules());
	 execute_state_machines(rules, local_bindings);
      }
   }
   return nullptr;
}

} // namespace Astl

#endif
