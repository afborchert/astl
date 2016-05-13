/*
   Copyright (C) 2009, 2016 Andreas F. Borchert
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
#include <astl/syntax-tree.hpp>

namespace Astl {

// constructors ==============================================================

Node::Node() :
      at(new Attribute()), leaf(true), context(0) {
}

Node::Node(const Node& other) :
      loc(other.loc),
      at(new Attribute()),
      leaf(other.leaf),
      context(nullptr),
      token(other.token),
      op(other.op), subnodes(other.subnodes) {
}

Node::Node(const Location& loc, const Token& token) :
      loc(loc), at(new Attribute()), leaf(true),
      context(nullptr), token(token) {
}

Node::Node(const Location& loc, const Operator& op) :
      loc(loc), at(new Attribute()), leaf(false),
      context(nullptr), op(op) {
}

Node::Node(const Location& loc, const Operator& op, NodePtr subnode) :
      loc(loc), at(new Attribute()), leaf(false),
      context(nullptr), op(op) {
   assert(subnode);
   subnodes.push_back(subnode);
}

Node::Node(const Location& loc, const Operator& op,
	 NodePtr subnode1, NodePtr subnode2) :
      loc(loc), at(new Attribute()), leaf(false),
      context(nullptr), op(op) {
   assert(subnode1); assert(subnode2);
   subnodes.push_back(subnode1);
   subnodes.push_back(subnode2);
}

Node::Node(const Location& loc, const Operator& op,
	 NodePtr subnode1, NodePtr subnode2, NodePtr subnode3) :
      loc(loc), at(new Attribute()), leaf(false),
      context(nullptr), op(op) {
   assert(subnode1); assert(subnode2); assert(subnode3);
   subnodes.push_back(subnode1);
   subnodes.push_back(subnode2);
   subnodes.push_back(subnode3);
}

Node::Node(const Location& loc, const Operator& op,
	 NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
	 NodePtr subnode4) :
      loc(loc), at(new Attribute()), leaf(false),
      context(nullptr), op(op) {
   assert(subnode1); assert(subnode2); assert(subnode3); assert(subnode4);
   subnodes.push_back(subnode1);
   subnodes.push_back(subnode2);
   subnodes.push_back(subnode3);
   subnodes.push_back(subnode4);
}

Node::Node(const Location& loc, const Operator& op,
	 NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
	 NodePtr subnode4, NodePtr subnode5) :
      loc(loc), at(new Attribute()), leaf(false),
      context(nullptr), op(op) {
   assert(subnode1); assert(subnode2); assert(subnode3);
   assert(subnode4); assert(subnode5);
   subnodes.push_back(subnode1);
   subnodes.push_back(subnode2);
   subnodes.push_back(subnode3);
   subnodes.push_back(subnode4);
   subnodes.push_back(subnode5);
}

Node::Node(const Location& loc, const Operator& op,
	 NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
	 NodePtr subnode4, NodePtr subnode5, NodePtr subnode6) :
      loc(loc), at(new Attribute()), leaf(false),
      context(nullptr), op(op) {
   assert(subnode1); assert(subnode2); assert(subnode3);
   assert(subnode4); assert(subnode5); assert(subnode6);
   subnodes.push_back(subnode1);
   subnodes.push_back(subnode2);
   subnodes.push_back(subnode3);
   subnodes.push_back(subnode4);
   subnodes.push_back(subnode5);
   subnodes.push_back(subnode6);
}

// destructor ================================================================
Node::~Node() {
   if (context) {
      delete context;
   }
}

// accessors =================================================================

const Location& Node::get_location() const {
   return loc;
}

AttributePtr Node::get_attribute() const {
   return at;
}

void Node::set_attribute(AttributePtr newat) {
   assert(newat && newat->get_type() == Attribute::dictionary);
   at = newat;
}

bool Node::is_leaf() const {
   return leaf;
}

const Token& Node::get_token() const {
   assert(leaf);
   return token;
}

Operator Node::get_op() const {
   assert(!leaf);
   return op;
}

unsigned int Node::size() const {
   assert(!leaf);
   return subnodes.size();
}

const NodePtr& Node::get_operand(unsigned int index) const {
   assert(!leaf && index < subnodes.size());
   return subnodes[index];
}

// mutators ==================================================================

Node& Node::operator=(const Node& other) {
   leaf = other.leaf; token = other.token;
   op = other.op; subnodes = other.subnodes;
   if (context) {
      delete context;
      context = nullptr;
   }
   return *this;
}

Node& Node::operator+=(NodePtr subnode) {
   assert(!leaf && subnode != nullptr);
   subnodes.push_back(subnode);
   return *this;
}

NodePtr& Node::get_operand(unsigned int index) {
   assert(!leaf && index < subnodes.size());
   return subnodes[index];
}

// context ===================================================================

void Node::set_context(const Context& context_param) {
   if (context) {
      delete context;
      context = nullptr;
   }
   context = new Context(context_param);
}

Context& Node::get_context() {
   assert(context);
   return *context;
}

// comparison ================================================================

bool Node::deep_tree_equality(NodePtr other) const {
   if (this == &(*other)) return true;
   if (leaf != other->leaf) return false;
   if (leaf) return token.get_text() == other->token.get_text();
   if (op != other->op) return false;
   if (subnodes.size() != other->subnodes.size()) return false;
   for (unsigned int i = 0; i < subnodes.size(); ++i) {
      if (!subnodes[i]->deep_tree_equality(other->subnodes[i])) return false;
   }
   return true;
}

bool deep_tree_equality(NodePtr node1, NodePtr node2) {
   return node1->deep_tree_equality(node2);
}

// I/O methods ===============================================================

static void visit_node(std::ostream& out, unsigned int level,
      NodePtr node) {
   if (node->is_leaf()) {
      Token token = node->get_token();
      out << "'" << token.get_text() << "'";
   } else {
      out << "(\"" << node->get_op() << "\"";
      AttributePtr dict = node->get_attribute();
      if (dict->size() > 0) {
	 out << " " << dict;
      }
      if (node->size() > 0) {
	 out << std::endl;
	 for (unsigned int i = 0; i < node->size(); ++i) {
	    for (unsigned int j = 0; j < level+1; ++j) out << "   ";
	    visit_node(out, level+1, node->get_operand(i));
	    if (i + 1 < node->size()) {
	       out << ", ";
	    }
	    out << std::endl;
	 }
	 for (unsigned int i = 0; i < level; ++i) out << "   ";
      }
      out << ")";
   }
}

std::ostream& operator<<(std::ostream& out, NodePtr snode) {
   visit_node(out, 0, snode);
   return out;
}

} // namespace
