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

#ifndef ASTL_FLOW_GRAPH_H
#define ASTL_FLOW_GRAPH_H

#include <string>
#include <list>
#include <map>
#include <astl/types.hpp>
#include <boost/dynamic_bitset.hpp>

namespace Astl {

   typedef boost::dynamic_bitset<unsigned long> LabelSet;
   typedef boost::dynamic_bitset<unsigned long> NodeTypeSet;
   typedef boost::dynamic_bitset<unsigned long> StateMachineSet;

   class FlowGraphNode {
      public:
	 typedef std::list<std::pair<std::string,
	    FlowGraphNodePtr> >::const_iterator Iterator;
	 typedef std::map<std::string, FlowGraphNodePtr>::const_iterator
	    BranchIterator;

	 // constructors
	 FlowGraphNode(BindingsPtr bindings);
	 FlowGraphNode(BindingsPtr bindings,
	    const std::string& type);
	 FlowGraphNode(BindingsPtr bindings, NodePtr node);
	 FlowGraphNode(BindingsPtr bindings,
	    const std::string& type, NodePtr node);

	 // mutators
	 void link(FlowGraphNodePtr fgnode);
	 void link(FlowGraphNodePtr fgnode, const std::string& label);

	 // accessors
	 unsigned int get_id() const;
	 const std::string& get_type() const;
	 unsigned int get_type_number() const;
	 NodePtr get_node() const;
	 AttributePtr get_attribute() const;
	 unsigned int get_number_of_outgoing_links() const;
	 Iterator begin_links() const;
	 Iterator end_links() const;
	 FlowGraphNodePtr get_branch(const std::string& label) const;
	 BranchIterator begin_branches() const;
	 BranchIterator end_branches() const;

      private:
	 BindingsPtr bindings;
	 unsigned int id;
	 std::string type;
	 unsigned int type_number;
	 NodePtr node;
	 std::list<std::pair<std::string, FlowGraphNodePtr> > links;
	 std::map<std::string, FlowGraphNodePtr> labeled_links;
	 AttributePtr at;
   };

   unsigned int nof_node_types(BindingsPtr bindings);
   unsigned int node_type_by_name(BindingsPtr bindings,
      const std::string& type);
   unsigned int nof_labels(BindingsPtr bindings);
   unsigned int label_by_name(BindingsPtr bindings,
      const std::string& label);

} // namespace Astl

#endif
