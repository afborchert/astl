/*
   Copyright (C) 2009, 2011 Andreas Franz Borchert
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
#include <cstdlib>
#include <memory>
#include <astl/arithmetic-ops.hpp>
#include <astl/expression.hpp>
#include <astl/flow-graph.hpp>
#include <astl/integer.hpp>
#include <astl/list-ops.hpp>
#include <astl/operators.hpp>
#include <astl/regex.hpp>
#include <astl/set-ops.hpp>
#include <astl/string-ops.hpp>

namespace Astl {

// list of operators that enforce its operands to be interpreted
// as integers (if it is not a set operator with two dictionaries)
static bool is_arithmetic_op(int opcode) {
   switch (opcode) {
      case ASTL_OPERATOR_PLUS_TK:
      case ASTL_OPERATOR_MINUS_TK:
      case ASTL_OPERATOR_DIV_TK:
      case ASTL_OPERATOR_MOD_TK:
      case ASTL_OPERATOR_STAR_TK:
      case ASTL_OPERATOR_POWER_TK:
	 return true;
      default:
	 return false;
   }
}

static bool is_set_op(int opcode) {
   switch (opcode) {
      case ASTL_OPERATOR_PLUS_TK:
      case ASTL_OPERATOR_MINUS_TK:
      case ASTL_OPERATOR_STAR_TK:
      case ASTL_OPERATOR_POWER_TK:
	 return true;
      default:
	 return false;
   }
}

// list of operators that enforce its operands to be interpreted
// as strings or lists
static bool is_string_op(int opcode) {
   switch (opcode) {
      case ASTL_OPERATOR_AMPERSAND_TK:
      case ASTL_OPERATOR_X_TK:
	 return true;
      default:
	 return false;
   }
}

Expression::Expression(NodePtr expr, BindingsPtr bindings_param)
      : root(expr), bindings(bindings_param) {
   assert(!expr->is_leaf());
   assert(expr->size() == 1);
   // descend to designator if it is one
   if (expr->get_op() == Op::expression) {
      expr = expr->get_operand(0);
   }
   if (expr->get_op() == Op::primary &&
	 expr->get_operand(0)->get_op() == Op::designator) {
      expr = expr->get_operand(0);
   }
   if (expr->get_op() == Op::designator) {
      desat = eval_designator(expr->get_operand(0));
      if (desat->exists()) {
	 result = desat->get_value(expr->get_location());
      }
   } else {
      result = recursive_evaluation(expr);
   }
}

AttributePtr Expression::get_result() const {
   return result;
}

bool Expression::is_lvalue() const {
   return desat && desat->is_lvalue();
}

DesignatorPtr Expression::get_designator() const {
   assert(is_lvalue());
   return desat;
}

NodePtr Expression::convert_to_node() const {
   if (result) {
      if (result->get_type() == Attribute::tree) {
	 return result->get_node();
      } else {
	 return std::make_shared<Node>(root->get_location(),
	    Token(result->convert_to_string()));
      }
   } else {
      return std::make_shared<Node>(root->get_location(), Token(""));
   }
}

bool Expression::convert_to_bool() const {
   if (result) {
      return result->convert_to_bool();
   } else {
      return false;
   }
}

IntegerPtr Expression::convert_to_integer() const {
   if (result) {
      return result->convert_to_integer(root->get_location());
   } else {
      return std::make_shared<Integer>(0);
   }
}

AttributePtr Expression::convert_to_list() const {
   if (!result) {
      // return an empty list for a null value
      return std::make_shared<Attribute>(Attribute::list);
   }
   return result->convert_to_list();
}

AttributePtr Expression::convert_to_dict() const {
   if (!result) {
      // return an empty dictionary for a null value
      return std::make_shared<Attribute>(Attribute::dictionary);
   }
   return result->convert_to_dict();
}

DesignatorPtr Expression::eval_designator(NodePtr expr) {
   if (expr->is_leaf()) {
      /* IDENT token */
      std::string varname = expr->get_token().get_text();
      if (bindings->defined(varname)) {
	 return std::make_shared<Designator>(bindings, varname);
      } else {
	 std::ostringstream os;
	 os << "unknown identifier: " << varname;
	 throw Exception(expr->get_location(), os.str());
      }
   } else {
      assert(expr->size() == 2);
      DesignatorPtr desAt = eval_designator(expr->get_operand(0));
      if (expr->get_op() == Op::DOT) {
	 std::string key = expr->get_operand(1)->get_token().get_text();
	 desAt->add_key(key, expr->get_location());
      } else if (expr->get_op() == Op::LBRACE) {
	 AttributePtr indexAt = recursive_evaluation(expr->get_operand(1));
	 if (!indexAt) {
	    throw Exception(expr->get_operand(1)->get_location(),
	       "index is null");
	 }
	 desAt->add_key(indexAt->convert_to_string(), expr->get_location());
      } else if (expr->get_op() == Op::LBRACKET) {
	 AttributePtr indexAt = recursive_evaluation(expr->get_operand(1));
	 desAt->add_index(indexAt, expr->get_location());
      }
      return desAt;
   }
}

AttributePtr Expression::eval_primary(NodePtr expr) {
   assert(!expr->is_leaf());
   switch (expr->get_op().get_opcode()) {
      case ASTL_OPERATOR_DESIGNATOR:
	 {
	    auto desAt = eval_designator(expr->get_operand(0));
	    return desAt->get_value(expr->get_location());
	 }
      case ASTL_OPERATOR_FUNCTION_CALL:
	 {
	    auto func = recursive_evaluation(expr->get_operand(0));
	    if (func->get_type() != Attribute::function) {
	       throw Exception(expr->get_operand(0)->get_location(),
		  "function expected");
	    }
	    auto args(std::make_shared<Attribute>(Attribute::list));
	    if (expr->size() == 2) {
	       auto expr_list = expr->get_operand(1);
	       for (std::size_t i = 0; i < expr_list->size(); ++i) {
		  auto arg = recursive_evaluation(expr_list->get_operand(i));
		  args->push_back(arg);
	       }
	    }
	    FunctionPtr f = func->get_func();
	    try {
	       return f->eval(args);
	    } catch (Exception& e) {
	       throw Exception(expr->get_location(), e.what());
	    }
	 }
      case ASTL_OPERATOR_FUNCTION_CONSTRUCTOR:
	 {
	    FunctionPtr f;
	    auto local_bindings = std::make_shared<Bindings>(bindings);
	    if (expr->size() == 1) {
	       auto block = expr->get_operand(0);
	       f = std::make_shared<RegularFunction>(block, local_bindings);
	    } else {
	       auto params = expr->get_operand(0);
	       auto block = expr->get_operand(1);
	       f = std::make_shared<RegularFunction>(block,
		  local_bindings, params);
	    }
	    return std::make_shared<Attribute>(f);
	 }
      case ASTL_OPERATOR_LIST_AGGREGATE:
	 {
	    auto list = std::make_shared<Attribute>(Attribute::list);
	    for (std::size_t i = 0; i < expr->size(); ++i) {
	       auto elem = recursive_evaluation(expr->get_operand(i));
	       list->push_back(elem);
	    }
	    return list;
	 }
      case ASTL_OPERATOR_DICTIONARY_AGGREGATE:
	 {
	    auto dict = std::make_shared<Attribute>(Attribute::dictionary);
	    for (std::size_t i = 0; i < expr->size(); ++i) {
	       auto pair = expr->get_operand(i);
	       assert(pair->get_op() == Op::key_value_pair);
	       std::string key = pair->get_operand(0)->get_token().get_text();
	       auto value = recursive_evaluation(pair->get_operand(1));
	       dict->update(key, value);
	    }
	    return dict;
	 }
      case ASTL_OPERATOR_TREE_EXPRESSION:
	 {
	    auto tree = gen_tree(expr, bindings);
	    return std::make_shared<Attribute>(tree);
	 }
      case ASTL_OPERATOR_CARDINAL_LITERAL_TK:
	 {
	    std::string s = expr->get_operand(0)->get_token().get_text();
	    auto cardval = std::make_shared<Integer>(s, expr->get_location());
	    return std::make_shared<Attribute>(cardval);
	 }
      case ASTL_OPERATOR_STRING_LITERAL_TK:
	 {
	    std::string sval = expr->get_operand(0)->get_token().get_text();
	    return std::make_shared<Attribute>(sval);
	 }
      case ASTL_OPERATOR_EXISTS_TK:
	 {
	    auto desAt = eval_designator(expr->get_operand(0));
	    return std::make_shared<Attribute>(desAt->exists());
	 }
      case ASTL_OPERATOR_PREFIX_INCREMENT:
	 {
	    auto desAt = eval_designator(expr->get_operand(0));
	    auto valueAt = 
		  desAt->get_value(expr->get_operand(0)->get_location());
	    auto resultAt = arithmetic_binary_op(Op::PLUS,
		  valueAt, std::make_shared<Attribute>(1),
		  expr->get_location());
	    desAt->assign(resultAt, expr->get_location());
	    return resultAt;
	 }
      case ASTL_OPERATOR_PREFIX_DECREMENT:
	 {
	    auto desAt = eval_designator(expr->get_operand(0));
	    auto valueAt = 
		  desAt->get_value(expr->get_operand(0)->get_location());
	    auto resultAt = arithmetic_binary_op(Op::MINUS,
		  valueAt, std::make_shared<Attribute>(1),
		  expr->get_location());
	    desAt->assign(resultAt, expr->get_location());
	    return resultAt;
	 }
      case ASTL_OPERATOR_POSTFIX_INCREMENT:
	 {
	    auto desAt = eval_designator(expr->get_operand(0));
	    auto valueAt = 
		  desAt->get_value(expr->get_operand(0)->get_location());
	    auto resultAt = arithmetic_binary_op(Op::PLUS,
		  valueAt, std::make_shared<Attribute>(1),
		  expr->get_location());
	    desAt->assign(resultAt, expr->get_location());
	    return valueAt;
	 }
      case ASTL_OPERATOR_POSTFIX_DECREMENT:
	 {
	    auto desAt = eval_designator(expr->get_operand(0));
	    auto valueAt = 
		  desAt->get_value(expr->get_operand(0)->get_location());
	    AttributePtr resultAt = arithmetic_binary_op(Op::MINUS,
		  valueAt, std::make_shared<Attribute>(1),
		  expr->get_location());
	    desAt->assign(resultAt, expr->get_location());
	    return valueAt;
	 }
      case ASTL_OPERATOR_NULL_T_TK:
	 {
	    return AttributePtr(nullptr);
	 }
      default:
	 /* use of parentheses */
	 return recursive_evaluation(expr);
   }
}

AttributePtr Expression::recursive_evaluation(NodePtr expr) {
   assert(!expr->is_leaf());
   if (expr->get_op() == Op::expression) {
      expr = expr->get_operand(0);
   }
   if (expr->get_op() == Op::primary) {
      return eval_primary(expr->get_operand(0));
   } else if (expr->size() == 3) {
      /* conditional */
      AttributePtr condAt(recursive_evaluation(expr->get_operand(0)));
      bool cond_result;
      if (condAt) {
	 cond_result = condAt->convert_to_bool();
      } else {
	 cond_result = false;
      }
      if (cond_result) {
	 return recursive_evaluation(expr->get_operand(1));
      } else {
	 return recursive_evaluation(expr->get_operand(2));
      }
   } else if (expr->get_op() == Op::assignment) {
      expr = expr->get_operand(0);
      Expression des(expr->get_operand(0), bindings);
      assert(des.is_lvalue());
      DesignatorPtr desAt = des.get_designator();
      AttributePtr rightAt(recursive_evaluation(expr->get_operand(1)));
      if (expr->get_op() == Op::EQ) {
	 desAt->assign(rightAt, expr->get_location());
      } else {
	 AttributePtr leftAt = desAt->get_value(expr->get_location());
	 if (expr->get_op() == Op::AMP_EQ &&
	       leftAt && leftAt->get_type() == Attribute::list) {
	    desAt->assign(list_binary_op(Op::AMPERSAND, leftAt, rightAt,
	       expr->get_location()), expr->get_location());
	 } else if (leftAt && leftAt->get_type() == Attribute::dictionary) {
	    if (expr->get_op() == Op::AMP_EQ) {
	       throw Exception(expr->get_location(),
		  "operator '&=' is not supported for dictionaries");
	    }
	    if (rightAt) {
	       if (rightAt->get_type() != Attribute::dictionary) {
		  rightAt = rightAt->convert_to_dict();
	       }
	       switch (expr->get_op().get_opcode()) {
		  case ASTL_OPERATOR_PLUS_EQ_TK:
		     desAt->assign(set_binary_op(Op::PLUS,
			leftAt, rightAt, expr->get_location()),
			expr->get_location());
		     break;
		  case ASTL_OPERATOR_MINUS_EQ_TK:
		     desAt->assign(set_binary_op(Op::MINUS,
			leftAt, rightAt, expr->get_location()),
			expr->get_location());
		     break;
		  default:
		     assert(false); std::abort();
	       }
	    }
	 } else {
	    if (!rightAt) {
	       rightAt = std::make_shared<Attribute>("");
	    }
	    switch (expr->get_op().get_opcode()) {
	       case ASTL_OPERATOR_AMP_EQ_TK:
		  desAt->assign(std::make_shared<Attribute>(
		     leftAt->convert_to_string() +
			rightAt->convert_to_string()),
		     expr->get_location());
		  break;
	       case ASTL_OPERATOR_PLUS_EQ_TK:
		  desAt->assign(arithmetic_binary_op(Op::PLUS,
		     leftAt, rightAt, expr->get_location()),
		     expr->get_location());
		  break;
	       case ASTL_OPERATOR_MINUS_EQ_TK:
		  desAt->assign(arithmetic_binary_op(Op::MINUS,
		     leftAt, rightAt, expr->get_location()),
		     expr->get_location());
		  break;
	       default:
		  assert(false); std::abort();
	    }
	 }
      }
      return desAt->get_value(expr->get_location());
   } else if (expr->get_op() == Op::OR || expr->get_op() == Op::AND) {
      AttributePtr leftAt(recursive_evaluation(expr->get_operand(0)));
      bool leftVal;
      if (leftAt) {
	 leftVal = leftAt->convert_to_bool();
      } else {
	 leftVal = false;
      }
      // short circuit evaluation
      if (expr->get_op() == Op::OR && leftVal) {
	 return std::make_shared<Attribute>(true);
      } else if (expr->get_op() == Op::AND && !leftVal) {
	 return std::make_shared<Attribute>(false);
      }
      AttributePtr rightAt(recursive_evaluation(expr->get_operand(1)));
      bool rightVal;
      if (rightAt) {
	 rightVal = rightAt->convert_to_bool();
      } else {
	 rightVal = false;
      }
      if (rightVal) {
	 return std::make_shared<Attribute>(true);
      } else {
	 return std::make_shared<Attribute>(false);
      }
   } else if (expr->get_op() == Op::MATCHES) {
      AttributePtr stringAt = recursive_evaluation(expr->get_operand(0));
      if (!stringAt) {
	 stringAt = std::make_shared<Attribute>("");
      }
      NodePtr regexpr = expr->get_operand(1);
      std::string res;
      if (regexpr->is_leaf()) {
	 res = regexpr->get_token().get_text();
      } else {
	 AttributePtr regexAt = recursive_evaluation(regexpr);
	 res = regexAt->convert_to_string();
      }
      Regex re(regexpr->get_location(), res);
      std::string s = stringAt->convert_to_string();
      AttributePtr match_result = re.match(s);
      if (match_result != nullptr) {
	 return match_result;
      } else {
	 return std::make_shared<Attribute>(false);
      }
   } else if (expr->size() == 2) {
      /* binary operators */
      AttributePtr leftAt = recursive_evaluation(expr->get_operand(0));
      AttributePtr rightAt = recursive_evaluation(expr->get_operand(1));
      if (((expr->get_op() == Op::AMPERSAND &&
	    (leftAt || rightAt) &&
	    (leftAt && leftAt->get_type() == Attribute::list)) ||
	       (rightAt && rightAt->get_type() == Attribute::list))) {
	 return list_binary_op(expr->get_op(), leftAt, rightAt,
	    expr->get_location());
      }
      // handle comparisons with null before we convert this
      if ((!leftAt || !rightAt) &&
	    (expr->get_op() == Op::EQEQ || expr->get_op() == Op::NE)) {
	 return std::make_shared<Attribute>(
	    (leftAt == rightAt) == (expr->get_op() == Op::EQEQ)
	 );
      }
      // convert null values to the empty string
      if (!leftAt) {
	 leftAt = std::make_shared<Attribute>("");
      }
      if (!rightAt) {
	 rightAt = std::make_shared<Attribute>("");
      }
      if (!leftAt->is_scalar() && !rightAt->is_scalar() &&
	    (expr->get_op() == Op::EQEQ || expr->get_op() == Op::NE)) {
	 // we compare attribute pointers in case of non-scalar attributes
	 return std::make_shared<Attribute>(leftAt->equal_to(rightAt) ==
		  (expr->get_op() == Op::EQEQ)
	 );
      } else if (is_string_op(expr->get_op().get_opcode())) {
	 return string_binary_op(expr->get_op(), leftAt, rightAt,
	    expr->get_location());
      } else if (is_set_op(expr->get_op().get_opcode()) &&
	    leftAt->get_type() == Attribute::dictionary &&
	    rightAt->get_type() == Attribute::dictionary) {
	 return set_binary_op(expr->get_op(), leftAt, rightAt,
	    expr->get_location());
      } else if (is_arithmetic_op(expr->get_op().get_opcode()) ||
	    leftAt->is_integer() || rightAt->is_integer()) {
	 return arithmetic_binary_op(expr->get_op(), leftAt, rightAt,
	    expr->get_location());
      } else {
	 return string_binary_op(expr->get_op(), leftAt, rightAt,
	    expr->get_location());
      }
   } else if (expr->get_op() == Op::NOT) {
      AttributePtr opat(recursive_evaluation(expr->get_operand(0)));
      return std::make_shared<Attribute>(!opat || !opat->convert_to_bool());
   } else {
      /* unary operators */
      assert(expr->size() == 1);
      AttributePtr at = recursive_evaluation(expr->get_operand(0));
      if (!at) {
	 at = AttributePtr(nullptr);
      }
      IntegerPtr operand = std::make_shared<Integer>(
	 *(at->convert_to_integer(expr->get_location())));
      operand->neg();
      return std::make_shared<Attribute>(operand);
   }
}

} // namespace Astl
