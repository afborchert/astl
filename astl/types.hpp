/*
   Copyright (C) 2009-2019 Andreas Franz Borchert
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

#ifndef ASTL_TYPES_HPP
#define ASTL_TYPES_HPP

#include <iostream>
#include <memory>
#include <astl/exception.hpp>

// forward declaration of all types which are member of a reference cycle

namespace Astl {

   class Attribute;
   using AttributePtr = std::shared_ptr<Attribute>;

   class Node;
   using NodePtr = std::shared_ptr<Node>;

   class Function;
   using FunctionPtr = std::shared_ptr<Function>;

   class Bindings;
   using BindingsPtr = std::shared_ptr<Bindings>;

   class FlowGraphNode;
   using FlowGraphNodePtr = std::shared_ptr<FlowGraphNode>;

   using Builtin = AttributePtr (*)(BindingsPtr, AttributePtr);

   template<typename T> class Stream;
   using OutputStream = Stream<std::ostream>;
   using OutputStreamPtr = std::shared_ptr<OutputStream>;

   using InputStream = Stream<std::istream>;
   using InputStreamPtr = std::shared_ptr<InputStream>;

   class Context;
   class Rules;

} // namespace Astl

#endif
