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
#include <astl/rule.hpp>
#include <astl/operator.hpp>
#include <astl/operators.hpp>
#include <astl/syntax-tree.hpp>

namespace Astl {

// ==== BasicRule =============================================================

BasicRule::BasicRule(const Rules& rules_param) :
   rules(rules_param), arity(0), type(prefix) {
}

BasicRule::BasicRule(NodePtr tree_expr_param, const Rules& rules_param) :
      rules(rules_param), tree_expr(tree_expr_param),
      arity(0), type(prefix) {
   assert(tree_expr && !tree_expr->is_leaf());
   NodePtr node = tree_expr;
   if (node->get_op() == Op::PRE) {
      tree_expr = node = node->get_operand(0);
   } else if (node->get_op() == Op::POST) {
      tree_expr = node = node->get_operand(0);
      type = postfix;
   }
   if (node->get_op() == Op::conditional_tree_expression) {
      node = node->get_operand(0);
   }
   if (node->get_op() == Op::contextual_tree_expression) {
      node = node->get_operand(0);
   }
   if (node->get_op() == Op::named_tree_expression) {
      node = node->get_operand(0);
   }
   if (node->get_op() == Op::variable_length_tree_expression) {
      arity = Arity();
      node = node->get_operand(0);
   }
   assert(node->get_op() == Op::tree_expression);
   assert(node->size() >= 1);
   if (arity.fixed) {
      arity.arity = node->size() - 1;
   }
   node = node->get_operand(0);
   opset = std::make_shared<OperatorSet>(node, rules);
}

OperatorSetPtr BasicRule::get_opset() const {
   return opset;
}

Arity BasicRule::get_arity() const {
   return arity;
}

const NodePtr BasicRule::get_tree_expression() const {
   return tree_expr;
}

const BasicRule::Type BasicRule::get_type() const {
   return type;
}

// ==== Rule ==================================================================

Rule::Rule(NodePtr tree_expression_param,
	 NodePtr rhs_param, const Rules& rules) :
      BasicRule(tree_expression_param, rules), rhs(rhs_param) {
}

Rule::Rule(NodePtr tree_expression_param, NodePtr rhs_param,
	    const std::string& name_param, const Rules& rules) :
      BasicRule(tree_expression_param, rules), rhs(rhs_param),
      name(name_param) {
}

const std::string& Rule::get_name() const {
   return name;
}


const NodePtr Rule::get_rhs() const {
   return rhs;
}

std::ostream& operator<<(std::ostream& out, RulePtr rule) {
   const std::string& name(rule->get_name());
   if (name.length() > 0) {
      out << name << ": ";
   }
   out << rule->get_tree_expression() << " -> " << rule->get_rhs() << std::endl;
   return out;
}

} // namespace Astl
