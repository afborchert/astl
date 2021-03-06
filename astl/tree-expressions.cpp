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

#include <cassert>
#include <memory>
#include <sstream>
#include <astl/bindings.hpp>
#include <astl/exception.hpp>
#include <astl/expression.hpp>
#include <astl/location.hpp>
#include <astl/operators.hpp>
#include <astl/opset.hpp>
#include <astl/regex.hpp>
#include <astl/tree-expressions.hpp>

namespace Astl {

static void bind_variable(BindingsPtr bindings, const Location& loc,
      const std::string& varname, NodePtr tree) {
   if (!bindings->define(varname, std::make_shared<Attribute>(tree))) {
      std::ostringstream os;
      os << "multiply defined variable: " << varname;
      throw Exception(loc, os.str());
   }
}

static void bind_variable(BindingsPtr bindings,
      const Location& loc, const std::string& varname, AttributePtr at) {
   if (!bindings->define(varname, at)) {
      std::ostringstream os;
      os << "multiply defined variable: " << varname;
      throw Exception(loc, os.str());
   }
}

static bool recursive_matches(NodePtr root, NodePtr expression,
      BindingsPtr bindings, Context& context) {
   while (!expression->is_leaf() &&
	 expression->get_op() == Op::named_tree_expression) {
      /* tree_expression AS IDENT construct */
      assert(expression->size() == 2);
      NodePtr name = expression->get_operand(1);
      assert(name->is_leaf());
      std::string varname = name->get_token().get_text();
      bind_variable(bindings, name->get_location(), varname, root);
      expression = expression->get_operand(0);
   }
   if (expression->is_leaf()) {
      std::string varname = expression->get_token().get_text();
      if (bindings->defined(varname)) {
	 /* already defined */
	 AttributePtr at = bindings->get(varname);
	 if (!at) {
	    return false;
	 } else if (at->get_type() == Attribute::tree) {
	    return at->get_node()->deep_tree_equality(root);
	 } else if (root->is_leaf()) {
	    return at->convert_to_string() == root->get_token().get_text();
	 } else {
	    return false;
	 }
      } else {
	 /* place holder */
	 bind_variable(bindings, expression->get_location(), varname, root);
      }
   } else if (expression->get_op() == Op::regular_expression) {
      if (!root->is_leaf()) return false;
      NodePtr name;
      NodePtr regexp;
      if (expression->size() == 1) {
	 regexp = expression->get_operand(0);
      } else {
	 assert(expression->size() == 2);
	 name = expression->get_operand(0);
	 assert(name->is_leaf());
	 regexp = expression->get_operand(1);
      }
      assert(regexp->is_leaf());

      std::string pattern = regexp->get_token().get_text();
      Regex re(regexp->get_location(), pattern);
      std::string literal = root->get_token().get_literal();
      AttributePtr match_result = re.match(literal);
      if (match_result == nullptr) return false;
      if (name) {
	 std::string varname(name->get_token().get_text());
	 bind_variable(bindings, name->get_location(), varname, match_result);
      }
   } else if (expression->get_op() == Op::string_literal) {
      if (!root->is_leaf()) return false;
      std::string literal = root->get_token().get_literal();
      std::string expected = expression->get_operand(0)->get_token().get_text();
      if (literal != expected) return false;
   } else {
      /* tree expression */
      if (root->is_leaf()) return false;
      // check for variable length
      bool variable_length = false;
      NodePtr valist;
      NodePtr remaining_subnodes;
      if (expression->get_op() == Op::variable_length_tree_expression) {
	 variable_length = true;
	 if (expression->size() >= 2) {
	    valist = expression->get_operand(1);
	    if (expression->size() == 3) {
	       remaining_subnodes = expression->get_operand(2);
	    }
	 }
	 expression = expression->get_operand(0);
      }
      // does the operator match?
      OperatorSet opset(expression->get_operand(0), bindings->get_rules());
      if (!opset.includes(root->get_op())) return false;
      // does the arity match?
      std::size_t arity = expression->size() - 1;
      std::size_t remaining_arity = 0;
      if (remaining_subnodes) {
	 remaining_arity = remaining_subnodes->size();
      }
      if (variable_length) {
	 if (root->size() < arity + remaining_arity) return false;
      } else {
	 if (root->size() != arity) return false;
      }
      // do the subtrees match?
      for (std::size_t i = 0; i < arity; ++i) {
	 if (!recursive_matches(root->get_operand(i),
	       expression->get_operand(i+1), bindings, context)) {
	    return false;
	 }
      }
      if (remaining_subnodes) {
	 std::size_t root_arity = root->size();
	 for (std::size_t i = 0; i < remaining_arity; ++i) {
	    int j = root_arity - remaining_arity + i;
	    if (!recursive_matches(root->get_operand(j),
		  remaining_subnodes->get_operand(i), bindings, context)) {
	       return false;
	    }
	 }
      }
      std::size_t remaining = root->size() - arity - remaining_arity;
      if (variable_length && valist) {
	 std::string varname = valist->get_token().get_text();
	 if (bindings->defined(varname)) {
	    /* already defined */
	    AttributePtr at = bindings->get(varname);
	    if (!at || at->get_type() != Attribute::list) return false;
	    if (at->size() != remaining) return false;
	    for (std::size_t i = 0; i < remaining; ++i) {
	       AttributePtr element = at->get_value(i);
	       NodePtr node = root->get_operand(arity + i);
	       if (!element) return false;
	       if (element->get_type() == Attribute::tree) {
		  if (!element->get_node()->deep_tree_equality(node)) {
		     return false;
		  }
	       } else if (node->is_leaf()) {
		  if (element->convert_to_string() !=
			node->get_token().get_text()) {
		     return false;
		  }
	       } else {
		  return false;
	       }
	    }
	 } else {
	    /* place holder */
	    AttributePtr list = std::make_shared<Attribute>(Attribute::list);
	    for (std::size_t i = 0; i < remaining; ++i) {
	       NodePtr node = root->get_operand(arity + i);
	       list->push_back(std::make_shared<Attribute>(node));
	    }
	    bind_variable(bindings, expression->get_location(), varname, list);
	 }
      }
   }
   return true;
}

bool matches(NodePtr root, NodePtr expression,
      BindingsPtr bindings, Context& context) {
   BindingsPtr local_bindings = std::make_shared<Bindings>(bindings);
   assert(!expression->is_leaf());
   NodePtr where_expression = NodePtr(nullptr);
   if (expression->get_op() == Op::conditional_tree_expression) {
      where_expression = expression->get_operand(1);
      expression = expression->get_operand(0);
   }
   if (expression->get_op() == Op::contextual_tree_expression) {
      assert(expression->size() == 2);
      NodePtr context_expression = expression->get_operand(1);
      NodePtr next_expression; bool next_defined = false;
      bool first_context_expr = true;
      do {
	 // move to next context expression in the chain
	 NodePtr matched_node;
	 if (next_defined) {
	    next_defined = false;
	    context_expression = next_expression;
	 }
	 // check for negation
	 bool expected_result = true;
	 if (!context_expression->is_leaf() &&
	       context_expression->get_op() == Op::NOT) {
	    expected_result = false;
	    context_expression = context_expression->get_operand(0);
	 }
	 // check if further context expressions are coming
	 if (!context_expression->is_leaf() &&
	       context_expression->get_op() == Op::context_expression) {
	    next_expression = context_expression->get_operand(1);
	    next_defined = true;
	    context_expression = context_expression->get_operand(0);
	 }
	 // perform the actual check for the current context expression
	 assert(!context_expression->is_leaf() &&
	    context_expression->get_op() == Op::context_match);
	 context_expression = context_expression->get_operand(0);
	 bool result;
	 if (first_context_expr || !expected_result) {
	    result = context.matches(context_expression, local_bindings, root);
	    first_context_expr = false;
	 } else {
	    result = context.and_matches(context_expression, local_bindings,
	       root);
	 }
	 if (result != expected_result) {
	    return false;
	 }
      } while (next_defined);
      expression = expression->get_operand(0);
   }
   assert((expression->get_op() == Op::tree_expression ||
	 expression->get_op() == Op::variable_length_tree_expression ||
         expression->get_op() == Op::named_tree_expression) &&
	 expression->size() > 0);
   if (recursive_matches(root, expression, local_bindings, context)) {
      if (where_expression) {
	 Expression e(where_expression, local_bindings);
	 if (!e.get_result()->convert_to_bool()) {
	    return false;
	 }
      }
      bindings->merge(local_bindings);
      return true;
   } else {
      return false;
   }
}

NodePtr gen_tree(NodePtr troot, BindingsPtr bindings) {
   bool named = false;
   std::string name;
   NodePtr newroot;
   if (!troot->is_leaf() &&
	 (troot->get_op() == Op::named_expression ||
	 troot->get_op() == Op::named_tree_expression_constructor)) {
      name = troot->get_operand(1)->get_token().get_text();
      troot = troot->get_operand(0);
      named = true;
   }
   if (troot->is_leaf()) {
      /* variable */
      std::string varname = troot->get_token().get_text();
      if (bindings->defined(varname)) {
	 newroot = bindings->get(varname)->get_node();
      } else {
	 std::ostringstream os;
	 os << "undefined variable in replacement tree: " << varname;
	 throw Exception(troot->get_location(), os.str());
      }
   } else if (troot->get_op() == Op::expression) {
      Expression expr(troot, bindings);
      newroot = expr.convert_to_node();
   } else {
      assert(troot->get_op() == Op::tree_expression);
      std::string opname = troot->get_operand(0)->get_token().get_text();
      Operator op(opname);
      newroot = std::make_shared<Node>(troot->get_location(), op);
      for (std::size_t i = 1; i < troot->size(); ++i) {
	 NodePtr subnode = troot->get_operand(i);
	 if (!subnode->is_leaf() && subnode->get_op() == Op::subnode_list) {
	    NodePtr listexpr = subnode->get_operand(0);
	    AttributePtr valist;
	    if (listexpr->is_leaf()) {
	       /* variable name that must be bound to a
		  list of syntax tree nodes */
	       std::string varname = listexpr->get_token().get_text();
	       if (!bindings->defined(varname)) {
		  std::ostringstream os;
		  os << "undefined variable in replacement tree: " << varname;
		  throw Exception(listexpr->get_location(), os.str());
	       }
	       valist = bindings->get(varname);
	    } else {
	       /* expression that must return a
		  list of syntax tree nodes */
	       assert(listexpr->get_op() == Op::expression);
	       Expression expr(listexpr, bindings);
	       valist = expr.get_result();
	       if (!valist) {
		  throw Exception(listexpr->get_location(),
		     "null returned by expression in replacement tree");
	       }
	    }
	    if (valist->get_type() == Attribute::list) {
	       for (std::size_t j = 0; j < valist->size(); ++j) {
		  AttributePtr element = valist->get_value(j);
		  if (element && element->get_type() == Attribute::tree) {
		     *newroot += valist->get_value(j)->get_node();
		  } else {
		     std::ostringstream os;
		     os << "list member " << j << " is not a tree";
		     throw Exception(listexpr->get_location(), os.str());
		  }
	       }
	    } else {
	       throw Exception(listexpr->get_location(),
		  "list of syntax tree nodes expected");
	    }
	 } else {
	    *newroot += gen_tree(subnode, bindings);
	 }
      }
   }
   if (named) {
      bindings->define(name, std::make_shared<Attribute>(newroot));
   }
   return newroot;
}

} // namespace Astl
