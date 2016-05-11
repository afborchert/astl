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

#ifndef ASTL_CLONER_H
#define ASTL_CLONER_H

#include <astl/syntax-tree.hpp>

namespace Astl {

   /**
    * Clone tree pointed to by root and set cloned_root to
    * the cloned tree.  If ptr is non-null and found anywhere
    * below root, cloned_ptr is set to the corresponding cloned
    * subtree. Note that attributes are not cloned.
    */
   void clone_tree_and_ptr(const NodePtr& root, const NodePtr& ptr,
      NodePtr& cloned_root, NodePtr*& cloned_ptr);

   /**
    * Clone tree pointed to by root and return the cloned tree. Note
    * that attributes are not cloned.
    */
   NodePtr clone(const NodePtr& root);

   /**
    * Clone tree pointed to by root and return the cloned tree.
    */
   NodePtr clone_including_attributes(const NodePtr& root);

} // namespace Astl

#endif
