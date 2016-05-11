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

#ifndef ASTL_TYPES_H
#define ASTL_TYPES_H

#include <memory>
#include <astl/exception.hpp>

// forward declaration of all types which are member of a reference cycle

namespace Astl {

   class Attribute;
   typedef std::shared_ptr<Attribute> AttributePtr;

   class Node;
   typedef std::shared_ptr<Node> NodePtr;

   class Function;
   typedef std::shared_ptr<Function> FunctionPtr;

   class Bindings;
   typedef std::shared_ptr<Bindings> BindingsPtr;

   class FlowGraphNode;
   typedef std::shared_ptr<FlowGraphNode> FlowGraphNodePtr;

   typedef AttributePtr (*Builtin)(BindingsPtr, AttributePtr);

   class Context;
   class Rules;

} // namespace Astl

#endif
