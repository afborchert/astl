/*
   Copyright (C) 2009, 2010, 2016 Andreas F. Borchert
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

#ifndef ASTL_SYNTAX_TREE_H
#define ASTL_SYNTAX_TREE_H

#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
#include <astl/attribute.hpp>
#include <astl/context.hpp>
#include <astl/operator.hpp>
#include <astl/token.hpp>
#include <astl/treeloc.hpp>
#include <astl/types.hpp>

namespace Astl {

   /**
    * A syntax tree is represented by a node which, if it
    * is not a leaf node, has one or more subnodes which
    * represent subtrees.
    *
    * Leaf nodes represent a lexical token while non-leaf
    * nodes, so-called operator nodes, consist of an operator
    * and an arbitrary number of subnodes. Please note that
    * it is possible to have an operator node without
    * any subnodes. Such an operator node is technically
    * a leaf node but not treated as such, i.e. is_leaf()
    * returns false for all operator nodes even if they have
    * no subnodes.
    *
    * Each node is associated with a location which specifies
    * a stretch of a source file. Locations of the bison-generated
    * location type must be converted to objects of type Location
    * by using the make_loc function out of location.hpp.
    *
    * Just a few mutators are supported: Nodes can be assigned to
    * and operands can be added to an operator node. Individual
    * subnodes (or operands) can be replaced by retrieving a
    * NodePtr reference through get_operand() and then by assigning
    * another subnode to it.
    *
    * The creation of cyclic references is not permitted (but also
    * not checked for). Shared references to the same subnode are
    * permitted.
    */
   class Node {
      public:
	 // general constructors
	 Node(); /**< Create a leaf node with a default-constructed token. */
	 Node(const Node& other); /**< Copy constructor */

	 /**
	  * Construction of a leaf node that represents a lexical token.
	  */
	 Node(const Location& loc, const Token& token);

	 // operator node constructors
	 Node(const Location& loc, const Operator& op);
	 Node(const Location& loc, const Operator& op,
	    NodePtr subnode1);
	 Node(const Location& loc, const Operator& op,
	    NodePtr subnode1, NodePtr subnode2);
	 Node(const Location& loc, const Operator& op,
	    NodePtr subnode1, NodePtr subnode2, NodePtr subnode3);
	 Node(const Location& loc, const Operator& op,
	    NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
	    NodePtr subnode4);
	 Node(const Location& loc, const Operator& op,
	    NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
	    NodePtr subnode4, NodePtr subnode5);
	 Node(const Location& loc, const Operator& op,
	    NodePtr subnode1, NodePtr subnode2, NodePtr subnode3,
	    NodePtr subnode4, NodePtr subnode5, NodePtr subnode6);

	 // deconstructor
	 ~Node();

	 /**
	  * Return the location of the node which is defined by 
	  * the concatenated locations of the individual tokens
	  * that represent this node.
	  */
	 const Location& get_location() const;

	 /**
	  * Return the attribute associated with the node.
	  * This is always a dictionary.
	  */
	 AttributePtr get_attribute() const;

	 /**
	  * Set the attribute. This must be a dictionary.
	  * This attribute shall not be shared with any other node.
	  * (To assure this, the clone method can be used.)
	  */
	 void set_attribute(AttributePtr newat);

	 /**
	  * Returns true for leaf nodes that represent lexical tokens.
	  */
	 bool is_leaf() const;

	 // ===== accessors for leaf nodes =====
	 /**
	  * This accessor is restricted to leaf nodes representing
	  * a lexical token and returns a copy of the token.
	  */
	 const Token& get_token() const;

	 // ===== accessors for operator nodes =====
	 /**
	  * This accessor is restricted to operator nodes and
	  * returns a copy of the operator.
	  */
	 Operator get_op() const;

	 /**
	  * This accessor is restricted to operator nodes and
	  * returns the number of operands (i.e. subnodes).
	  */
	 std::size_t size() const;

	 /**
	  * This accessor is restricted to operator nodes and
	  * returns the index-th operand, counting from 0.
	  */
	 const NodePtr& get_operand(std::size_t index) const;

	 /**
	  * Assignment operator for nodes.
	  */
	 Node& operator=(const Node& other);

	 /**
	  * This operator is restricted to operator nodes and
	  * allows to add another operand to a operator node.
	  */
	 Node& operator+=(NodePtr subnode1);

	 /**
	  * This mutator is restricted to operator nodes
	  * and allows to replace the index-th operand.
	  */
	 NodePtr& get_operand(std::size_t index);

	 void set_context(const Context& context_param);
	 Context& get_context();

	 bool deep_tree_equality(NodePtr other) const;

      private:
	 Location loc;
	 AttributePtr at;
	 bool leaf;

	 /* declared as pointer to save space when it is not required:
	    on a 64 bit architecture we just need 8 instead of 48 bytes */
	 std::unique_ptr<Context> context;

	 // leaf node
	 Token token;

	 // operator node
	 Operator op;
	 std::vector<NodePtr> subnodes;
   };
   bool deep_tree_equality(NodePtr node1, NodePtr node2);

   /**
    * This output operator prints a textual representation of
    * an entire tree using a nested Lisp-like notation with
    * parentheses that extends over multiple output lines.
    * It is intended for debugging purposes. Please note that
    * as tokens are printed as-is it is not possible to parse
    * the output text.
    */
   std::ostream& operator<<(std::ostream& out, NodePtr node);

} // namespace Astl

#endif
