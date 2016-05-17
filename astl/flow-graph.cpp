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
#include <utility>
#include <astl/flow-graph.hpp>
#include <astl/attribute.hpp>
#include <astl/treeloc.hpp>
#include <astl/exception.hpp>

namespace Astl {

#define NODE_TYPES "node_types"
#define LABELS "labels"
#define ID "nextid"

static unsigned int by_name(BindingsPtr bindings,
      const std::string& dict, const std::string& key) {
   if (key.size() == 0) return 0;
   if (!bindings) return 0;
   AttributePtr graph = bindings->get("graph");
   if (!graph) return 0;
   if (graph->get_type() != Attribute::dictionary) return 0;
   if (!graph->is_defined(dict)) {
      graph->update(dict, std::make_shared<Attribute>());
   }
   AttributePtr dictAt = graph->get_value(dict);
   assert(dictAt);
   if (dictAt->is_defined(key)) {
      AttributePtr val = dictAt->get_value(key);
      if (val->get_type() == Attribute::integer) {
	 Location loc;
	 return val->get_integer()->get_unsigned_int(loc);
      } else {
	 return 0;
      }
   } else {
      // +1 as 0 is reserved for empty keys and various error conditions
      unsigned int next = dictAt->size() + 1;
      dictAt->update(key, std::make_shared<Attribute>(next));
      return next;
   }
}

static unsigned int nof(BindingsPtr bindings,
      const std::string& dict) {
   if (!bindings) return 1;
   AttributePtr graph = bindings->get("graph");
   if (!graph) return 1;
   if (graph->get_type() != Attribute::dictionary) return 1;
   if (!graph->is_defined(dict)) {
      graph->update(dict, std::make_shared<Attribute>());
   }
   AttributePtr dictAt = graph->get_value(dict);
   assert(dictAt);
   return dictAt->size() + 1;
}

unsigned int nof_node_types(BindingsPtr bindings) {
   return nof(bindings, NODE_TYPES);
}

unsigned int node_type_by_name(BindingsPtr bindings,
      const std::string& type) {
   return by_name(bindings, NODE_TYPES, type);
}

unsigned int nof_labels(BindingsPtr bindings) {
   return nof(bindings, LABELS);
}

unsigned int label_by_name(BindingsPtr bindings,
      const std::string& label) {
   return by_name(bindings, LABELS, label);
}

static unsigned int new_id(BindingsPtr bindings) {
   if (!bindings) return 0;
   AttributePtr graph = bindings->get("graph");
   if (!graph) return 0;
   if (graph->get_type() != Attribute::dictionary) return 0;
   if (!graph->is_defined(ID)) {
      graph->update(ID, std::make_shared<Attribute>(1));
   }
   AttributePtr idAt = graph->get_value(ID);
   assert(idAt);
   Location loc;
   unsigned int id = idAt->get_integer()->get_unsigned_int(loc);
   graph->update(ID, std::make_shared<Attribute>(id + 1));
   return id;
}

FlowGraphNode::FlowGraphNode(BindingsPtr bindings) :
      bindings(bindings), id(new_id(bindings)),
      type_number(0), at(std::make_shared<Attribute>()) {
   at->update("branch", std::make_shared<Attribute>());
}

FlowGraphNode::FlowGraphNode(BindingsPtr bindings,
	 const std::string& type) :
      bindings(bindings), id(new_id(bindings)), type(type),
      type_number(node_type_by_name(bindings, type)),
      at(std::make_shared<Attribute>()) {
   at->update("branch", std::make_shared<Attribute>());
}

FlowGraphNode::FlowGraphNode(BindingsPtr bindings, NodePtr node) :
      bindings(bindings), id(new_id(bindings)),
      type_number(0), node(node),
      at(std::make_shared<Attribute>()) {
   assert(node);
   at->update("astnode", std::make_shared<Attribute>(node));
   at->update("branch", std::make_shared<Attribute>());
}

FlowGraphNode::FlowGraphNode(BindingsPtr bindings,
	 const std::string& type, NodePtr node) :
      bindings(bindings), id(new_id(bindings)),
      type(type), type_number(node_type_by_name(bindings, type)),
      node(node), at(std::make_shared<Attribute>()) {
   assert(node);
   at->update("astnode", std::make_shared<Attribute>(node));
   at->update("branch", std::make_shared<Attribute>());
}

void FlowGraphNode::link(FlowGraphNodePtr fgnode) {
   assert(fgnode);
   links.push_back(std::make_pair("", fgnode));
}

void FlowGraphNode::link(FlowGraphNodePtr fgnode, const std::string& label) {
   assert(fgnode);
   links.push_back(std::make_pair(label, fgnode));
   labeled_links[label] = fgnode;
   AttributePtr branches = at->get_value("branch");
   branches->update(label, std::make_shared<Attribute>(fgnode));
}

unsigned int FlowGraphNode::get_id() const {
   return id;
}

const std::string& FlowGraphNode::get_type() const {
   return type;
}

unsigned int FlowGraphNode::get_type_number() const {
   return type_number;
}

NodePtr FlowGraphNode::get_node() const {
   return node;
}

AttributePtr FlowGraphNode::get_attribute() const {
   return at;
}

unsigned int FlowGraphNode::get_number_of_outgoing_links() const {
   return links.size();
}

FlowGraphNode::Iterator FlowGraphNode::begin_links() const {
   return links.begin();
}

FlowGraphNode::Iterator FlowGraphNode::end_links() const {
   return links.end();
}

FlowGraphNodePtr FlowGraphNode::get_branch(const std::string& label) const {
   BranchIterator it = labeled_links.find(label);
   if (it == labeled_links.end()) {
      return FlowGraphNodePtr(nullptr);
   } else {
      return it->second;
   }
}

FlowGraphNode::BranchIterator FlowGraphNode::begin_branches() const {
   return labeled_links.begin();
}

FlowGraphNode::BranchIterator FlowGraphNode::end_branches() const {
   return labeled_links.end();
}

} // namespace Astl
