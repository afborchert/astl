#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <astl/scanner.hpp>
#include <astl/parser.hpp>
#include <astl/yytname.hpp>
#include <astl/location.hpp>
#include <astl/run.hpp>
#include <astl/operators.hpp>

using namespace std;
using namespace Astl;

static char* cmdname;

void usage() {
   cerr << "Usage: " << cmdname << " source rules [pattern [count]]" << endl;
   exit(1);
}

int main(int argc, char** argv) {
   cmdname = *argv++; --argc;
   /* fetch source argument */
   if (argc == 0) usage();
   char* source_name = *argv++; --argc;
   /* fetch rules argument */
   if (argc == 0) usage();
   char* rules_name = *argv++; --argc;
   /* fetch pattern argument, if given */
   unsigned int count = 0; char* pattern = 0;
   if (argc > 0) {
      pattern = *argv++; --argc;
   }
   /* fetch count argument, if given */
   if (argc > 0) {
      char* count_string = *argv++; --argc;
      count = atoi(count_string);
      if (count < 1) usage();
   }
   /* all arguments should have been processed by now */
   if (argc > 0) usage();

   try {
      ifstream source(source_name);
      if (!source) {
	 cerr << cmdname << ": unable to open " << source_name <<
	    " for reading" << endl;
	 exit(1);
      }
      Scanner scanner(source, source_name);
      NodePtr root;
      parser p(scanner, root);
      if (p.parse() != 0) exit(1);

      run(root, rules_name, pattern, count, Op::LPAREN, cout);
   } catch (Exception e) {
      cout << endl;
      cerr << e.what() << endl;
   }
}
