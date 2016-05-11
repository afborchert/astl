/*
   Copyright (C) 2009, 2010 Andreas Franz Borchert
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

#include <iostream>
#include <sstream>
#include <astl/bindings.hpp>
#include <astl/printer.hpp>
#include <astl/tree-expressions.hpp>
#include <astl/scanner.hpp>
#include <astl/syntax-tree.hpp>
#include <astl/operator.hpp>
#include <astl/context.hpp>
#include <astl/default-bindings.hpp>
#include <astl/operators.hpp>
#include <astl/expression.hpp>
#include <astl/rules.hpp>
#include <astl/cloner.hpp>
#include <astl/parenthesizer.hpp>

namespace Astl {

static bool recursive_print(std::ostream& out, const NodePtr root,
	 const RuleTable& rules, BindingsPtr bindings, unsigned int indent,
	 Context& context) throw(Exception);

static bool expand_variable(std::ostream& out, std::string name,
      const RuleTable& rules, unsigned int indent,
      BindingsPtr bindings, BindingsPtr local_bindings,
      Context& context) throw(Exception) {
   if (name.size() > 0 && local_bindings->defined(name)) {
      NodePtr node = local_bindings->get(name)->get_node();
      return recursive_print(out, node, rules, bindings, indent, context);
   } else {
      return false;
   }
}

static int get_indent(const std::string text) {
   int i = text.size() - 1;
   unsigned int indent = 0;
   while (i >= 0 && is_whitespace(text[i])) {
      if (text[i] == '\n') return indent;
      ++indent; --i;
   }
   while (i >= 0) {
      if (text[i] == '\n') return 0;
      --i;
   }
   return -1;
}

static void expand_text(std::ostream& out, const Token& t,
      unsigned int indent) {
   std::string text = t.get_text();
   for (int i = 0; i < text.size(); ++i) {
      char ch = text[i];
      out << ch;
      if (ch == '\n') {
	 for (int wi = 0; wi < indent; ++wi) {
	    out << ' ';
	 }
      }
   }
}

static bool recursive_print(std::ostream& out, const NodePtr root,
	 const RuleTable& rules, BindingsPtr bindings,
	 unsigned int indent,
	 Context& context) throw(Exception) {
   if (root->is_leaf()) {
      return !!(out << root->get_token().get_literal());
   } else {
      unsigned int arity = root->size();
      Operator op = root->get_op();
      BindingsPtr local_bindings;
      RuleTable::print_iterator it, end;
      int found = 0;
      for (it = rules.reversed_find(op, arity, end); it != end; ++it) {
	 ++found;
	 local_bindings = BindingsPtr(new Bindings(bindings));
	 if (matches(root, it->second->get_tree_expression(),
	       local_bindings, context)) break;
      }
      if (it == end) {
	 // try wildcard rules
	 for (it = rules.reversed_find(op, -1, end); it != end; ++it) {
	    ++found;
	    local_bindings = BindingsPtr(new Bindings(bindings));
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
	       arity << " parameters";
	    throw Exception(root->get_location(), os.str());
	 }
      }
      context.descend(root);
      const NodePtr& node = it->second->get_rhs();
      unsigned int add_indent = 0;
      for (int pi = 0; pi < node->size(); ++pi) {
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
		  assert(false);
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
	    for (int i = 1; i < list->size(); ++i) {
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
      const RuleTable& rules) throw(Exception) {
   Context context;
   return recursive_print(out, root, rules,
      create_default_bindings(root), 0, context);
}

bool print(std::ostream& out, const NodePtr root,
      const RuleTable& rules, BindingsPtr bindings) throw(Exception) {
   Context context;
   return recursive_print(out, root, rules, bindings, 0, context);
}

AttributePtr gen_text(const RuleTable& print_rules, NodePtr root,
      BindingsPtr bindings) throw(Exception) {
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
      return AttributePtr(new Attribute(os.str()));
   } else {
      return AttributePtr((Attribute*) 0);
   }
}

AttributePtr gen_text(NodePtr root,
      BindingsPtr bindings) throw(Exception) {
   if (!bindings->rules_defined()) {
      throw Exception("no print rules defined");
   }
   const Rules& rules(bindings->get_rules());
   if (!rules.print_rules_defined()) {
      throw Exception("no print rules defined");
   }
   const RuleTable& print_rules(rules.get_print_rule_table());
   gen_text(print_rules, root, bindings);
}

} // namespace Astl
