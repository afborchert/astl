/*
   Copyright (C) 2009, 2016 Andreas Franz Borchert
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

#ifndef ASTL_FUNCTION_H
#define ASTL_FUNCTION_H

#include <astl/arity.hpp>
#include <astl/attribute.hpp>
#include <astl/bindings.hpp>
#include <astl/builtin-functions.hpp>
#include <astl/exception.hpp>
#include <astl/types.hpp>

namespace Astl {

   class Function {
      public:
	 Function(BindingsPtr bindings);
	 Function(BindingsPtr bindings,
	    std::initializer_list<std::string> parameters);
	 Function(BindingsPtr bindings, NodePtr parameters) throw(Exception);
	 virtual AttributePtr eval(AttributePtr args)
	    const throw(Exception) = 0;
	 const Arity& get_arity() const {
	    return arity;
	 }
      protected:
	 BindingsPtr bindings;
	 Arity arity;
	 std::vector<std::string> parameters;
	 bool bind_parameters;

	 BindingsPtr process_parameters(AttributePtr args) const
	    throw(Exception);
   };

   class RegularFunction: public Function {
      public:
	 RegularFunction(NodePtr block, BindingsPtr bindings);
	 RegularFunction(NodePtr block, BindingsPtr bindings,
	    NodePtr parameters) throw(Exception);
	 virtual AttributePtr eval(AttributePtr args) const throw(Exception);
      private:
	 NodePtr block;
   };

   class BuiltinFunction: public Function {
      public:
	 BuiltinFunction(Builtin bf, BindingsPtr bindings);
	 BuiltinFunction(Builtin bf, BindingsPtr bindings,
	    std::initializer_list<std::string> parameters);
	 virtual AttributePtr eval(AttributePtr args) const throw(Exception);
      private:
	 Builtin bf;
   };

} // namespace Astl

#endif
