/*
   Copyright (C) 2009-2019 Andreas Franz Borchert
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <astl/attribute.hpp>
#include <astl/candidate-set.hpp>
#include <astl/default-bindings.hpp>
#include <astl/execution.hpp>
#include <astl/loader.hpp>
#include <astl/parenthesizer.hpp>
#include <astl/printer.hpp>
#include <astl/rules.hpp>
#include <astl/run.hpp>
#include <astl/sm-execution.hpp>
#include <astl/mt19937.hpp>
#include <astl/syntax-tree.hpp>

namespace Astl {

class MyConsumer: public Consumer {
   public:
      MyConsumer(const char* p, Rules& r, std::size_t count,
	    const Operator& parentheses, std::ostream& out,
	    BindingsPtr bindings) :
	    counter(0), width(0), pattern(p),
	    rules(r), bindings(bindings),
	    lparen(parentheses), out(out) {
	 assert(count > 0);
	 --count;
	 width = 1;
	 while (count >= 10) {
	    ++width;
	    count /= 10;
	 }
      };

      virtual bool consume(NodePtr root, CandidatePtr candidate) {
	 // set "location" and "rulename" attribute
	 AttributePtr rootAt = root->get_attribute();
	 rootAt->update("location",
	    std::make_shared<Attribute>(candidate->get_location()));
	 rootAt->update("rulename",
	    std::make_shared<Attribute>(candidate->get_rule()->get_name()));
	 // parenthesize it when possible
	 if (rules.operator_rules_defined()) {
	    parenthesize(root, rules.get_operator_table(), lparen);
	 }
	 // print it
	 if (pattern) {
	    std::ostringstream os;
	    for (const char* cp = pattern; *cp; ++cp) {
	       if (*cp == '%') {
		  os << std::setfill('0') << std::setw(width) << counter;
	       } else {
		  os << *cp;
	       }
	    }
	    std::ofstream out(os.str(), std::ios_base::trunc);
	    if (out) {
	       print(out, root, rules.get_print_rule_table(), bindings);
	       out << std::endl;
	    } else {
	       std::ostringstream osmsg;
	       osmsg << "unable to open " << os.str() << " for writing";
	       throw Exception(osmsg.str());
	    }
	 } else {
	    print(out, root, rules.get_print_rule_table(), bindings);
	    out << std::endl;
	 }
	 ++counter;
	 return true;
      }
   private:
      std::size_t counter;
      int width;
      const char* pattern;
      Rules& rules;
      BindingsPtr bindings;
      const Operator& lparen;
      std::ostream& out;
};

void run(NodePtr root,
      Loader& loader,
      const char* rules_filename, const char* pattern,
      std::size_t count,
      const Operator& parentheses,
      std::ostream& out,
      BindingsPtr extra_bindings,
      int argc, char** argv) {
   Rules rules(loader.load(rules_filename), loader);

   // setup default bindings
   BindingsPtr bindings = create_default_bindings(root, &rules, extra_bindings);

   // execute global attribution rules, if defined
   if (rules.attribution_rules_defined()) {
      execute(root, rules.get_attribution_rule_table(), bindings);
   }
   // execute state machines, if present
   execute_state_machines(rules, bindings);

   NodePtr main = rules.get_function("main");
   if (main) {
      // collect remaining arguments for main()
      AttributePtr arg_list(std::make_shared<Attribute>(Attribute::list));
      while (argc > 0) {
	 AttributePtr arg(std::make_shared<Attribute>(*argv++)); --argc;
	 arg_list->push_back(arg);
      }
      AttributePtr args;
      if (main->size() == 2) {
	 // pass list directly, if main has no explicit parameter declaration
	 args = arg_list;
      } else {
	 // we pass just one argument with a list of arguments
	 args = std::make_shared<Attribute>(Attribute::list);
	 args->push_back(arg_list);
      }
      // invoke main
      BindingsPtr local_bindings(bindings);
      FunctionPtr f;
      if (main->size() == 2) {
	 f = std::make_shared<RegularFunction>(main->get_operand(1),
	    local_bindings);
      } else {
	 f = std::make_shared<RegularFunction>(main->get_operand(2),
	    local_bindings, main->get_operand(1));
      }
      try {
	 f->eval(args);
      } catch (Exception& e) {
	 throw Exception(main->get_location(), e.what());
      }
   }

   if (rules.print_rules_defined() &&
	    rules.transformation_rules_defined()) {
      std::random_device random;
      PseudoRandomGeneratorPtr prg = std::make_shared<mt19937>(random());
      const RuleTable& rt(rules.get_transformation_rule_table());
      CandidateSet candidates(root, rt, bindings);
      if (candidates.size() == 0) {
	 std::ostringstream os;
	 os << "no matching transformation rule found in " << rules_filename;
	 throw Exception(os.str());
      }
      if (count < candidates.size()) {
	 count = candidates.size();
      }
      ConsumerPtr consumer = std::make_shared<MyConsumer>(pattern, rules, count,
	 parentheses, out, bindings);
      candidates.set_prg(prg);
      candidates.set_consumer(consumer);
      if (count == candidates.size()) {
	 candidates.gen_mutations();
      } else {
	 candidates.gen_mutations(count);
      }
   }
}

void run(NodePtr root,
      const char* rules_filename, const char* pattern,
      std::size_t count,
      const Operator& parentheses,
      std::ostream& out,
      BindingsPtr extra_bindings) {
   Loader loader;
   run(root, loader, rules_filename, pattern, count, parentheses, out,
      extra_bindings, 0, 0);
}

void run(NodePtr root,
      const char* rules_filename, const char* pattern,
      std::size_t count,
      const Operator& parentheses,
      std::ostream& out) {
   Loader loader;
   run(root, loader, rules_filename, pattern, count, parentheses, out,
      nullptr, 0, 0);
}

void usage(char* cmdname) {
   std::ostringstream os;
   os << "Usage: " << cmdname <<
      " script [source options...] source [options...]";
   throw Exception(os.str());
}

void run(int& argc, char**& argv, SyntaxTreeGenerator& astgen,
      Loader& loader, const Operator& parentheses,
      BindingsPtr extra_bindings) {
   /* fetch cmdname */
   char* cmdname = *argv++; --argc;
   /* fetch name of our script */
   if (argc == 0) usage(cmdname);
   char* script_name = *argv++; --argc;
   /* generate AST */
   if (argc == 0) usage(cmdname);
   NodePtr root = astgen.gen(argc, argv);
   if (!root) {
      throw Exception("no abstract syntax tree has been generated");
   }
   run(root, loader, script_name, /* pattern= */ nullptr, /* count = */ 0,
      parentheses, std::cout, extra_bindings, argc, argv);
}

void run(int& argc, char**& argv, SyntaxTreeGenerator& astgen,
      Loader& loader, const Operator& parentheses) {
   run(argc, argv, astgen, loader, parentheses, nullptr);
}

} // namespace Astl
