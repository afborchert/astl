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

#include <cassert>
#include <sstream>
#include <astl/context.hpp>
#include <astl/bindings.hpp>
#include <astl/tree-expressions.hpp>

namespace Astl {

Context::Context() : matching_it_defined(false) {
}

Context::Context(const Context& other) :
   path(other.path), matching_it_defined(false) {
}

std::string get_here_name(BindingsPtr bindings, BindingsPtr local_bindings) {
   if (!bindings->defined("here") && local_bindings->defined("here")) {
      return "here";
   }
   for (int counter = 1; ; ++counter) {
      std::ostringstream os;
      os << "here" << counter;
      if (!bindings->defined(os.str())) {
	 return os.str();
      }
   }
}

bool Context::matches(NodePtr tree_expr, BindingsPtr bindings,
      NodePtr node) throw(Exception) {
   matching_it_defined = false;
   return and_matches(tree_expr, bindings, node);
}

bool Context::and_matches(NodePtr tree_expr, BindingsPtr bindings,
      NodePtr node) throw(Exception) {
   Context empty_context;
   BindingsPtr local_bindings = BindingsPtr(new Bindings(bindings));
   PathMemberPtr it;
   NodePtr last;
   if (matching_it_defined) {
      last = matching_it->node;
      it = matching_it->parent;
      matching_it_defined = false;
   } else {
      it = path;
      last = node;
   }
   for (; it; it = it->parent) {
      if (Astl::matches(it->node, tree_expr, local_bindings, empty_context)) {
	 std::string here = get_here_name(bindings, local_bindings);
	 if (local_bindings->defined(here)) {
	    AttributePtr at = local_bindings->get(here);
	    assert(at->get_type() == Attribute::tree);
	    if (at->get_node() != last) {
	       return false;
	    }
	 }
	 if (!bindings->merge(local_bindings)) {
	    throw Exception(tree_expr->get_location(),
	       "one of the variable bindings is in conflict");
	 }
	 matching_it = it;
	 matching_it_defined = true;
	 return true;
      }
      last = it->node;
   }
   return false;
}

bool Context::suppressed() const {
   assert(path);
   return path->is_suppressed;
}

void Context::descend(NodePtr node) {
   PathMemberPtr npath(new PathMember(node, path));
   path = npath;
   matching_it_defined = false;
}

void Context::ascend() {
   assert(path);
   path = path->parent;
   matching_it_defined = false;
}

void Context::suppress_ancestors() {
   for (PathMemberPtr it = path; it && !it->is_suppressed; it = it->parent) {
      it->is_suppressed = true;
   }
}

std::ostream& operator<<(std::ostream& out, const Context& context) {
   out << "{";
   bool first = true;
   for (Context::PathMemberPtr it = context.path; it; it = it->parent) {
      if (first) {
	 first = false;
      } else {
	 out << ", ";
      }
      out << it->node->get_op();
   }
   out << "}";
   return out;
}

} // namespace Astl
