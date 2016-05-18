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

#ifndef ASTL_FUNCTION_H
#define ASTL_FUNCTION_H

#include <astl/attribute.hpp>
#include <astl/bindings.hpp>
#include <astl/builtin-functions.hpp>
#include <astl/exception.hpp>
#include <astl/types.hpp>

namespace Astl {

   class Function {
      public:
	 Function(BindingsPtr bindings_param);
	 virtual AttributePtr eval(AttributePtr args)
	    const throw(Exception) = 0;
      protected:
	 BindingsPtr bindings;
   };

   class RegularFunction: public Function {
      public:
	 RegularFunction(NodePtr block_param, BindingsPtr bindings_param);
	 virtual AttributePtr eval(AttributePtr args) const throw(Exception);
      private:
	 NodePtr block;
   };

   class BuiltinFunction: public Function {
      public:
	 BuiltinFunction(Builtin bf_param, BindingsPtr bindings_param);
	 virtual AttributePtr eval(AttributePtr args) const throw(Exception);
      private:
	 Builtin bf;
   };

} // namespace Astl

#endif
