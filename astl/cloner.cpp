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

#include <memory>
#include <astl/attribute.hpp>
#include <astl/cloner.hpp>
#include <astl/syntax-tree.hpp>
#include <astl/token.hpp>

namespace Astl {

void gen_clone_tree_and_ptr(const NodePtr& root, const NodePtr& ptr,
      NodePtr& cloned_root, NodePtr*& cloned_ptr,
      bool with_attributes) {
   if (root->is_leaf()) {
      cloned_root = std::make_shared<Node>(*root);
   } else {
      const Location& loc = root->get_location();
      Operator op = root->get_op();
      cloned_root = std::make_shared<Node>(loc, op);
      for (unsigned int i = 0; i < root->size(); ++i) {
	 /* insert dummy node */
	 *cloned_root += std::make_shared<Node>();
	 /* fetch reference of dummy node ... */
	 NodePtr& subtree = cloned_root->get_operand(i);
	 /* ... and let it be replaced by the clone */
	 gen_clone_tree_and_ptr(root->get_operand(i), ptr, subtree, cloned_ptr,
	    with_attributes);
      }
   }
   if (with_attributes) {
      AttributePtr attributes = root->get_attribute()->clone();
      cloned_root->set_attribute(attributes);
   }
   if (ptr == root) {
      cloned_ptr = &cloned_root;
   }
}

void clone_tree_and_ptr(const NodePtr& root, const NodePtr& ptr,
      NodePtr& cloned_root, NodePtr*& cloned_ptr) {
   gen_clone_tree_and_ptr(root, ptr, cloned_root, cloned_ptr, false);
}

NodePtr clone(const NodePtr& root) {
   NodePtr cloned_root;
   NodePtr ptr; // just passed as dummy
   NodePtr* cloned_ptr = 0; // just passed as dummy
   clone_tree_and_ptr(root, ptr, cloned_root, cloned_ptr);
   return cloned_root;
}

NodePtr clone_including_attributes(const NodePtr& root) {
   NodePtr cloned_root;
   NodePtr ptr; // just passed as dummy
   NodePtr* cloned_ptr = 0; // just passed as dummy
   gen_clone_tree_and_ptr(root, ptr, cloned_root, cloned_ptr, true);
   return cloned_root;
}

} // namespace Astl
