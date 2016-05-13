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

#ifndef ASTL_RULE
#define ASTL_RULE

#include <memory>
#include <astl/types.hpp>
#include <astl/operator.hpp>
#include <astl/opset.hpp>

namespace Astl {

   using Arity = std::pair<bool, unsigned int>;
      /* first: true if the arity is variable,
         second: the arity, if it is fixed, i.e. first is false */
   constexpr auto variable_arity = Arity(false, 0);
   constexpr auto no_parameters = Arity(true, 0);

   class BasicRule {
      public:
	 // attribution rules can be prefix or postfix rules,
	 // by default we have prefix rules
	 typedef enum {prefix, postfix} Type;

	 // constructors
	 BasicRule(const Rules& rules_param);
	 BasicRule(NodePtr tree_expr_param, const Rules& rules_param);

	 // accessors
	 OperatorSetPtr get_opset() const;
	 Arity get_arity() const;
	 const NodePtr get_tree_expression() const;
	 const Type get_type() const;

      protected:
	 const Rules& rules;
	 NodePtr tree_expr;
	 // cache opset, arity, and type from the tree expression
	 OperatorSetPtr opset;
	 Arity arity;
	 Type type;
   };

   class Rule: public BasicRule {
      public:
	 // constructors
	 Rule(NodePtr tree_expression_param, NodePtr rhs_param,
	    const Rules& rules_param);
	 Rule(NodePtr tree_expression_param, NodePtr rhs_param,
	    const std::string& name_param, const Rules& rules_param);

	 // accessors
	 const std::string& get_name() const;

	 const NodePtr get_rhs() const;

      private:
	 const NodePtr rhs;
	 const std::string name;
   };

   typedef std::shared_ptr<Rule> RulePtr;

   /**
    * This output operator prints a textual representation of a rule
    * using a nested Lisp-like notation with parentheses that extends
    * over multiple output lines. It is intended for debugging purposes.
    * Please note that as tokens are printed as-is it is not possible to
    * parse the output text.
    */
   std::ostream& operator<<(std::ostream& out, RulePtr rule);

} // namespace Astl

#endif
