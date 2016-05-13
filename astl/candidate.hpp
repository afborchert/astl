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

#ifndef ASTL_CANDIDATE_H
#define ASTL_CANDIDATE_H

#include <astl/rule.hpp>
#include <astl/tree-expressions.hpp>
#include <astl/exception.hpp>
#include <astl/treeloc.hpp>
#include <astl/bindings.hpp>
#include <iostream>
#include <memory>

namespace Astl {

   class Candidate {
      public:
	 Candidate(NodePtr root, NodePtr& node, RulePtr rule,
	    BindingsPtr bindings);

	 // accessors
	 RulePtr get_rule() const;
	 NodePtr get_subtree() const;
	 const Location& get_location() const;
	 BindingsPtr get_bindings() const;

	 // transformations, in-place and cloning
	 void transform_inplace() const throw(Exception);
	    // executes the transformation in-place
	 NodePtr transform() const throw(Exception);
	    // clones the tree, executes the transformation on the
	    // clone, and returns it

      private:
	 const NodePtr root;
	 NodePtr* node; // points to the matched subtree
	 BindingsPtr bindings;
	 const RulePtr rule; // matching rule
	 NodePtr gen_tree(NodePtr root) const throw(Exception);
   };

   typedef std::shared_ptr<Candidate> CandidatePtr;

   std::ostream& operator<<(std::ostream& out, CandidatePtr candidate);

} // namespace Astl

#endif
