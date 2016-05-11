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

#include <cassert>
#include <astl/syntax-tree.hpp>

namespace Astl {

#ifdef USE_UNRESTRICTED_UNIONS
#define op opnode._op
#define subnode opnode._subnode
#endif

// constructors ==============================================================

Node::Node() :
      at(new Attribute()), leaf(true), context(0) {
#ifdef USE_UNRESTRICTED_UNIONS
      new(&token) Token;
#endif
}

#ifdef USE_FLAT_UNIONS
Node::Node(const Node& other) :
      at(new Attribute()), context(0),
      loc(other.loc), leaf(other.leaf), token(other.token),
      op(other.op), subnode(other.subnode) {
}

Node::Node(const Location& loc_param, const Token& token_param) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(true), token(token_param) {
}

Node::Node(const Location& loc_param, const Operator& op_param) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false), op(op_param) {
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false), op(op_param) {
   assert(subnode1);
   subnode.push_back(subnode1);
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1, NodePtr subnode2) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false), op(op_param) {
   assert(subnode1); assert(subnode2);
   subnode.push_back(subnode1);
   subnode.push_back(subnode2);
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1, NodePtr subnode2, NodePtr subnode3) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false), op(op_param) {
   assert(subnode1); assert(subnode2); assert(subnode3);
   subnode.push_back(subnode1);
   subnode.push_back(subnode2);
   subnode.push_back(subnode3);
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
      NodePtr subnode4) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false), op(op_param) {
   assert(subnode1); assert(subnode2); assert(subnode3); assert(subnode4);
   subnode.push_back(subnode1);
   subnode.push_back(subnode2);
   subnode.push_back(subnode3);
   subnode.push_back(subnode4);
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
      NodePtr subnode4, NodePtr subnode5) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false), op(op_param) {
   assert(subnode1); assert(subnode2); assert(subnode3);
   assert(subnode4); assert(subnode5);
   subnode.push_back(subnode1);
   subnode.push_back(subnode2);
   subnode.push_back(subnode3);
   subnode.push_back(subnode4);
   subnode.push_back(subnode5);
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
      NodePtr subnode4, NodePtr subnode5, NodePtr subnode6) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false), op(op_param) {
   assert(subnode1); assert(subnode2); assert(subnode3);
   assert(subnode4); assert(subnode5); assert(subnode6);
   subnode.push_back(subnode1);
   subnode.push_back(subnode2);
   subnode.push_back(subnode3);
   subnode.push_back(subnode4);
   subnode.push_back(subnode5);
   subnode.push_back(subnode6);
}
#endif

#ifdef USE_UNRESTRICTED_UNIONS
Node::Node(const Node& other) :
      at(new Attribute()), context(0),
      loc(other.loc), leaf(other.leaf) {
   if (leaf) {
      new(&token) Token(other.token);
   } else {
      new(&op) Operator(other.op);
      new(&subnode) NodePtrList(other.subnode);
   }
}

Node::Node(const Location& loc_param, const Token& token_param) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(true) {
   new(&token) Token(token_param);
}

Node::Node(const Location& loc_param, const Operator& op_param) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false) {
   new(&op) Operator(op_param);
   new(&subnode) NodePtrList();
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false) {
   new(&op) Operator(op_param);
   new(&subnode) NodePtrList();
   assert(subnode1);
   subnode.push_back(subnode1);
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1, NodePtr subnode2) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false) {
   new(&op) Operator(op_param);
   new(&subnode) NodePtrList();
   assert(subnode1); assert(subnode2);
   subnode.push_back(subnode1);
   subnode.push_back(subnode2);
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1, NodePtr subnode2, NodePtr subnode3) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false) {
   new(&op) Operator(op_param);
   new(&subnode) NodePtrList();
   assert(subnode1); assert(subnode2); assert(subnode3);
   subnode.push_back(subnode1);
   subnode.push_back(subnode2);
   subnode.push_back(subnode3);
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
      NodePtr subnode4) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false) {
   new(&op) Operator(op_param);
   new(&subnode) NodePtrList();
   assert(subnode1); assert(subnode2); assert(subnode3); assert(subnode4);
   subnode.push_back(subnode1);
   subnode.push_back(subnode2);
   subnode.push_back(subnode3);
   subnode.push_back(subnode4);
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
      NodePtr subnode4, NodePtr subnode5) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false) {
   new(&op) Operator(op_param);
   new(&subnode) NodePtrList();
   assert(subnode1); assert(subnode2); assert(subnode3);
   assert(subnode4); assert(subnode5);
   subnode.push_back(subnode1);
   subnode.push_back(subnode2);
   subnode.push_back(subnode3);
   subnode.push_back(subnode4);
   subnode.push_back(subnode5);
}

Node::Node(const Location& loc_param, const Operator& op_param,
      NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
      NodePtr subnode4, NodePtr subnode5, NodePtr subnode6) :
      at(new Attribute()), context(0),
      loc(loc_param), leaf(false) {
   new(&op) Operator(op_param);
   new(&subnode) NodePtrList();
   assert(subnode1); assert(subnode2); assert(subnode3);
   assert(subnode4); assert(subnode5); assert(subnode6);
   subnode.push_back(subnode1);
   subnode.push_back(subnode2);
   subnode.push_back(subnode3);
   subnode.push_back(subnode4);
   subnode.push_back(subnode5);
   subnode.push_back(subnode6);
}
#endif

// destructor ================================================================
Node::~Node() {
   if (context) {
      delete context;
      context = 0;
   }
#ifdef USE_UNRESTRICTED_UNIONS
   /*
   if (leaf) {
      token.~Token();
   } else {
      op.~Operator();
      subnode.~NodePtrList();
   }
   */
#endif
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
   return subnode.size();
}

const NodePtr& Node::get_operand(unsigned int index) const {
   assert(!leaf && index < subnode.size());
   NodePtrList::const_iterator it = subnode.begin();
   while (index-- > 0) {
      ++it;
   }
   return *it;
}

// mutators ==================================================================

Node& Node::operator=(const Node& other) {
   leaf = other.leaf; token = other.token;
   op = other.op; subnode = other.subnode;
   if (context) {
      delete context;
      context = 0;
   }
   return *this;
}

Node& Node::operator+=(NodePtr subnode1) {
   assert(!leaf && subnode1 != 0);
   subnode.push_back(subnode1);
   return *this;
}

NodePtr& Node::get_operand(unsigned int index) {
   assert(!leaf && index < subnode.size());
   NodePtrList::iterator it = subnode.begin();
   while (index-- > 0) {
      ++it;
   }
   return *it;
}

// context ===================================================================

void Node::set_context(const Context& context_param) {
   if (context) {
      delete context;
      context = 0;
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
   if (subnode.size() != other->subnode.size()) return false;
   std::list<NodePtr>::const_iterator
      it1(subnode.begin()), it2(other->subnode.begin());
   while (it1 != subnode.end()) {
      if (!(*it1++)->deep_tree_equality(*it2++)) return false;
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
	 for (int i = 0; i < node->size(); ++i) {
	    for (int j = 0; j < level+1; ++j) out << "   ";
	    visit_node(out, level+1, node->get_operand(i));
	    if (i + 1 < node->size()) {
	       out << ", ";
	    }
	    out << std::endl;
	 }
	 for (int i = 0; i < level; ++i) out << "   ";
      }
      out << ")";
   }
}

std::ostream& operator<<(std::ostream& out, NodePtr snode) {
   visit_node(out, 0, snode);
   return out;
}

} // namespace
