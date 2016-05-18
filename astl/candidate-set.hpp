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

#ifndef ASTL_CANDIDATE_SET_H
#define ASTL_CANDIDATE_SET_H

#include <memory>
#include <vector>
#include <astl/bindings.hpp>
#include <astl/candidate.hpp>
#include <astl/context.hpp>
#include <astl/exception.hpp>
#include <astl/prg.hpp>
#include <astl/rule-table.hpp>
#include <astl/syntax-tree.hpp>

namespace Astl {

   class Consumer {
      public:
	 virtual bool consume(NodePtr root, CandidatePtr candidate) = 0;
   };
   typedef std::shared_ptr<Consumer> ConsumerPtr;

   class CandidateSet {
      public:
	 CandidateSet(NodePtr root, const RuleTable& rules);
	 CandidateSet(NodePtr root, const RuleTable& rules,
	    ConsumerPtr consumer, PseudoRandomGeneratorPtr prg);
	 CandidateSet(NodePtr root, const RuleTable& rules,
	    BindingsPtr bindings);
	 CandidateSet(NodePtr root, const RuleTable& rules,
	    BindingsPtr bindings, ConsumerPtr consumer,
	    PseudoRandomGeneratorPtr prg);

	 // accessors
	 unsigned int size() const;
	 CandidatePtr operator[](unsigned int index) const;

	 // generators
	 // general PRE: consumer and prg must be well-defined */
	 void gen_mutation() throw(Exception);
	 void gen_mutations() throw(Exception);
	 void gen_mutations(unsigned int count) throw(Exception);

	 // mutators
	 void set_consumer(ConsumerPtr consumer_param);
	 void set_prg(PseudoRandomGeneratorPtr prg_param);
	 CandidateSet& operator+=(CandidatePtr candidate);
	 void suppress_transformation_conflicts();
	    // must not be invoked after using any of the accessors

      private:
	 mutable bool generated; // list of candidates generated?
	 mutable std::vector<CandidatePtr> candidates;
	 mutable NodePtr root;
	 bool suppress_conflicts;
	 const RuleTable& rules;
	 BindingsPtr bindings;
	 ConsumerPtr consumer;
	 PseudoRandomGeneratorPtr prg;
	 void generate() const; // generate candidates, if necessary
	 void traverse(NodePtr& node, Context& context) const;
	 bool add_matching_candidates(NodePtr& node,
	    RulePtr rule, Context& context) const;
   };

} // namespace Astl

#endif
