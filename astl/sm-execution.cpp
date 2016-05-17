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

#include <cstdlib>
#include <stack>
#include <memory>
#include <astl/sm-execution.hpp>
#include <astl/execution.hpp>
#include <astl/flow-graph.hpp>
#include <astl/tree-expressions.hpp>
#include <astl/expression.hpp>

namespace Astl {

struct Instance;
typedef std::shared_ptr<Instance> InstancePtr;
struct InstanceThread;
struct CacheKey;
struct CacheEntry;
typedef std::map<CacheKey, CacheEntry> Cache;

/*
   Shared data structure for a given state machine that has been
   created at a particular control flow node;
   here we have
    - the data structure about which nodes have been visited
      with which states,
    - the cache for the interprocedural analysis, and
    - the shared variables of a state machine
*/
struct Instance {
   Instance(StateMachinePtr sm_param, BindingsPtr bindings_param) :
      sm(sm_param), bindings(bindings_param) {
   }
   bool visited(unsigned int id, unsigned int state) const {
      Map::const_iterator it = states.find(id);
      if (it == states.end()) return false;
      const StateSet& set(it->second);
      if (state >= set.size()) return false;
      return set.test(state);
   }
   bool visit(unsigned int id, unsigned int state) {
      StateSet stateset(sm->get_nofxstates()); stateset.set(state);
      std::pair<Map::iterator, bool> result =
	 states.insert(make_pair(id, stateset));
      if (result.second) {
	 return true;
      }
      StateSet& set(result.first->second);
      if (set.test(state)) return false;
      set.set(state);
      return true;
   }
   void retract(unsigned int id, unsigned int state) {
      Map::iterator it = states.find(id);
      assert(it != states.end());
      it->second.reset(state);
   }
   StateMachinePtr sm;
   BindingsPtr bindings; // shared bindings
   typedef std::map<unsigned int, StateSet> Map;
   Map states; // set of states per cfg node
   // cache data structure, used by the cache action
   // this is required to support interprocedural paths for
   // global state machines
   Cache cache;
};

/*
   Data structure for a particular path taken by a state
   machine. Here we maintain
    - the current state of a state machine,
    - the private variables, and
    - the stack used for the interprocedural analysis
*/
struct InstanceThread {
   InstanceThread(InstancePtr smi_param) :
	 smi(smi_param), state(0),
	 bindings(smi->sm->add_private_bindings(smi->bindings)),
	 close_id(0) {
   }
   void fork_bindings() {
      // get shared bindings
      BindingsPtr new_bindings = std::make_shared<Bindings>(smi->bindings);
      // and duplicate private bindings
      new_bindings->merge(bindings);
      // and update local functions
      smi->sm->update_function_bindings(new_bindings);
      bindings = new_bindings;
   }
   unsigned int combined_state() const {
      if (stack.size() > 0) {
	 return
	    stack.top().entry_state * smi->sm->get_nofstates() + state;
      } else {
	 return state;
      }
   }
   bool visited(unsigned int id) const {
      return smi->visited(id, combined_state());
   }
   bool visit(unsigned int id) {
      return smi->visit(id, combined_state());
   }
   void retract(unsigned int id) {
      return smi->retract(id, combined_state());
   }
   InstancePtr smi;
   unsigned int state; // current state
   BindingsPtr bindings; // private bindings
   // used by the cache action:
   typedef struct {
      FlowGraphNodePtr return_node;
      unsigned int entry_id;
      unsigned int entry_state;
   } StackEntry;
   std::stack<StackEntry> stack; // used by cache action
   unsigned int close_id; // used for the detection of an endless recursion
};

struct CacheKey {
   unsigned int state; // entry state
   unsigned int entry_id;
   bool operator<(const CacheKey& other) const {
      return state < other.state ||
             (state == other.state && entry_id < other.entry_id);
   }
   bool operator==(const CacheKey& other) const {
      return state == other.state && entry_id == other.entry_id;
   }
};
struct CacheInstance {
   InstanceThread instance;
   FlowGraphNodePtr fgnode;
};
struct CacheEntry {
   // cached exit states seen so far
   StateSet states;
   // list of instances which have to be forked off
   // whenever a new exit state is seen
   std::list<CacheInstance> instances;
};

/*
   Collection of instance threads that share a location
*/
struct Thread {
   typedef std::list<InstanceThread>::iterator Iterator;
   std::list<InstanceThread> instances;
   FlowGraphNodePtr node;
};

typedef std::list<Thread> ThreadList;

/*
   Global data structures including
    - the global bindings,
    - the thread list, and
    - the table of state machines that have been created
      at a particular control flow node
*/
struct ExecutionContext {
   ExecutionContext() : next_id(1) {
   }
   bool creatable(unsigned int node_id, unsigned int sm_id) {
      StateMachineSet smset(nof_sms); smset.set(sm_id);
      std::pair<Map::iterator, bool> result =
	 created.insert(make_pair(node_id, smset));
      if (result.second) {
	 return true;
      }
      StateMachineSet& set(result.first->second);
      if (set.test(sm_id)) return false;
      set.set(sm_id);
      return true;
   }
   void set_close_id(InstanceThread& it) {
      if (!it.close_id) it.close_id = next_id++;
   }
   void add_candidate_for_close(InstanceThread& it) {
      assert(it.close_id);
      candidates_for_close.insert(std::make_pair(it.close_id, it));
   }
   void remove_candidate_for_close(InstanceThread& it) {
      candidates_for_close.erase(it.close_id);
   }
   void run_close_handlers() {
      for (InstanceThreadMap::iterator it = candidates_for_close.begin();
	    it != candidates_for_close.end();
	    ++it) {
	 it->second.smi->sm->run_close_handlers(it->second.state,
	       it->second.bindings);
      }
      candidates_for_close.clear();
   }
   BindingsPtr bindings;
   ThreadList threads;
   // which sms have been created/tested at a particular cfg node?
   typedef std::map<unsigned int, StateMachineSet> Map;
   Map created;
   unsigned int nof_sms;
   // which instance threads need to be closed at the end?
   typedef std::map<unsigned int, InstanceThread> InstanceThreadMap;
   unsigned int next_id;
   InstanceThreadMap candidates_for_close;
};

static FlowGraphNodePtr get_root(BindingsPtr bindings) {
   FlowGraphNodePtr root;
   AttributePtr graph = bindings->get("graph");
   if (graph && graph->get_type() == Attribute::dictionary &&
	 graph->is_defined("root")) {
      AttributePtr rootAt = graph->get_value("root");
      if (rootAt && rootAt->get_type() == Attribute::flow_graph_node) {
	 root = rootAt->get_fgnode();
      }
   }
   return root;
}

static InstanceThread create_instance(ExecutionContext& ec, StateMachinePtr sm,
      FlowGraphNodePtr start) {
   InstancePtr ip = std::make_shared<Instance>(sm, sm->get_shared_bindings());
   StateMachineSet smset(ec.nof_sms);
   smset.set(sm->get_id());
   std::pair<ExecutionContext::Map::iterator, bool> result =
      ec.created.insert(make_pair(start->get_id(), smset));
   if (!result.second) {
      result.first->second.set(sm->get_id());
   }
   // StateSet states(sm->get_nofstates()); states.set(0);
   // ip->states[start->get_id()] = states;
   return InstanceThread(ip);
}

void check_creation(ExecutionContext& ec, StateMachinePtr sm,
	    FlowGraphNodePtr fgnode, std::list<InstanceThread>& instances) {
   unsigned int node_type = fgnode->get_type_number();
   NodePtr ast = fgnode->get_node();
   for (StateMachine::Iterator it = sm->get_creating_rules_begin();
	 it != sm->get_creating_rules_end(); ++it) {
      StateMachineRulePtr smr = *it;
      const NodeTypeSet& ntset = smr->get_nodetypes();
      // skip rule if the node type does not match
      if (ntset.size() > 0 &&
	    (node_type >= ntset.size() || !ntset.test(node_type))) {
	 continue;
      }
      // check tree expression, if any
      BindingsPtr bindings = std::make_shared<Bindings>(ec.bindings);
      if (smr->tree_expr_defined()) {
	 if (!ast) continue;
	 if (ast->is_leaf()) continue;
	 Arity arity = smr->get_arity();
	 if (arity.fixed && arity.arity != ast->size()) continue;
	 std::string opname(ast->get_op().get_name());
	 if (!smr->get_opset()->includes(opname)) continue;
	 NodePtr tree_expr = smr->get_tree_expression();
	 if (!matches(ast, tree_expr, bindings, ast->get_context())) continue;
      }
      // check node condition, if any
      NodePtr nodecond = smr->get_node_condition();
      bindings->define("node", std::make_shared<Attribute>(fgnode));
      if (nodecond) {
	 Expression cond(nodecond, bindings);
	 if (!cond.convert_to_bool()) continue;
      }
      InstanceThread ithread = create_instance(ec, sm, fgnode);
      instances.push_back(ithread);
      NodePtr block = smr->get_block();
      if (block) {
	 BindingsPtr local_bindings =
	    std::make_shared<Bindings>(ithread.bindings);
	 local_bindings->merge(bindings);
	 execute(block, local_bindings);
      }
      return;
   }
}

void check_cache(ExecutionContext& ec, InstanceThread& t,
      unsigned int successor_index) {
   if (t.stack.size() == 0) return;
   InstanceThread::StackEntry entry = t.stack.top();
   if (successor_index != entry.return_node->get_id()) return;
   // ok, we are returning from a procedure call
   t.stack.pop();
   // cache new exit state
   InstancePtr smi = t.smi;
   CacheKey key = {entry.entry_state, entry.entry_id};
   Cache::iterator it = smi->cache.find(key);
   assert(it != smi->cache.end());
   it->second.states.set(t.state);
   // fork off new instance threads for all who are
   // waiting for new exit states
   for (std::list<CacheInstance>::const_iterator iit =
	 it->second.instances.begin();
	 iit != it->second.instances.end(); ++iit) {
      Thread nthread; nthread.node = iit->fgnode;
      InstanceThread newt = iit->instance;
      newt.fork_bindings();
      newt.state = t.state;
      if (newt.close_id) ec.remove_candidate_for_close(newt);
      nthread.instances.push_back(newt);
      ec.threads.push_back(nthread);
   }
}

bool execute_rules(ExecutionContext& ec, InstanceThread& t,
      const std::string& label_text, unsigned int label_index,
      FlowGraphNodePtr fgnode, unsigned int successor_index) {
   InstancePtr smi = t.smi;
   StateMachinePtr sm = smi->sm;
   unsigned int node_type = fgnode->get_type_number();
   NodePtr ast = fgnode->get_node();
   int newstate = t.state;
   bool cut = false;
   for (StateMachine::Iterator it = sm->get_rules_begin();
	 !cut && it != sm->get_rules_end(); ++it) {
      StateMachineRulePtr smr = *it;
      const NodeTypeSet& ntset = smr->get_nodetypes();
      // skip rule if the node type does not match
      if (ntset.size() > 0 &&
	    (node_type >= ntset.size() || !ntset.test(node_type))) {
	 continue;
      }
      // check tree expression, if any
      BindingsPtr bindings = std::make_shared<Bindings>(t.bindings);
      if (smr->tree_expr_defined()) {
	 if (!ast) continue;
	 if (ast->is_leaf()) continue;
	 Arity arity = smr->get_arity();
	 if (arity.fixed && arity.arity != ast->size()) continue;
	 std::string opname(ast->get_op().get_name());
	 if (!smr->get_opset()->includes(opname)) continue;
	 NodePtr tree_expr = smr->get_tree_expression();
	 if (!matches(ast, tree_expr, bindings, ast->get_context())) continue;
	 bindings = std::make_shared<Bindings>(bindings);
      }
      // check node condition, if any
      NodePtr nodecond = smr->get_node_condition();
      bindings->define("node", std::make_shared<Attribute>(fgnode));
      bindings->define("current_state",
	 std::make_shared<Attribute>(sm->get_state_by_number(t.state)));
      bindings->define("label", std::make_shared<Attribute>(label_text));
      if (nodecond) {
	 Expression cond(nodecond, bindings);
	 if (!cond.convert_to_bool()) continue;
      }
      // check individual alternatives
      for (StateMachineRule::AlternativesIterator ait =
	    smr->get_alternatives_begin();
	    !cut && ait != smr->get_alternatives_end(); ++ait) {
	 StateMachineRuleAlternativePtr alt = *ait;
	 // check if one of the given labels matches
	 const LabelSet& labels(alt->get_labels());
	 if (labels.size() > 0 &&
	       (label_index >= labels.size() || !labels.test(label_index))) {
	    continue;
	 }
	 // check if one of the given states matches
	 const StateSet& states(alt->get_states());
	 if (states.size() > 0 &&
	       (t.state >= states.size() || !states.test(t.state))) {
	    continue;
	 }
	 // execute it
	 if (alt->get_newstate() >= 0) {
	    newstate = alt->get_newstate();
	 }
	 NodePtr block = alt->get_block();
	 if (block) {
	    execute(block, bindings);
	 }
	 switch (alt->get_action()) {
	    case StateMachineRuleAlternative::close:
	       sm->run_close_handlers(t.state, bindings);
	       /* fall through */
	    case StateMachineRuleAlternative::cut:
	       cut = true;
	       break;
	    case StateMachineRuleAlternative::retract:
	       t.retract(fgnode->get_id());
	       cut = true;
	       break;
	    case StateMachineRuleAlternative::cache: {
		  NodePtr expr = alt->get_return_node();
		  Expression e(expr, bindings);
		  AttributePtr result = e.get_result();
		  if (!result ||
			result->get_type() != Attribute::flow_graph_node) {
		     throw Exception(expr->get_location(),
			"control flow graph node expected");
		  }
		  FlowGraphNodePtr fgnode = result->get_fgnode();
		  CacheKey key = {t.state, successor_index};
		  ec.set_close_id(t);
		  if (t.visited(successor_index)) {
		     Cache::iterator it = smi->cache.find(key);
		     bool forked = false;
		     if (it != smi->cache.end()) {
			// take a shortcut to the rtn node,
			// using all the cached states
			Thread nthread; nthread.node = fgnode;
			StateSet states = it->second.states;
			unsigned int state = states.find_first();
			while (state < states.size()) {
			   InstanceThread newt = t;
			   newt.fork_bindings();
			   newt.state = state;
			   nthread.instances.push_back(newt);
			   state = states.find_next(state);
			}
			if (nthread.instances.size() > 0) {
			   ec.threads.push_back(nthread);
			   forked = true;
			}
			// add this instance to the list such that
			// we are able to fork off further instances
			// whenever a new exit state is seen
			CacheInstance ci = {t, fgnode};
			it->second.instances.push_back(ci);
		     } else {
			// visited, but no results so far:
			// just create a new entry for it
			CacheEntry ce;
			CacheInstance ci = {t, fgnode};
			ce.states.resize(sm->get_nofstates());
			ce.instances.push_back(ci);
			smi->cache.insert(std::make_pair(key, ce));
		     }
		     if (!forked) {
			ec.add_candidate_for_close(t);
		     }
		     cut = true;
		  } else {
		     // just keep notice and continue
		     InstanceThread::StackEntry entry =
			{fgnode, successor_index, t.state};
		     t.stack.push(entry);
		     CacheEntry ce;
		     ce.states.resize(sm->get_nofstates());
		     smi->cache.insert(std::make_pair(key, ce));
		  }
	       }
	       break;
	    case StateMachineRuleAlternative::null:
	       break;
	    default:
	       assert(false); std::abort();
	 }
      }
   }
   t.state = newstate;
   return !cut;
}

StateMachinePtr create_dummy_sm(unsigned int id, BindingsPtr bindings) {
   StateMachinePtr dummy = std::make_shared<StateMachine>(bindings,
      "$dummy", id);
   Location loc; dummy->add_state("start", loc);
   return dummy;
}

void execute_state_machines(const Rules& rules, BindingsPtr bindings)
      throw(Exception) {
   Thread thread; thread.node = get_root(bindings);
   if (!thread.node) return; // no starting point
   const StateMachineTable& smtab = rules.get_sm_table(bindings);
   ExecutionContext ec;
   ec.bindings = bindings; ec.nof_sms = smtab.nof_state_machines() + 1;
   // create instances of global state machines
   for (StateMachineTable::Iterator it = smtab.get_global_begin();
	 it != smtab.get_global_end(); ++it) {
      InstanceThread ithread = create_instance(ec, *it, thread.node);
      thread.instances.push_back(ithread);
   }
   // create dummy state machine
   // (we need this to ensure that all reachable cfg nodes get
   // visited such that creation rules of local state machines can fire)
   InstanceThread dummy = create_instance(ec,
      create_dummy_sm(smtab.nof_state_machines(), bindings), thread.node);
   thread.instances.push_back(dummy);
   // start execution with initial thread
   ec.threads.push_back(thread);
   while (ec.threads.size() > 0) {
      Thread thread = ec.threads.front(); ec.threads.pop_front();
      FlowGraphNodePtr node = thread.node;
      unsigned int node_id = node->get_id();
      std::list<InstanceThread> instances;
      for (Thread::Iterator iit = thread.instances.begin();
	    iit != thread.instances.end(); ++iit) {
	 if (!iit->visit(node_id)) continue;
	 instances.push_back(*iit);
      }
      // check for new sm instances
      for (StateMachineTable::Iterator it = smtab.get_local_begin();
	    it != smtab.get_local_end(); ++it) {
	 StateMachinePtr sm = *it;
	 if (ec.creatable(node->get_id(), sm->get_id())) {
	    check_creation(ec, sm, node, instances);
	 }
      }
      if (node->get_number_of_outgoing_links() == 0) {
	 // exit node
	 for (Thread::Iterator iit = instances.begin();
	       iit != instances.end(); ++iit) {
	    execute_rules(ec, *iit, "", 0, node, 0);
	    iit->smi->sm->run_close_handlers(iit->state, iit->bindings);
	 }
      } else {
	 for (FlowGraphNode::Iterator it = node->begin_links();
	       it != node->end_links(); ++it) {
	    std::string label_text = it->first;
	    unsigned int label_index = label_by_name(bindings, label_text);
	    FlowGraphNodePtr successor = it->second;
	    Thread nthread; nthread.node = successor;
	    unsigned int id = successor->get_id();
	    for (Thread::Iterator iit = instances.begin();
		  iit != instances.end(); ++iit) {
	       InstanceThread t = *iit;
	       t.fork_bindings();
	       if (execute_rules(ec, t, label_text, label_index, node, id)) {
		  check_cache(ec, t, id);
		  nthread.instances.push_back(t);
	       }
	    }
	    if (nthread.instances.size() > 0) {
	       // note that we continue the actual thread as far
	       // as possible to avoid races in case of caching
	       // when two different threads hit the same function
	       ec.threads.push_front(nthread);
	    }
	 }
      }
   }
   ec.run_close_handlers();
}

} // namespace Astl
