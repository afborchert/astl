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

#ifndef ASTL_ATTRIBUTE_H
#define ASTL_ATTRIBUTE_H

#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <astl/exception.hpp>
#include <astl/function.hpp>
#include <astl/integer.hpp>
#include <astl/syntax-tree.hpp>
#include <astl/types.hpp>

namespace Astl {

   class Attribute: public std::enable_shared_from_this<Attribute> {
      public:
	 typedef enum {dictionary, list, match_result, tree,
	    flow_graph_node, function, string, integer, boolean} Type;
	 typedef std::vector<std::string> SubtokenVector;
	 typedef std::map<std::string, AttributePtr> Dictionary;
	 typedef std::pair<std::string, AttributePtr> DictionaryPair;
	 typedef Dictionary::const_iterator DictionaryIterator;
	 typedef std::insert_iterator<Dictionary> DictionaryInserter;
	 typedef std::vector<AttributePtr> Vector;

	 // constructors
	 Attribute(); // create an empty dictionary
	 Attribute(Type type_param);
	 Attribute(NodePtr node_param);
	 Attribute(const SubtokenVector& subtokens_param); // match_result
	 Attribute(FlowGraphNodePtr fgnode_param); // fgnode
	 Attribute(FunctionPtr func_param); // function
	 Attribute(const std::string& string_val); // string attribute
	 Attribute(IntegerPtr int_val); // integer attribute
	 Attribute(int intval); // integer attribute
	 Attribute(unsigned int intval); // integer attribute
	 Attribute(long intval); // integer attribute
	 Attribute(unsigned long intval); // integer attribute
	 Attribute(bool bool_val); // boolean attribute
	 template <typename T> Attribute(T val) : type(string) {
	    std::ostringstream os; os << val; svalue = os.str();
	 }

	 // mutators

	 /** Update an entry of a dictionary */
	 void update(const std::string& key, AttributePtr val);
	 void delete_key(const std::string& key);

	 /** Update a string value */
	 void update(const std::string& string_val);

	 /** Update an integer value */
	 void update(IntegerPtr int_val);

	 /** Update a boolean value */
	 void update(bool bool_val);

	 /** Update a list */
	 void push_back(AttributePtr val);
	 AttributePtr pop();
	 void update(std::size_t index, AttributePtr val);

	 // accessors
	 Type get_type() const;
	 std::size_t size() const;

	 // for dictionaries:
	 AttributePtr get_value(const std::string& key) const;
	 bool is_defined(const std::string& key) const;
	 AttributePtr get_keys() const; // return keys as list
	 DictionaryIterator get_pairs_begin() const;
	 DictionaryIterator get_pairs_end() const;
	 DictionaryInserter get_inserter();

	 // for lists and match results:
	 AttributePtr get_value(std::size_t index) const;

	 NodePtr get_node() const throw(Exception);

	 // for flow graph nodes:
	 FlowGraphNodePtr get_fgnode() const;

	 // for functions
	 FunctionPtr get_func() const;

	 // for strings and match_results
	 const std::string& get_string() const;

	 // for integers
	 IntegerPtr get_integer() const;

	 // type inquiries
	 bool is_scalar() const;
	 bool is_integer() const;

	 // general operators
	 bool equal_to(AttributePtr other) const throw(Exception);

	 // conversions
	 std::string convert_to_string() const throw(Exception);
	 IntegerPtr convert_to_integer(const Location& loc) const
	    throw(Exception);
	 bool convert_to_bool() const throw(Exception);
	 AttributePtr convert_to_list() throw(Exception);
	 AttributePtr convert_to_dict() throw(Exception);

	 // create a clone of this attribute
	 // (this is one level deep, i.e. not working recursively)
	 AttributePtr clone() const;

	 // copy contents from another attribute of the very same type
	 void copy(AttributePtr other) throw(Exception);

      private:
	 friend std::ostream& operator<<(std::ostream& out, AttributePtr at);
	 Type type;

	 /* if type == dictionary: */
	 Dictionary dict;

	 /* if type == list: */
	 Vector values;

	 /* if type == match_result: */
	 SubtokenVector subtokens;

	 /* if type == tree */
	 NodePtr node;

	 /* if type == fgnode: */
	 FlowGraphNodePtr fgnode;

	 /* if type == function: */
	 FunctionPtr func;

	 /* if type == string: */
	 std::string svalue;

	 /* if type == integer: */
	 IntegerPtr ivalue;

	 /* if type == bool: */
	 bool bval;
   };

   std::ostream& operator<<(std::ostream& out, AttributePtr at);

   /* useful for various set operators on dictionaries */
   inline bool dict_less_than(Attribute::DictionaryPair p1,
	 Attribute::DictionaryPair p2) {
      return p1.first < p2.first;
   }

} // namespace Astl

#endif
