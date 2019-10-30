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

#include <cassert>
#include <memory>
#include <set>
#include <sstream>
#include <astl/attribute.hpp>
#include <astl/execution.hpp>
#include <astl/function.hpp>
#include <astl/syntax-tree.hpp>

namespace Astl {

Function::Function(BindingsPtr bindings) :
      bindings(bindings), bind_parameters(true) {
}

Function::Function(BindingsPtr bindings,
	 std::initializer_list<std::string> parameters) :
      bindings(bindings), arity(parameters.size()), parameters(parameters),
      bind_parameters(true) {
}

Function::Function(BindingsPtr bindings, NodePtr pnode) :
      bindings(bindings), arity(pnode->size()), parameters(pnode->size()),
      bind_parameters(true) {
   std::set<std::string> pnames;
   for (std::size_t index = 0; index < arity.arity; ++index) {
      parameters[index] = pnode->get_operand(index)->get_token().get_text();
      auto res = pnames.insert(parameters[index]);
      if (!res.second) {
	 throw Exception(pnode->get_operand(index)->get_location(),
	    "parameter multiply declared: " + parameters[index]);
      }
   }
}

BuiltinFunction::BuiltinFunction(Builtin bf, BindingsPtr bindings) :
      Function(bindings), bf(bf) {
   bind_parameters = false; // parameters are passed directly
}

BuiltinFunction::BuiltinFunction(Builtin bf, BindingsPtr bindings,
	 std::initializer_list<std::string> parameters) :
      Function(bindings, parameters), bf(bf) {
}

RegularFunction::RegularFunction(NodePtr block,
      BindingsPtr bindings) :
      Function(bindings), block(block) {
}

RegularFunction::RegularFunction(NodePtr block, BindingsPtr bindings,
	 NodePtr parameters) :
      Function(bindings, parameters), block(block) {
}

BindingsPtr Function::process_parameters(AttributePtr args) const {
   BindingsPtr local_scope = std::make_shared<Bindings>(bindings);
   if (bind_parameters) {
      if (arity.fixed) {
	 if (args->size() != arity.arity) {
	    std::ostringstream os;
	    os << "wrong number of arguments, expected " <<
	       arity.arity << " but got " << args->size();
	    throw Exception(os.str());
	 }
	 for (std::size_t index = 0; index < arity.arity; ++index) {
	    bool ok = local_scope->define(parameters[index],
	       args->get_value(index));
	    assert(ok);
	 }
      } else {
	 bool ok = local_scope->define("args", args); assert(ok);
      }
   }
   return local_scope;
}

AttributePtr RegularFunction::eval(AttributePtr args) const {
   auto local_scope = process_parameters(args);
   return execute(block, local_scope);
}

AttributePtr BuiltinFunction::eval(AttributePtr args) const {
   auto local_scope = process_parameters(args);
   return (*bf)(local_scope, args);
}

} // namespace Astl
