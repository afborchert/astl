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

#ifndef ASTL_EXPRESSION_H
#define ASTL_EXPRESSION_H

#include <astl/syntax-tree.hpp>
#include <astl/attribute.hpp>
#include <astl/tree-expressions.hpp>
#include <astl/exception.hpp>
#include <astl/bindings.hpp>
#include <astl/designator.hpp>

namespace Astl {

   class Expression {
      public:
	 Expression(NodePtr expr, BindingsPtr bindings_param)
	    throw(Exception);

	 // accessors
	 AttributePtr get_result() const;
	 bool is_lvalue() const;
	 DesignatorPtr get_designator() const;
	 NodePtr convert_to_node() const throw(Exception);
	 bool convert_to_bool() const throw(Exception);
	 IntegerPtr convert_to_integer() const throw(Exception);
	 AttributePtr convert_to_list() const throw(Exception);
	 AttributePtr convert_to_dict() const throw(Exception);

      private:
	 NodePtr root; // root of the expression tree
	 BindingsPtr bindings;
	 AttributePtr result;
	 DesignatorPtr desat;
	 DesignatorPtr eval_designator(NodePtr expr) throw(Exception);
	 AttributePtr eval_primary(NodePtr expr) throw(Exception);
	 AttributePtr recursive_evaluation(NodePtr expr) throw(Exception);
   };

} // namespace Astl

#endif
