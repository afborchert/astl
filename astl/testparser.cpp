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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <astl/exception.hpp>
#include <astl/loader.hpp>
#include <astl/location.hpp>
#include <astl/operators.hpp>
#include <astl/parser.hpp>
#include <astl/printer.hpp>
#include <astl/rule-table.hpp>
#include <astl/rules.hpp>
#include <astl/scanner.hpp>
#include <astl/token.hpp>
#include <astl/yytname.hpp>

using namespace std;
using namespace Astl;

int main(int argc, char** argv) {
   char* cmdname = *argv++; --argc;
   if (argc > 1) {
      cerr << "Usage: " << cmdname << " [filename]" << endl;
      exit(1);
   }

   try {
      Scanner* scanner;
      ifstream* fin = 0;
      if (argc > 0) {
	 char* fname = *argv++; --argc;
	 fin = new ifstream(fname);
	 string filename(fname);
	 if (!*fin) {
	    cerr << cmdname << ": unable to open " << fname <<
	       " for reading" << endl;
	    exit(1);
	 }
	 scanner = new Scanner(*fin, filename);
      } else {
	 scanner = new Scanner(cin, "stdin");
      }

      NodePtr root;
      parser p(*scanner, root);
      if (p.parse() == 0) {
	 cout << root << endl;
      }
      delete scanner;

      Loader loader;
      Rules rules(loader);
      RuleTable rt;
      rt.scan(root, Op::print_rule, rules);
      // print(cout, root, rules); 
   } catch (Exception e) {
      cerr << e.what() << endl;
   }
}
