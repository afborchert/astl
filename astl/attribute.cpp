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
#include <cstdlib>
#include <memory>
#include <astl/attribute.hpp>
#include <astl/flow-graph.hpp>
#include <astl/utf8.hpp>

using namespace std;

namespace Astl {

Attribute::Attribute() : type(dictionary) {
}

Attribute::Attribute(NodePtr node_param) : type(tree), node(node_param) {
}

Attribute::Attribute(FunctionPtr func_param) :
      type(function), func(func_param) {
}

Attribute::Attribute(const std::string& string_val) :
      type(string), svalue(string_val) {
}

Attribute::Attribute(IntegerPtr int_val) :
      type(integer), ivalue(int_val) {
}

Attribute::Attribute(int intval) :
      type(integer), ivalue(std::make_shared<Integer>(intval)) {
}

Attribute::Attribute(unsigned int intval) :
      type(integer), ivalue(std::make_shared<Integer>(intval)) {
}

Attribute::Attribute(long intval) :
      type(integer), ivalue(std::make_shared<Integer>(intval)) {
}

Attribute::Attribute(unsigned long intval) :
      type(integer), ivalue(std::make_shared<Integer>(intval)) {
}

Attribute::Attribute(bool bool_val) :
      type(boolean), bval(bool_val) {
}

Attribute::Attribute(const std::vector<std::string>& subtokens_param) :
      type(match_result), subtokens(subtokens_param) {
   assert(subtokens.size() > 0);
}

Attribute::Attribute(FlowGraphNodePtr fgnode_param) :
      type(flow_graph_node), fgnode(fgnode_param) {
   assert(fgnode_param);
}

Attribute::Attribute(Type type_param) :
      type(type_param) {
}

void Attribute::update(const std::string& key, AttributePtr val) {
   assert(type == dictionary);
   dict[key] = val;
}

void Attribute::delete_key(const std::string& key) {
   assert(type == dictionary);
   std::map<std::string, AttributePtr>::iterator it = dict.find(key);
   if (it != dict.end()) {
      dict.erase(it);
   }
}

void Attribute::update(const std::string& string_val) {
   assert(type == string);
   svalue = string_val;
}

void Attribute::update(IntegerPtr int_val) {
   assert(type == integer);
   ivalue = int_val;
}

void Attribute::update(bool bool_val) {
   assert(type == boolean);
   bval = bool_val;
}

void Attribute::push_back(AttributePtr val) {
   assert(type == list);
   values.push_back(val);
}

AttributePtr Attribute::pop() {
   assert(type == list);
   AttributePtr rval;
   if (values.size() > 0) {
      rval = values.back();
      values.pop_back();
   }
   return rval;
}

void Attribute::update(unsigned int index, AttributePtr val) {
   assert(type == list);
   assert(index < size());
   values[index] = val;
}

Attribute::Type Attribute::get_type() const {
   return type;
}

AttributePtr Attribute::get_value(const std::string& key) const {
   assert(type == dictionary);
   std::map<std::string, AttributePtr>::const_iterator it = dict.find(key);
   if (it == dict.end()) {
      return AttributePtr(nullptr);
   } else {
      return it->second;
   }
}

bool Attribute::is_defined(const std::string& key) const {
   assert(type == dictionary);
   std::map<std::string, AttributePtr>::const_iterator it = dict.find(key);
   return it != dict.end();
}

AttributePtr Attribute::get_keys() const {
   assert(type == dictionary);
   AttributePtr keys = std::make_shared<Attribute>(list);
   for (std::map<std::string, AttributePtr>::const_iterator kit = dict.begin();
	 kit != dict.end(); ++kit) {
      keys->push_back(std::make_shared<Attribute>(kit->first));
   }
   return keys;
}

Attribute::DictionaryIterator Attribute::get_pairs_begin() const {
   assert(type == dictionary);
   return dict.begin();
}

Attribute::DictionaryIterator Attribute::get_pairs_end() const {
   assert(type == dictionary);
   return dict.end();
}

Attribute::DictionaryInserter Attribute::get_inserter() {
   assert(type == dictionary);
   return std::inserter(dict, dict.end());
}

AttributePtr Attribute::get_value(unsigned int index) const {
   switch (type) {
      case list:
	 assert(index < size());
	 return values[index];

      case match_result:
	 assert(index < size());
	 return std::make_shared<Attribute>(subtokens[index + 1]);

      default:
	 assert(false); std::abort();
   }
}

std::size_t Attribute::size() const {
   switch (type) {
      case list:
	 return values.size();

      case dictionary:
	 return dict.size();

      case string:
	 {
	    size_t len = 0;
	    for (auto ch: codepoint_range(svalue)) {
	       len += 1 + ch * 0; /* count & suppress warning */
	    }
	    return len;
	 }

      case integer:
	 return ivalue->to_string().size();

      case boolean:
      case function:
	 return 1;

      case tree:
	 return node->size();

      case flow_graph_node:
	 return fgnode->get_number_of_outgoing_links();

      case match_result:
	 return subtokens.size() - 1;

      default:
	 assert(false); std::abort();
   }
}

NodePtr Attribute::get_node() const throw(Exception) {
   if (type == tree) {
      return node;
   } else if (type == flow_graph_node) {
      return fgnode->get_node();
   } else {
      return std::make_shared<Node>(Location(), Token(convert_to_string()));
   }
}

FlowGraphNodePtr Attribute::get_fgnode() const {
   assert(type == flow_graph_node);
   return fgnode;
}

FunctionPtr Attribute::get_func() const {
   assert(type == function);
   return func;
}

const std::string& Attribute::get_string() const {
   switch (type) {
      case string:
	 return svalue;
      case match_result:
	 return subtokens[0];
      default:
	 assert(false); std::abort();
   }
}

IntegerPtr Attribute::get_integer() const {
   assert(type == integer);
   return ivalue;
}

std::string Attribute::convert_to_string() const throw(Exception) {
   switch (type) {
      case string:
	 return svalue;
      case integer:
	 return ivalue->to_string();
      case boolean:
	 return bval? "1": "0";
      case match_result:
	 return subtokens[0];
      case tree:
	 if (node->is_leaf()) {
	    return node->get_token().get_text();
	 } else {
	    return node->get_op().get_name();
	 }
      case flow_graph_node:
	 return fgnode->get_type();
      case function:
	 {
	    AttributePtr rval = func->eval(AttributePtr(nullptr));
	    if (rval) {
	       return rval->convert_to_string();
	    } else {
	       return "";
	    }
	 }
      case list:
      case dictionary:
	 /* in case of lists and dictionaries we take, as in Perl,
	    the actual size if it is put into a string context
	 */
	 {
	    std::ostringstream os; os << size(); return os.str();
	 }
      default:
	 assert(false); std::abort();
   }
}

IntegerPtr Attribute::convert_to_integer(const Location& loc) const
      throw(Exception) {
   if (type == integer) {
      return ivalue;
   } else if (type == boolean) {
      return std::make_shared<Integer>((unsigned long) bval);
   }
   std::string s = convert_to_string();
   if (s == "") {
      s = "0";
   }
   return std::make_shared<Integer>(s, loc);
}

bool Attribute::is_scalar() const {
   return type == string || type == integer || type == boolean;
}

bool Attribute::is_integer() const {
   return type == integer || type == boolean;
}

bool Attribute::convert_to_bool() const throw(Exception) {
   switch (type) {
      case boolean:
	 return bval;
      case integer:
	 return ivalue->to_bool();
      case tree:
      case match_result:
      case flow_graph_node:
	 return true;
      default:
	 std::string string_value = convert_to_string();
	 return string_value.size() > 0 && string_value != "0";
   }
}

AttributePtr Attribute::convert_to_list() throw(Exception) {
   if (type == Attribute::list) {
      return shared_from_this();
   }
   if (type == Attribute::tree) {
      NodePtr n = get_node();
      if (!n->is_leaf()) {
	 // generate a list with all the subnodes of n
	 AttributePtr l = std::make_shared<Attribute>(Attribute::list);
	 for (unsigned int i = 0; i < n->size(); ++i) {
	    l->push_back(std::make_shared<Attribute>(n->get_operand(i)));
	 }
	 return l;
      }
   } else if (type == Attribute::dictionary) {
      return get_keys();
   } else if (type == Attribute::match_result) {
      // generate a list with all the matched subtokens
      AttributePtr l = std::make_shared<Attribute>(Attribute::list);
      for (unsigned int i = 0; i < size(); ++i) {
	 l->push_back(get_value(i));
      }
      return l;
   } else if (type == Attribute::flow_graph_node) {
      // generate a list with all nodes which can be reached from this node
      AttributePtr l = std::make_shared<Attribute>(Attribute::list);
      FlowGraphNodePtr fgn = get_fgnode();
      for (FlowGraphNode::Iterator it = fgn->begin_links();
	    it != fgn->end_links(); ++it) {
	 l->push_back(std::make_shared<Attribute>(it->second));
      }
      return l;
   }
   std::string scalar = convert_to_string();
   AttributePtr l = std::make_shared<Attribute>(Attribute::list);
   l->push_back(std::make_shared<Attribute>(scalar));
   return l;
}

AttributePtr Attribute::convert_to_dict() throw(Exception) {
   if (type == Attribute::dictionary) {
      return shared_from_this();
   }
   // convert it into a list ...
   AttributePtr l = convert_to_list();
   AttributePtr set = std::make_shared<Attribute>(Attribute::dictionary);
   // and use all the list members as keys
   for (unsigned int index = 0; index < l->size(); ++index) {
      AttributePtr member = l->get_value(index);
      if (member) {
	 // convert member to string to use it as a key
	 std::string key = member->convert_to_string();
	 set->update(key, std::make_shared<Attribute>(true));
      }
   }
   return set;
}

bool Attribute::equal_to(AttributePtr other) const throw(Exception) {
   if (this == other.get()) {
      return true;
   }
   if (is_scalar() && other->is_scalar()) {
      return convert_to_string() == other->convert_to_string();
   } else {
      if (type != other->type) {
	 return false;
      }
      switch (type) {
	 case tree:
	    return node == other->node;
	 case function:
	    return func == other->func;
	 case flow_graph_node:
	    return fgnode == other->fgnode;
	 default:
	    return false;
      }
   }
}

AttributePtr Attribute::clone() const {
   AttributePtr cat = std::make_shared<Attribute>(type);
   switch (type) {
      case dictionary:
	 cat->dict = dict;
	 break;

      case list:
	 cat->values = values;
	 break;

      case match_result:
	 cat->subtokens = subtokens;
	 break;

      case tree:
	 cat->node = node;
	 break;

      case flow_graph_node:
	 cat->fgnode = fgnode;
	 break;

      case function:
	 cat->func = func;
	 break;

      case string:
	 cat->svalue = svalue;
	 break;

      case integer:
	 cat->ivalue = ivalue;
	 break;

      case boolean:
	 cat->bval = bval;
	 break;

      default:
	 assert(false); std::abort();
   }
   return cat;
}

void Attribute::copy(AttributePtr other) throw(Exception) {
   if (!other) {
      Exception("source is null");
   }
   if (type != other->type) {
      Exception("type mismatch");
   }
   switch (type) {
      case dictionary:
	 dict = other->dict;
	 break;

      case list:
	 values = other->values;
	 break;

      case match_result:
	 subtokens = other->subtokens;
	 break;

      case tree:
	 node = other->node;
	 break;

      case flow_graph_node:
	 fgnode = other->fgnode;
	 break;

      case function:
	 func = other->func;
	 break;

      case string:
	 svalue = other->svalue;
	 break;

      case integer:
	 ivalue = other->ivalue;
	 break;

      case boolean:
	 bval = other->bval;
	 break;

      default:
	 assert(false); std::abort();
   }
}

std::ostream& operator<<(std::ostream& out, AttributePtr at) {
   if (at) {
      switch (at->type) {
	 case Attribute::dictionary:
	    {
	       out << "{";
	       bool first = true;
	       for (std::map<std::string, AttributePtr>::iterator it =
		     at->dict.begin(); it != at->dict.end(); ++it) {
		  if (first) {
		     first = false;
		  } else {
		     out << ", ";
		  }
		  out << it->first << " -> " << it->second;
	       }
	       out << "}";
	       break;
	    }

	 case Attribute::list:
	    out << "[";
	    for (unsigned int i = 0; i < at->size(); ++i) {
	       if (i > 0) out << ", ";
	       out << at->values[i];
	    }
	    out << "]";
	    break;
	 
	 case Attribute::match_result:
	    out << "{matched -> \"" << at->subtokens[0] << '"';
	    if (at->size() > 0) {
	       out << ", captures -> [";
	       for (unsigned int i = 0; i < at->size(); ++i) {
		  if (i > 0) out << ", ";
		  out << '"' << at->subtokens[i+1] << '"';
	       }
	       out << "]";
	    }
	    out << "}";
	    break;

	 case Attribute::tree:
	    out << "subtree";
	    // out << at->node; // can lead to an endless recursion
	    break;

	 case Attribute::flow_graph_node:
	    out << "flow graph node";
	    break;

	 case Attribute::function:
	    out << "function";
	    break;

	 case Attribute::string:
	    out << '"' << at->svalue << '"';
	    break;

	 case Attribute::integer:
	    out << at->ivalue->to_string();
	    break;

	 case Attribute::boolean:
	    out << (at->bval? "true": "false");
	    break;
      }
   } else {
      out << "null";
   }
   return out;
}

} // namespace Astl
