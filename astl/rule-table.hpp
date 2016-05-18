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

#ifndef ASTL_RULE_TABLE_H
#define ASTL_RULE_TABLE_H

#include <map>
#include <string>
#include <memory>
#include <astl/operator.hpp>
#include <astl/rule.hpp>
#include <astl/types.hpp>

namespace Astl {

   class RuleTable {
      public:
	 typedef unsigned int Rank;
	 typedef std::pair<std::string, Arity> key_pair;
	 typedef std::map<Rank, RulePtr> submap_type;
	 typedef std::pair<Rank, RulePtr> value_pair;
	 typedef std::pair<const key_pair, submap_type> pair;
	 typedef std::map<key_pair, submap_type> map_type;
	 typedef submap_type::const_iterator iterator;
	 typedef submap_type::const_reverse_iterator print_iterator;
	 // constructor
	 RuleTable();

	 // mutators
	 void scan(NodePtr root, const Operator& ruleop, const Rules& rules);

	 // accessors
	 /**
	  * Returns an iterator that delivers the prefix rules
	  * that match the given operator and arity.
	  */
	 iterator find_prefix(const Operator& op, Arity arity,
	    iterator& end) const;
	 /**
	  * Returns an iterator that delivers the postfix rules
	  * that match the given operator and arity.
	  */
	 iterator find_postfix(const Operator& op, Arity arity,
	    iterator& end) const;
	 /**
	  * Returns an iterator that delivers print rules
	  * in the reverse order of appearance in the source.
	  */
	 print_iterator reversed_find(const Operator& op, Arity arity,
	    print_iterator& end) const;
	 unsigned int size() const;

      private:
	 Rank current_rank;
	 map_type table[2]; // prefix and postfix tables
	 void traverse(NodePtr node,
	    const Operator& ruleop, const Rules& rules);
	 iterator find(const Operator& op, Arity arity,
	    Rule::Type rtype, iterator& end) const;
   };
   typedef std::shared_ptr<RuleTable> RuleTablePtr;

} // namespace Astl

#endif
