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

#include <cassert>
#include <memory>
#include <astl/function.hpp>
#include <astl/execution.hpp>
#include <astl/attribute.hpp>

namespace Astl {

Function::Function(BindingsPtr bindings_param) : bindings(bindings_param) {
}

BuiltinFunction::BuiltinFunction(Builtin bf_param, BindingsPtr bindings_param) :
      Function(bindings_param), bf(bf_param) {
}

RegularFunction::RegularFunction(NodePtr block_param,
      BindingsPtr bindings_param) :
      Function(bindings_param), block(block_param) {
}

AttributePtr RegularFunction::eval(AttributePtr args) const throw(Exception) {
   BindingsPtr local_scope = std::make_shared<Bindings>(bindings);
   bool ok = local_scope->define("args", args); assert(ok);
   return execute(block, local_scope);
}

AttributePtr BuiltinFunction::eval(AttributePtr args) const throw(Exception) {
   return (*bf)(bindings, args);
}

} // ast
