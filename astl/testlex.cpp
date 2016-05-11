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

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <astl/scanner.hpp>
#include <astl/parser.hpp>
#include <astl/yytname.hpp>
#include <astl/location.hpp>
#include <astl/token.hpp>
#include <astl/exception.hpp>
#include <astl/bindings.hpp>
#include <astl/candidate.hpp>

using namespace std;
using namespace Astl;

int main(int argc, char** argv) {
   char* cmdname = *argv++; --argc;
   if (argc > 1) {
      cerr << "Usage: " << cmdname << " [filename]" << endl;
      exit(1);
   }

   cout << "sizeof Bindings = " << sizeof(Bindings) << endl;
   cout << "sizeof Candidate = " << sizeof(Candidate) << endl;
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
      location loc;
      semantic_type yylval;
      int token;
      while ((token = scanner->get_token(yylval, loc)) != 0) {
	 cout << yytname[token - 255];
	 if (yylval) {
	    cout << " \"" << yylval->get_token().get_text() << '"';
	    yylval = NodePtr((Node*)0);
	 }
	 cout << endl;
      }
      delete scanner;
      if (fin) delete fin;
   } catch (Exception e) {
      cerr << e.what() << endl;
   }
}
