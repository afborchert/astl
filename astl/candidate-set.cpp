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
#include <memory>
#include <astl/tree-expressions.hpp>
#include <astl/default-bindings.hpp>
#include <astl/candidate-set.hpp>
#include <astl/operator.hpp>

namespace Astl {

CandidateSet::CandidateSet(NodePtr root, const RuleTable& rules) :
      generated(false), root(root), suppress_conflicts(false),
      rules(rules), bindings(create_default_bindings(root)),
      consumer(nullptr), prg(nullptr) {
}

CandidateSet::CandidateSet(NodePtr root, const RuleTable& rules,
	 ConsumerPtr consumer, PseudoRandomGeneratorPtr prg) :
      generated(false), root(root), suppress_conflicts(false),
      rules(rules), bindings(create_default_bindings(root)),
      consumer(consumer), prg(prg) {
}

CandidateSet::CandidateSet(NodePtr root, const RuleTable& rules,
	 BindingsPtr bindings) :
      generated(false), root(root), suppress_conflicts(false),
      rules(rules), bindings(bindings), consumer(nullptr), prg(nullptr) {
}

CandidateSet::CandidateSet(NodePtr root, const RuleTable& rules,
	 BindingsPtr bindings,
	 ConsumerPtr consumer, PseudoRandomGeneratorPtr prg) :
      generated(false), root(root), suppress_conflicts(false),
      rules(rules), bindings(bindings), consumer(consumer), prg(prg) {
}

void CandidateSet::generate() const {
   if (!generated) {
      Context context;
      traverse(root, context);
      generated = true;
   }
}

void CandidateSet::traverse(NodePtr& node, Context& context) const {
   if (node->is_leaf()) return;
   Arity arity(node->size());
   Operator op = node->get_op();
   RuleTable::iterator end;
   node->set_context(context);
   // prefix visitation
   bool found = false; // matching rule found
   if (!suppress_conflicts || node != root) {
      for (RuleTable::iterator it = rules.find_prefix(op, Arity(), end);
	    it != end; ++it) {
	 found = add_matching_candidates(node, it->second, context);
	 if (found && suppress_conflicts) break;
      }
      for (RuleTable::iterator it = rules.find_prefix(op, arity, end);
	    it != end; ++it) {
	 found = add_matching_candidates(node, it->second, context);
	 if (found && suppress_conflicts) break;
      }
   }
   // descending
   bool suppressed = suppress_conflicts && found;
   if (!suppressed) {
      context.descend(node);
      for (unsigned int i = 0; i < node->size(); ++i) {
	 traverse(node->get_operand(i), context);
      }
      // suppress the postfix visitation in case of transformations if
      //    - we consider the root or
      //    - a transformation would possibly conflict with another
      //      already executed transformation
      suppressed = suppress_conflicts &&
	 (node == root || context.suppressed());
      context.ascend();
   }
   if (!suppressed) {
      // postfix visitation
      // note that in case of conflict suppression just one candidate
      // is considered, all others are suppressed
      bool found = false; // matching rule found
      for (RuleTable::iterator it = rules.find_postfix(op, arity, end);
	    it != end; ++it) {
	 found = add_matching_candidates(node, it->second, context);
	 if (found && suppress_conflicts) break;
      }
      if (!found || !suppress_conflicts) {
	 for (RuleTable::iterator it = rules.find_postfix(op, Arity(), end);
	       it != end; ++it) {
	    found = add_matching_candidates(node, it->second, context);
	    if (found && suppress_conflicts) break;
	 }
      }
   }
}

bool CandidateSet::add_matching_candidates(NodePtr& node,
      RulePtr rule, Context& context) const {
   auto local_bindings = std::make_shared<Bindings>(bindings);
   auto left_expr = rule->get_tree_expression();
   auto right_expr = rule->get_rhs();
   if (matches(node, left_expr, local_bindings, context)) {
      candidates.push_back(std::make_shared<Candidate>(root,
	 node, rule, local_bindings));
      context.suppress_ancestors();
      return true;
   }
   return false;
}

unsigned int CandidateSet::size() const {
   generate();
   return candidates.size();
}

CandidatePtr CandidateSet::operator[](unsigned int index) const {
   generate();
   assert(index < size());
   return candidates[index];
}

void CandidateSet::gen_mutation() throw(Exception) {
   generate();
   assert(size() > 0); assert(consumer); assert(prg);
   CandidatePtr candidate = candidates[prg->pick(size())];
   consumer->consume(candidate->transform(), candidate);
}

void CandidateSet::gen_mutations() throw(Exception) {
   generate();
   assert(consumer); assert(prg);
   for (unsigned int i = 0; i < size(); ++i) {
      CandidatePtr candidate = candidates[i];
      consumer->consume(candidate->transform(), candidate);
   }
}

void CandidateSet::gen_mutations(unsigned int count) throw(Exception) {
   generate();
   assert(consumer); assert(prg);
   if (count > size()) count = size();
   /* select count candidates,
      see section 3.4.2 in Donald E. Knuth, TAOCP, Volume 2
   */
   unsigned int selected = 0; unsigned int seen = 0;
   for (unsigned int i = 0; i < size(); ++i) {
      double rval = prg->val();
      if ((candidates.size() - seen) * rval < count - selected) {
	 CandidatePtr candidate = candidates[i];
	 if (!consumer->consume(candidate->transform(), candidate)) {
	    return;
	 }
	 ++selected;
	 if (selected == count) break;
      }
      ++seen;
   }
}

CandidateSet& CandidateSet::operator+=(CandidatePtr candidate) {
   generate();
   candidates.push_back(candidate);
   return *this;
}

void CandidateSet::suppress_transformation_conflicts() {
   assert(!generated);
   suppress_conflicts = true;
}

void CandidateSet::set_consumer(ConsumerPtr consumer_param) {
   consumer = consumer_param;
}

void CandidateSet::set_prg(PseudoRandomGeneratorPtr prg_param) {
   prg = prg_param;
}

} // namespace Astl
