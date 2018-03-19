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

#ifndef ASTL_CONTEXT_H
#define ASTL_CONTEXT_H

#include <iostream>
#include <list>
#include <memory>
#include <astl/operator.hpp>
#include <astl/types.hpp>

namespace Astl {

   class Context {
      public:
	 // constructors
	 Context();
	 Context(const Context& other);

	 // accessors
	 bool matches(NodePtr tree_expr, BindingsPtr bindings,
	       NodePtr node);
	 bool and_matches(NodePtr tree_expr, BindingsPtr bindings,
	       NodePtr node);
	 bool suppressed() const;

	 // mutators
	 void descend(NodePtr node);
	 void ascend();
	 void suppress_ancestors();

      private:
	 friend std::ostream& operator<<(std::ostream& out,
	    const Context& context);
	 struct PathMember;
	 typedef std::shared_ptr<PathMember> PathMemberPtr;
	 struct PathMember {
	    NodePtr node;
	    bool is_suppressed;
	    PathMemberPtr parent;
	    PathMember(NodePtr node, PathMemberPtr parent) :
	       node(node), is_suppressed(false), parent(parent) {
	    }
	 };
	 PathMemberPtr path;

	 // for and_matches():
	 bool matching_it_defined;
	 PathMemberPtr matching_it;
   };

   std::ostream& operator<<(std::ostream& out, const Context& context);

} // namespace Astl

#endif
