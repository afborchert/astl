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

#include <astl/selective-printer.hpp>

namespace Astl {

static void recursive_selective_print(std::ostream& out,
      const std::string& begin_text, const std::string& join_text,
      unsigned int& counter,
      const NodePtr root, Operator& op,
      const RuleTable& rules) throw(Exception) {
   if (root->is_leaf()) return;
   Operator rop = root->get_op();
   if (rop == op) {
      if (counter == 0) {
	 out << begin_text;
      } else {
	 out << join_text;
      }
      print(out, root, rules);
      ++counter;
   } else {
      unsigned int arity = root->size();
      for (unsigned int i = 0; i < arity; ++i) {
	 recursive_selective_print(out, begin_text, join_text,
	    counter, root->get_operand(i), op, rules);
      }
   }
}

bool selective_print(std::ostream& out,
      const std::string& begin_text,
      const std::string& join_text,
      const std::string& end_text,
      const NodePtr root, Operator& op,
      const RuleTable& rules) throw(Exception) {
   unsigned int counter = 0;
   recursive_selective_print(out, begin_text, join_text,
      counter, root, op, rules);
   if (counter > 0) out << end_text;
   return !!out;
}

} // namespace Astl
