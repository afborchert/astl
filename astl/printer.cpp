/*
   Copyright (C) 2009, 2010, 2016 Andreas Franz Borchert
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

#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <astl/bindings.hpp>
#include <astl/cloner.hpp>
#include <astl/context.hpp>
#include <astl/default-bindings.hpp>
#include <astl/expression.hpp>
#include <astl/operator.hpp>
#include <astl/operators.hpp>
#include <astl/parenthesizer.hpp>
#include <astl/printer.hpp>
#include <astl/rules.hpp>
#include <astl/scanner.hpp>
#include <astl/syntax-tree.hpp>
#include <astl/tree-expressions.hpp>
#include <astl/utf8.hpp>

namespace Astl {

static constexpr int TAB_STOP = 8;

static bool recursive_print(std::ostream& out, const NodePtr root,
	 const RuleTable& rules, BindingsPtr bindings, std::size_t indent,
	 Context& context);

static bool expand_variable(std::ostream& out, std::string name,
      const RuleTable& rules, std::size_t indent,
      BindingsPtr bindings, BindingsPtr local_bindings,
      Context& context) {
   if (name.size() > 0 && local_bindings->defined(name)) {
      NodePtr node = local_bindings->get(name)->get_node();
      return recursive_print(out, node, rules, bindings, indent, context);
   } else {
      return false;
   }
}

static int get_indent(const std::string text) {
   auto range = codepoint_range(text);
   auto it = range.end();
   std::size_t indent = 0;
   while (it != range.begin()) {
      auto ch = *--it;
      if (!is_whitespace(ch)) break;
      if (ch == '\n') return indent;
      if (ch == '\t') {
	 indent += TAB_STOP - indent % TAB_STOP;
      } else {
	 ++indent;
      }
   }
   while (it != range.begin()) {
      auto ch = *--it;
      if (ch == '\n') return 0;
   }
   return -1;
}

static void expand_text(std::ostream& out, const Token& t,
      std::size_t indent) {
   std::string text = t.get_text();
   for (auto ch: codepoint_range(text)) {
      out << ch;
      if (ch == '\n') {
	 std::size_t wi = 0;
	 while (wi < indent) {
	    if (indent - wi >= TAB_STOP) {
	       out << '\t'; wi += TAB_STOP;
	    } else {
	       out << ' '; ++wi;
	    }
	 }
      }
   }
}

static bool recursive_print(std::ostream& out, const NodePtr root,
	 const RuleTable& rules, BindingsPtr bindings,
	 std::size_t indent,
	 Context& context) {
   if (root->is_leaf()) {
      return !!(out << root->get_token().get_literal());
   } else {
      Arity arity(root->size());
      Operator op = root->get_op();
      BindingsPtr local_bindings;
      RuleTable::print_iterator it, end;
      int found = 0;
      for (it = rules.reversed_find(op, arity, end); it != end; ++it) {
	 ++found;
	 local_bindings = std::make_shared<Bindings>(bindings);
	 if (matches(root, it->second->get_tree_expression(),
	       local_bindings, context)) break;
      }
      if (it == end) {
	 // try wildcard rules
	 for (it = rules.reversed_find(op, Arity(), end);
	       it != end; ++it) {
	    ++found;
	    local_bindings = std::make_shared<Bindings>(bindings);
	    if (matches(root, it->second->get_tree_expression(),
		  local_bindings, context)) break;
	 }
	 if (it == end) {
	    std::ostringstream os;
	    if (found > 0) {
	       os << "no matching ";
	    } else {
	       os << "no ";
	    }
	    os << "rule found for '" << op.get_name() << "' with " <<
	       root->size() << " parameters";
	    throw Exception(root->get_location(), os.str());
	 }
      }
      context.descend(root);
      const NodePtr& node = it->second->get_rhs();
      std::size_t add_indent = 0;
      for (std::size_t pi = 0; pi < node->size(); ++pi) {
	 const NodePtr& subnode = node->get_operand(pi);
	 if (subnode->is_leaf()) {
	    Token t = subnode->get_token();
	    switch (t.get_tokenval()) {
	       case parser::token::TEXT_LITERAL: {
		     expand_text(out, t, indent);
		     int new_indent = get_indent(t.get_text());
		     if (new_indent >= 0) add_indent = new_indent;
		     break;
		  }
	       case parser::token::VARIABLE:
		  if (!expand_variable(out, t.get_text(), rules,
			indent + add_indent,
			bindings, local_bindings, context)) {
		     std::ostringstream os;
		     os << "undefined variable in replacement text: " <<
			t.get_text();
		     throw Exception(subnode->get_location(), os.str());
		  }
		  break;
	       default:
		  assert(false); std::abort();
	    }
	 } else if (subnode->get_op() == Op::print_expression_listvar) {
	    std::string varname =
	       subnode->get_operand(0)->get_token().get_text();
	    if (!local_bindings->defined(varname)) {
	       std::ostringstream os;
	       os << "undefined variable in replacement list: " <<
		  varname;
	       throw Exception(subnode->get_location(), os.str());
	    }
	    AttributePtr list = local_bindings->get(varname);
	    if (list->get_type() != Attribute::list) {
	       std::ostringstream os;
	       os << "list expected: " << varname;
	       throw Exception(subnode->get_location(), os.str());
	    }
	    if (list->size() > 0) {
	       recursive_print(out, list->get_value(0)->get_node(),
		  rules, bindings, indent + add_indent, context);
	    }
	    for (std::size_t i = 1; i < list->size(); ++i) {
	       if (subnode->size() == 2) {
		  Token t = subnode->get_operand(1)->get_token();
		  expand_text(out, t, indent);
		  int new_indent = get_indent(t.get_text());
		  if (new_indent >= 0) add_indent = new_indent;
	       }
	       recursive_print(out, list->get_value(i)->get_node(),
		  rules, bindings, indent + add_indent, context);
	    }
	 } else {
	    assert(subnode->get_op() == Op::expression);
	    Expression expr(subnode, local_bindings);
	    if (!recursive_print(out, expr.convert_to_node(),
		  rules, bindings, indent, context)) {
	       return false;
	    }
	 }
      }
   }
   context.ascend();
   return true;
}

bool print(std::ostream& out, const NodePtr root,
      const RuleTable& rules) {
   Context context;
   return recursive_print(out, root, rules,
      create_default_bindings(root), 0, context);
}

bool print(std::ostream& out, const NodePtr root,
      const RuleTable& rules, BindingsPtr bindings) {
   Context context;
   return recursive_print(out, root, rules, bindings, 0, context);
}

AttributePtr gen_text(const RuleTable& print_rules, NodePtr root,
      BindingsPtr bindings) {
   const Rules& rules(bindings->get_rules());
   std::ostringstream os;
   if (rules.operator_rules_defined()) {
      NodePtr cloned_root = clone_including_attributes(root);
      /*
	 the parentheses operator can be configured through
	 the operator set named "parentheses";
	 Op::LPAREN is taken by default
      */
      OperatorSetPtr parenthesesSet = rules.get_opset("parentheses");
      Operator parentheses;
      if (parenthesesSet && parenthesesSet->get_card() == 1) {
	 OperatorSet::Iterator it = parenthesesSet->begin();
	 parentheses = Operator(*it);
      } else {
	 parentheses = Op::LPAREN;
      }
      parenthesize(cloned_root, rules.get_operator_table(), parentheses);
      root = cloned_root;
   }
   if (print(os, root, print_rules, bindings)) {
      return std::make_shared<Attribute>(os.str());
   } else {
      return AttributePtr(nullptr);
   }
}

AttributePtr gen_text(NodePtr root, BindingsPtr bindings) {
   if (!bindings->rules_defined()) {
      throw Exception("no print rules defined");
   }
   const Rules& rules(bindings->get_rules());
   if (!rules.print_rules_defined()) {
      throw Exception("no print rules defined");
   }
   const RuleTable& print_rules(rules.get_print_rule_table());
   return gen_text(print_rules, root, bindings);
}

} // namespace Astl
