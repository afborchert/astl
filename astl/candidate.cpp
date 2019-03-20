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
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <astl/candidate.hpp>
#include <astl/cloner.hpp>
#include <astl/execution.hpp>
#include <astl/expression.hpp>
#include <astl/operators.hpp>
#include <astl/tree-expressions.hpp>

namespace Astl {

Candidate::Candidate(NodePtr root, NodePtr& node,
      RulePtr rule, BindingsPtr bindings) :
      root(root), node(&node), bindings(bindings), rule(rule) {
}

NodePtr Candidate::transform() const {
   NodePtr rhs = rule->get_rhs();
   NodePtr pre_block; NodePtr post_block;
   if (!rhs->is_leaf() && rhs->get_op() == Op::transformation_instructions) {
      for (std::size_t i = 1; i < rhs->size(); ++i) {
	 NodePtr block = rhs->get_operand(i);
	 if (block->get_op() == Op::pre_transformation_block) {
	    pre_block = block->get_operand(0);
	 } else {
	    assert(block->get_op() == Op::post_transformation_block);
	    post_block = block->get_operand(0);
	 }
      }
      rhs = rhs->get_operand(0);
   }
   // execute the blocks and the transformation
   if (pre_block) execute(pre_block, bindings);
   NodePtr cloned_root;
   NodePtr* cloned_ptr = 0;
   clone_tree_and_ptr(root, *node, cloned_root, cloned_ptr);
   assert(cloned_ptr);
   *cloned_ptr = gen_tree(rhs);
   if (post_block) execute(post_block, bindings);
   // set "location" and "rulename" attribute
   AttributePtr rootAt = cloned_root->get_attribute();
   rootAt->update("location",
      std::make_shared<Attribute>(get_location()));
   rootAt->update("rulename",
      std::make_shared<Attribute>(rule->get_name()));
   return cloned_root;
}

void Candidate::transform_inplace() const {
   NodePtr rhs = rule->get_rhs();
   NodePtr pre_block; NodePtr post_block;
   if (!rhs->is_leaf() && rhs->get_op() == Op::transformation_instructions) {
      for (std::size_t i = 1; i < rhs->size(); ++i) {
	 NodePtr block = rhs->get_operand(i);
	 if (block->get_op() == Op::pre_transformation_block) {
	    pre_block = block->get_operand(0);
	 } else {
	    assert(block->get_op() == Op::post_transformation_block);
	    post_block = block->get_operand(0);
	 }
      }
      rhs = rhs->get_operand(0);
   }
   if (pre_block) execute(pre_block, bindings);
   *node = gen_tree(rhs);
   if (post_block) execute(post_block, bindings);
}

NodePtr Candidate::gen_tree(NodePtr troot) const {
   NodePtr newroot = Astl::gen_tree(troot, bindings);
   return newroot;
}

RulePtr Candidate::get_rule() const {
   return rule;
}

NodePtr Candidate::get_subtree() const {
   return *node;
}

const Location& Candidate::get_location() const {
   return get_subtree()->get_location();
}

BindingsPtr Candidate::get_bindings() const {
   return bindings;
}

std::ostream& operator<<(std::ostream& out, CandidatePtr candidate) {
   return out << "rule " << candidate->get_rule() << " matches " <<
      candidate->get_subtree() << " with " << candidate->get_bindings() <<
      " at " << candidate->get_location();
}

} // namespace Astl
