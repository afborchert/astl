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

#include <iostream>
#include <memory>
#include <astl/exception.hpp>
#include <astl/flow-graph.hpp>
#include <astl/printer.hpp>
#include <astl/std-functions.hpp>
#include <astl/types.hpp>

namespace Astl {

AttributePtr builtin_assert(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("argument missing for assert function");
   }
   AttributePtr assertion = args->get_value(0);
   if (!assertion || !assertion->convert_to_bool()) {
      throw Exception("assertion failed");
   }
   return assertion;
}

AttributePtr builtin_push(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() == 0) {
      throw Exception("argument missing for push function");
   }
   AttributePtr list = args->get_value(0);
   if (!list || list->get_type() != Attribute::list) {
      throw Exception("list expected as first argument of push");
   }
   for (std::size_t i = 1; i < args->size(); ++i) {
      list->push_back(args->get_value(i));
   }
   return list;
}

AttributePtr builtin_pop(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() == 0) {
      throw Exception("argument missing for pop function");
   }
   if (args->size() > 1) {
      throw Exception("too many arguments for pop function");
   }
   AttributePtr list = args->get_value(0);
   if (!list || list->get_type() != Attribute::list) {
      throw Exception("list expected as argument of pop");
   }
   return list->pop();
}

AttributePtr builtin_len(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for len function");
   }
   AttributePtr at = args->get_value(0);
   if (at) {
      return std::make_shared<Attribute>(at->size());
   } else {
      return std::make_shared<Attribute>(0);
   }
}

AttributePtr builtin_defined(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for defined function");
   }
   AttributePtr at = args->get_value(0);
   if (at) {
      return std::make_shared<Attribute>("1");
   } else {
      return std::make_shared<Attribute>("0");
   }
}

AttributePtr builtin_isstring(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for defined function");
   }
   AttributePtr at = args->get_value(0);
   return
      std::make_shared<Attribute>(at && at->get_type() == Attribute::string);
}

AttributePtr builtin_type(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for type function");
   }
   AttributePtr at = args->get_value(0);
   if (at) {
      switch (at->get_type()) {
	 case Attribute::dictionary:
	    return std::make_shared<Attribute>("dictionary");
	 case Attribute::list:
	    return std::make_shared<Attribute>("list");
	 case Attribute::match_result:
	    return std::make_shared<Attribute>("match_result");
	 case Attribute::tree:
	    return std::make_shared<Attribute>("tree");
	 case Attribute::flow_graph_node:
	    return std::make_shared<Attribute>("flow_graph_node");
	 case Attribute::function:
	    return std::make_shared<Attribute>("function");
	 case Attribute::string:
	    return std::make_shared<Attribute>("string");
	 case Attribute::integer:
	    return std::make_shared<Attribute>("integer");
	 case Attribute::boolean:
	    return std::make_shared<Attribute>("boolean");
	 default:
	    /* not needed but it helps to suppress the warning */
	    return AttributePtr(nullptr);
      }
   } else {
      return std::make_shared<Attribute>("null");
   }
}

AttributePtr builtin_isoperator(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for isoperator function");
   }
   AttributePtr at = args->get_value(0);
   return std::make_shared<Attribute>(
      at && at->get_type() == Attribute::tree && !at->get_node()->is_leaf());
}

AttributePtr builtin_operator(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for operator function");
   }
   AttributePtr at = args->get_value(0);
   if (at && at->get_type() == Attribute::tree && !at->get_node()->is_leaf()) {
      return std::make_shared<Attribute>(at->get_node()->get_op().get_name());
   } else {
      return std::make_shared<Attribute>("");
   }
}

AttributePtr builtin_tokenliteral(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for tokenliteral function");
   }
   AttributePtr at = args->get_value(0);
   if (at && at->get_type() == Attribute::tree && at->get_node()->is_leaf()) {
      return std::make_shared<Attribute>(
	 at->get_node()->get_token().get_literal());
   } else {
      return std::make_shared<Attribute>("");
   }
}

AttributePtr builtin_tokentext(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for tokenliteral function");
   }
   AttributePtr at = args->get_value(0);
   if (at && at->get_type() == Attribute::tree && at->get_node()->is_leaf()) {
      return std::make_shared<Attribute>(
	 at->get_node()->get_token().get_text());
   } else {
      return std::make_shared<Attribute>("");
   }
}

AttributePtr builtin_location(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for location function");
   }
   AttributePtr at = args->get_value(0);
   if (at && at->get_type() == Attribute::tree) {
      NodePtr node = at->get_node();
      Location loc = node->get_location();
      std::ostringstream os;
      os << loc;
      return std::make_shared< Attribute>(os.str());
   } else {
      return std::make_shared<Attribute>("");
   }
}

AttributePtr builtin_println(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (args) {
      for (std::size_t i = 0; i < args->size(); ++i) {
	 AttributePtr at = args->get_value(i);
	 if (at) {
	    if (at->get_type() == Attribute::list) {
	       for (std::size_t j = 0; j < at->size(); ++j) {
		  AttributePtr ait = at->get_value(j);
		  std::cout << ait->convert_to_string();
	       }
	    } else {
	       std::cout << at->convert_to_string();
	    }
	 }
      }
   }
   std::cout << std::endl;
   return AttributePtr(nullptr);
}

AttributePtr builtin_integer(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for integer function");
   }
   AttributePtr at = args->get_value(0);
   if (at) {
      if (at->get_type() == Attribute::integer) {
	 return at;
      } else {
	 Location loc;
	 return std::make_shared<Attribute>(at->convert_to_integer(loc));
      }
   } else {
      return std::make_shared<Attribute>(std::make_shared<Integer>(0l));
   }
}

AttributePtr builtin_string(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for string function");
   }
   AttributePtr at = args->get_value(0);
   if (at) {
      return std::make_shared<Attribute>(at->convert_to_string());
   } else if (at->get_type() == Attribute::string) {
      return at;
   } else {
      return std::make_shared<Attribute>("");
   }
}

AttributePtr builtin_gentext(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for gentext function");
   }
   AttributePtr at = args->get_value(0);
   if (at && at->get_type() == Attribute::tree) {
      return gen_text(at->get_node(), bindings);
   } else {
      throw Exception("abstract syntax tree expected as argument to gentext");
   }
}

AttributePtr builtin_clone(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for clone function");
   }
   AttributePtr at = args->get_value(0);
   return at->clone();
}

AttributePtr builtin_copy(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 2) {
      throw Exception("wrong number of arguments for copy function");
   }
   AttributePtr target = args->get_value(0);
   AttributePtr source = args->get_value(1);
   if (!target || !source) {
      throw Exception("arguments of the copy function must be non-null");
   }
   target->copy(source);
   return target;
}

// control flow graph construction functions

AttributePtr builtin_cfg_node(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() == 0) {
      return std::make_shared<Attribute>(
	       std::make_shared<FlowGraphNode>(bindings));
   }
   switch (args->size()) {
      case 1:
	 {
	    AttributePtr arg = args->get_value(0);
	    if (!arg) {
	       throw Exception("string or abstract syntax tree expected as argument to cfg_node function");
	    }
	    if (arg->get_type() == Attribute::tree) {
	       return std::make_shared<Attribute>(
		  std::make_shared<FlowGraphNode>(
		     bindings, arg->get_node()
		  )
	       );
	    } else {
	       return std::make_shared<Attribute>(
		  std::make_shared<FlowGraphNode>(
		     bindings, arg->convert_to_string()
		  )
	       );
	    }
	 }
      case 2:
	 {
	    AttributePtr type = args->get_value(0);
	    AttributePtr node = args->get_value(1);
	    if (!node || node->get_type() != Attribute::tree) {
	       throw Exception("abstract syntax tree expected as second argument to cfg_node function");
	    }
	    if (!type) {
	       throw Exception("string expected as first argument to cfg_node function");
	    }
	    return std::make_shared<Attribute>(
	       std::make_shared<FlowGraphNode>(
		  bindings, type->convert_to_string(),
		  node->get_node()
	       )
	    );
	 }
      default:
	 throw Exception("wrong number of arguments for cfg_node function");
   }
}

AttributePtr builtin_cfg_connect(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() < 2 || args->size() > 3) {
      throw Exception("wrong number of arguments for cfg_connect function");
   }
   AttributePtr node1 = args->get_value(0);
   if (!node1 || node1->get_type() != Attribute::flow_graph_node) {
      throw Exception("flow graph node expected as argument to cfg_connect function");
   }
   AttributePtr node2 = args->get_value(1);
   if (!node2 || node2->get_type() != Attribute::flow_graph_node) {
      throw Exception("flow graph node expected as argument to cfg_connect function");
   }
   FlowGraphNodePtr fgnode1 = node1->get_fgnode();
   FlowGraphNodePtr fgnode2 = node2->get_fgnode();
   if (args->size() == 2) {
      fgnode1->link(fgnode2);
   } else {
      AttributePtr label = args->get_value(2);
      if (!label) {
	 throw Exception("string expected as third argument to cfg_connect function");
      }
      fgnode1->link(fgnode2, label->convert_to_string());
   }
   return AttributePtr(nullptr);
}

AttributePtr builtin_cfg_type(BindingsPtr bindings,
      AttributePtr args) throw(Exception) {
   if (!args || args->size() != 1) {
      throw Exception("wrong number of arguments for cfg_type function");
   }
   AttributePtr node = args->get_value(0);
   if (!node || node->get_type() != Attribute::flow_graph_node) {
      throw Exception("flow graph node expected as argument to cfg_type function");
   }
   FlowGraphNodePtr fgnode = node->get_fgnode();
   return std::make_shared<Attribute>(fgnode->get_type());
}

void insert_std_functions(BuiltinFunctions& bfs) {
   bfs.add("assert", builtin_assert);
   bfs.add("clone", builtin_clone);
   bfs.add("copy", builtin_copy);
   bfs.add("defined", builtin_defined);
   bfs.add("gentext", builtin_gentext);
   bfs.add("integer", builtin_integer);
   bfs.add("isoperator", builtin_isoperator);
   bfs.add("isstring", builtin_isstring);
   bfs.add("len", builtin_len);
   bfs.add("location", builtin_location);
   bfs.add("operator", builtin_operator);
   bfs.add("pop", builtin_pop);
   bfs.add("println", builtin_println);
   bfs.add("push", builtin_push);
   bfs.add("string", builtin_string);
   bfs.add("tokenliteral", builtin_tokenliteral);
   bfs.add("tokentext", builtin_tokentext);
   bfs.add("type", builtin_type);
   // control flow graph extensions
   bfs.add("cfg_connect", builtin_cfg_connect);
   bfs.add("cfg_node", builtin_cfg_node);
   bfs.add("cfg_type", builtin_cfg_type);
}

} // namespace Astl
