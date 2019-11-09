#------------------------------------------------------------------------------
# configuration parameters
#------------------------------------------------------------------------------
PREFIX = /usr/local
ASTL_LIBPREFIX = $(PREFIX)/share/astl
ASTL_ASTL_LIBDIR = $(ASTL_LIBPREFIX)/astl
BINDIR = $(PREFIX)/bin
DESTDIR =
#------------------------------------------------------------------------------
BisonSources :=	$(wildcard *.ypp)
GeneratedCPPSourcesFromBison := $(patsubst %.ypp,%.tab.cpp,$(BisonSources))
GeneratedCPPSources := $(GeneratedCPPSourcesFromBison) \
   $(patsubst %.ypp,%.tab.hpp,$(BisonSources)) \
   yytname.cpp operators.cpp
GeneratedHPPSources := $(patsubst %.ypp,%.tab.hpp,$(BisonSources)) \
   operators.hpp $(wildcard *.hh)
CPPSources := $(GeneratedCPPSources) \
   error.cpp scanner.cpp syntax-tree.cpp keywords.cpp \
   rule-table.cpp tree-expressions.cpp printer.cpp \
   loader.cpp rule.cpp rules.cpp operator-table.cpp \
   parenthesizer.cpp treeloc.cpp cloner.cpp \
   candidate.cpp execution.cpp \
   candidate-set.cpp context.cpp attribute.cpp expression.cpp \
   bindings.cpp designator.cpp builtin-functions.cpp \
   function.cpp std-functions.cpp default-bindings.cpp run.cpp \
   arithmetic-ops.cpp string-ops.cpp flow-graph.cpp list-ops.cpp \
   state-machine.cpp sm-execution.cpp opset.cpp atrules-function.cpp \
   trrules-function.cpp set-ops.cpp prrules-function.cpp
Objects := $(patsubst %.cpp,%.o,$(CPPSources))
testlex_objs := testlex.o
testparser_objs := testparser.o
testregex_objs := testregex.o
run_objs := astl-run.o
astl_astl_objs := astl-astl.o
astl_astl_free_objs := astl-astl-free.o
MainSources := testlex.cpp testparser.cpp testregex.cpp \
   astl-run.cpp astl-astl.cpp astl-astl-free.cpp
MainObjects := $(patsubst %.cpp,%.o,$(MainSources))
Lib := libastl.a
Utils := ../bison-scripts

DEFS +=		-DASTL_ASTL_LIBDIR='"$(ASTL_ASTL_LIBDIR)"'
CXX =		g++
CXXFLAGS =	-Wall -g -O2
LDFLAGS =
CPPFLAGS +=	-I.. -std=gnu++14 $(DEFS)
LDLIBS += -lgmp -lpcre2-8
BISON = bison

.PHONY:		all clean realclean depend
all:		$(Lib) testlex testparser testregex astl-astl astl-astl-free
clean:		; rm -f $(Objects) testlex.o testparser.o \
			$(GeneratedCPPSources) $(MainObjects)
realclean:	clean
		rm -f $(Lib) $(GeneratedCPPSources) $(GeneratedHPPSources) \
		   $(wildcard *.output) testlex testparser testregex \
		   astl-astl astl-astl-free

$(Lib):		$(Objects)
		ar r $@ $(Objects)

yytname.cpp:	parser.tab.cpp
		perl $(Utils)/extract_yytname.pl Astl \
		   parser.tab.cpp >$@

operators.hpp operators.cpp:	parser.tab.cpp operators.txt \
			$(Utils)/gen_operators.pl
		perl $(Utils)/gen_operators.pl -op operators.txt Astl \
		   astl/operator.hpp \
		   parser.tab.cpp operators.hpp operators.cpp

$(GeneratedCPPSourcesFromBison): %.tab.cpp: %.ypp
		$(BISON) -d $<

parser.tab.o:	operators.hpp

testlex:	$(testlex_objs) $(Lib)
		$(CXX) $(LDFLAGS) -o $@ $(testlex_objs) $(Lib) $(LDLIBS)
testparser:	$(testparser_objs) $(Lib)
		$(CXX) $(LDFLAGS) -o $@ $(testparser_objs) $(Lib) $(LDLIBS)
testregex:	$(testregex_objs) $(Lib)
		$(CXX) $(LDFLAGS) -o $@ $(testregex_objs) $(Lib) $(LDLIBS)
astl-run:	$(run_objs) $(Lib)
		$(CXX) $(LDFLAGS) -o $@ $(run_objs) $(Lib) $(LDLIBS)
astl-astl:	$(astl_astl_objs) $(Lib)
		$(CXX) $(LDFLAGS) -o $@ $(astl_astl_objs) $(Lib) $(LDLIBS)
astl-astl-free:	$(astl_astl_free_objs) $(Lib)
		$(CXX) $(LDFLAGS) -o $@ $(astl_astl_free_objs) $(Lib) $(LDLIBS)
depend:		$(CPPSources)
		perl ../gcc-makedepend/gcc-makedepend.pl \
		   -gcc $(CXX) $(CPPFLAGS) $(CPPSources) $(MainSources)

#------------------------------------------------------------------------------
# installation
#------------------------------------------------------------------------------
Binaries := astl-astl astl-astl-free
ifeq ($(DESTDIR),)
BinDir := $(BINDIR)
else
BinDir := $(DESTDIR)/$(BINDIR)
endif
InstalledBinaries := $(patsubst %,$(BinDir)/%,$(Binaries))
.PHONY:		install
install:	$(InstalledBinaries)
$(InstalledBinaries): $(BinDir)/%: %
		@mkdir -p $(BinDir)
		cp $< $@
		chmod 755 $@

#------------------------------------------------------------------------------
# for gcc-makedepend
#------------------------------------------------------------------------------
# DO NOT DELETE
parser.tab.o: parser.tab.cpp ../astl/error.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/parser.hpp \
 ../astl/yystype.hpp ../astl/syntax-tree.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh ../astl/operators.hpp ../astl/scanner.hpp \
 ../astl/yytname.hpp parser.tab.hpp
parser.tab.o: parser.tab.hpp stack.hh location.hh position.hh
yytname.o: yytname.cpp
operators.o: operators.cpp ../astl/operator.hpp ../astl/token.hpp
error.o: error.cpp ../astl/error.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/parser.hpp \
 ../astl/yystype.hpp ../astl/syntax-tree.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh
scanner.o: scanner.cpp ../astl/error.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/parser.hpp \
 ../astl/yystype.hpp ../astl/syntax-tree.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh ../astl/keywords.hpp ../astl/regex.hpp \
 ../astl/scanner.hpp ../astl/utf8.hpp
syntax-tree.o: syntax-tree.cpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp
keywords.o: keywords.cpp ../astl/keywords.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/attribute.hpp ../astl/exception.hpp \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh ../astl/scanner.hpp
rule-table.o: rule-table.cpp ../astl/operators.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/opset.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/types.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/arity.hpp \
 ../astl/syntax-tree.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/bindings.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/context.hpp
tree-expressions.o: tree-expressions.cpp ../astl/bindings.hpp \
 ../astl/attribute.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/arity.hpp \
 ../astl/builtin-functions.hpp ../astl/types.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/expression.hpp \
 ../astl/designator.hpp ../astl/tree-expressions.hpp \
 ../astl/operators.hpp ../astl/opset.hpp ../astl/regex.hpp
printer.o: printer.cpp ../astl/bindings.hpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/builtin-functions.hpp \
 ../astl/types.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/cloner.hpp ../astl/default-bindings.hpp \
 ../astl/expression.hpp ../astl/designator.hpp \
 ../astl/tree-expressions.hpp ../astl/operators.hpp \
 ../astl/parenthesizer.hpp ../astl/operator-table.hpp ../astl/printer.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/rules.hpp ../astl/loader.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp ../astl/scanner.hpp ../astl/parser.hpp \
 ../astl/yystype.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh ../astl/utf8.hpp
loader.o: loader.cpp ../astl/loader.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/syntax-tree.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/arity.hpp \
 ../astl/bindings.hpp ../astl/types.hpp ../astl/builtin-functions.hpp \
 ../astl/integer.hpp ../astl/stream.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/parser.hpp \
 ../astl/yystype.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh ../astl/scanner.hpp
rule.o: rule.cpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/operators.hpp ../astl/rule.hpp ../astl/arity.hpp \
 ../astl/opset.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/types.hpp ../astl/syntax-tree.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/bindings.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/context.hpp
rules.o: rules.cpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/operator.hpp ../astl/token.hpp \
 ../astl/operators.hpp ../astl/rules.hpp ../astl/loader.hpp \
 ../astl/syntax-tree.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/arity.hpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/context.hpp ../astl/operator-table.hpp ../astl/rule-table.hpp \
 ../astl/rule.hpp ../astl/opset.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp
operator-table.o: operator-table.cpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operator-table.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/types.hpp \
 ../astl/operators.hpp ../astl/opset.hpp ../astl/rules.hpp \
 ../astl/loader.hpp ../astl/syntax-tree.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/context.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/state-machine.hpp ../astl/flow-graph.hpp
parenthesizer.o: parenthesizer.cpp ../astl/operator-table.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/operator.hpp ../astl/token.hpp ../astl/types.hpp \
 ../astl/parenthesizer.hpp ../astl/syntax-tree.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/context.hpp
treeloc.o: treeloc.cpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh
cloner.o: cloner.cpp ../astl/attribute.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/function.hpp \
 ../astl/arity.hpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/cloner.hpp
candidate.o: candidate.cpp ../astl/candidate.hpp ../astl/bindings.hpp \
 ../astl/attribute.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/arity.hpp \
 ../astl/builtin-functions.hpp ../astl/types.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/tree-expressions.hpp ../astl/cloner.hpp \
 ../astl/execution.hpp ../astl/candidate-set.hpp ../astl/prg.hpp \
 ../astl/rule-table.hpp ../astl/expression.hpp ../astl/designator.hpp \
 ../astl/operators.hpp
execution.o: execution.cpp ../astl/designator.hpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/execution.hpp \
 ../astl/candidate-set.hpp ../astl/candidate.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/tree-expressions.hpp ../astl/prg.hpp \
 ../astl/rule-table.hpp ../astl/expression.hpp ../astl/operators.hpp \
 ../astl/parser.hpp ../astl/yystype.hpp ../astl/parser.tab.hpp \
 ../astl/stack.hh ../astl/location.hh
candidate-set.o: candidate-set.cpp ../astl/candidate-set.hpp \
 ../astl/bindings.hpp ../astl/attribute.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/function.hpp \
 ../astl/arity.hpp ../astl/builtin-functions.hpp ../astl/types.hpp \
 ../astl/integer.hpp ../astl/stream.hpp ../astl/syntax-tree.hpp \
 ../astl/context.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/candidate.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/tree-expressions.hpp ../astl/prg.hpp ../astl/rule-table.hpp \
 ../astl/default-bindings.hpp
context.o: context.cpp ../astl/bindings.hpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/builtin-functions.hpp \
 ../astl/types.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/tree-expressions.hpp
attribute.o: attribute.cpp ../astl/attribute.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/function.hpp \
 ../astl/arity.hpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/flow-graph.hpp ../astl/utf8.hpp
expression.o: expression.cpp ../astl/arithmetic-ops.hpp \
 ../astl/attribute.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/arity.hpp \
 ../astl/bindings.hpp ../astl/types.hpp ../astl/builtin-functions.hpp \
 ../astl/integer.hpp ../astl/stream.hpp ../astl/syntax-tree.hpp \
 ../astl/context.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/operators.hpp ../astl/expression.hpp ../astl/designator.hpp \
 ../astl/tree-expressions.hpp ../astl/flow-graph.hpp ../astl/list-ops.hpp \
 ../astl/regex.hpp ../astl/set-ops.hpp ../astl/string-ops.hpp
bindings.o: bindings.cpp ../astl/bindings.hpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/builtin-functions.hpp \
 ../astl/types.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp
designator.o: designator.cpp ../astl/designator.hpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/flow-graph.hpp
builtin-functions.o: builtin-functions.cpp ../astl/builtin-functions.hpp \
 ../astl/attribute.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/arity.hpp \
 ../astl/bindings.hpp ../astl/types.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp
function.o: function.cpp ../astl/attribute.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/function.hpp \
 ../astl/arity.hpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/execution.hpp ../astl/candidate-set.hpp \
 ../astl/candidate.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/tree-expressions.hpp ../astl/prg.hpp ../astl/rule-table.hpp
std-functions.o: std-functions.cpp ../astl/cloner.hpp \
 ../astl/syntax-tree.hpp ../astl/attribute.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/function.hpp \
 ../astl/arity.hpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/context.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/flow-graph.hpp ../astl/parser.hpp ../astl/yystype.hpp \
 ../astl/parser.tab.hpp ../astl/stack.hh ../astl/location.hh \
 ../astl/printer.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/scanner.hpp ../astl/std-functions.hpp \
 ../astl/utf8.hpp
default-bindings.o: default-bindings.cpp ../astl/atrules-function.hpp \
 ../astl/function.hpp ../astl/arity.hpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/integer.hpp ../astl/stream.hpp ../astl/types.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/bindings.hpp ../astl/builtin-functions.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/default-bindings.hpp ../astl/prrules-function.hpp \
 ../astl/rules.hpp ../astl/loader.hpp ../astl/operator-table.hpp \
 ../astl/state-machine.hpp ../astl/flow-graph.hpp \
 ../astl/std-functions.hpp ../astl/trrules-function.hpp
run.o: run.cpp ../astl/attribute.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/function.hpp \
 ../astl/arity.hpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/candidate-set.hpp ../astl/candidate.hpp \
 ../astl/rule.hpp ../astl/opset.hpp ../astl/tree-expressions.hpp \
 ../astl/prg.hpp ../astl/rule-table.hpp ../astl/default-bindings.hpp \
 ../astl/execution.hpp ../astl/loader.hpp ../astl/mt19937.hpp \
 ../astl/parenthesizer.hpp ../astl/operator-table.hpp ../astl/printer.hpp \
 ../astl/rules.hpp ../astl/state-machine.hpp ../astl/flow-graph.hpp \
 ../astl/run.hpp ../astl/generator.hpp ../astl/sm-execution.hpp
arithmetic-ops.o: arithmetic-ops.cpp ../astl/arithmetic-ops.hpp \
 ../astl/attribute.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/arity.hpp \
 ../astl/bindings.hpp ../astl/types.hpp ../astl/builtin-functions.hpp \
 ../astl/integer.hpp ../astl/stream.hpp ../astl/syntax-tree.hpp \
 ../astl/context.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/operators.hpp
string-ops.o: string-ops.cpp ../astl/attribute.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/function.hpp \
 ../astl/arity.hpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/operators.hpp ../astl/string-ops.hpp
flow-graph.o: flow-graph.cpp ../astl/attribute.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/function.hpp \
 ../astl/arity.hpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/flow-graph.hpp
list-ops.o: list-ops.cpp ../astl/list-ops.hpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/operators.hpp
state-machine.o: state-machine.cpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/execution.hpp \
 ../astl/candidate-set.hpp ../astl/candidate.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/tree-expressions.hpp ../astl/prg.hpp \
 ../astl/rule-table.hpp ../astl/expression.hpp ../astl/designator.hpp \
 ../astl/operators.hpp ../astl/rules.hpp ../astl/loader.hpp \
 ../astl/operator-table.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp
sm-execution.o: sm-execution.cpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/execution.hpp \
 ../astl/candidate-set.hpp ../astl/candidate.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/tree-expressions.hpp ../astl/prg.hpp \
 ../astl/rule-table.hpp ../astl/expression.hpp ../astl/designator.hpp \
 ../astl/flow-graph.hpp ../astl/sm-execution.hpp ../astl/rules.hpp \
 ../astl/loader.hpp ../astl/operator-table.hpp ../astl/state-machine.hpp
opset.o: opset.cpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/operators.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/rules.hpp ../astl/loader.hpp \
 ../astl/syntax-tree.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/arity.hpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/context.hpp ../astl/operator-table.hpp ../astl/rule-table.hpp \
 ../astl/rule.hpp ../astl/opset.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp
atrules-function.o: atrules-function.cpp ../astl/atrules-function.hpp \
 ../astl/function.hpp ../astl/arity.hpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/integer.hpp ../astl/stream.hpp ../astl/types.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/bindings.hpp ../astl/builtin-functions.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/execution.hpp ../astl/candidate-set.hpp ../astl/candidate.hpp \
 ../astl/tree-expressions.hpp ../astl/prg.hpp
trrules-function.o: trrules-function.cpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/execution.hpp \
 ../astl/candidate-set.hpp ../astl/candidate.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/tree-expressions.hpp ../astl/prg.hpp \
 ../astl/rule-table.hpp ../astl/trrules-function.hpp
set-ops.o: set-ops.cpp ../astl/integer.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operators.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/set-ops.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/arity.hpp \
 ../astl/bindings.hpp ../astl/types.hpp ../astl/builtin-functions.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp
prrules-function.o: prrules-function.cpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/types.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/stream.hpp ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/printer.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/prrules-function.hpp
testlex.o: testlex.cpp ../astl/bindings.hpp ../astl/attribute.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/function.hpp ../astl/arity.hpp ../astl/builtin-functions.hpp \
 ../astl/types.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/candidate.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/tree-expressions.hpp ../astl/parser.hpp \
 ../astl/yystype.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh ../astl/scanner.hpp ../astl/yytname.hpp
testparser.o: testparser.cpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/loader.hpp ../astl/syntax-tree.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/arity.hpp \
 ../astl/bindings.hpp ../astl/types.hpp ../astl/builtin-functions.hpp \
 ../astl/integer.hpp ../astl/stream.hpp ../astl/context.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/operators.hpp \
 ../astl/parser.hpp ../astl/yystype.hpp ../astl/parser.tab.hpp \
 ../astl/stack.hh ../astl/location.hh ../astl/printer.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/rules.hpp ../astl/operator-table.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp ../astl/scanner.hpp ../astl/yytname.hpp
testregex.o: testregex.cpp ../astl/attribute.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/function.hpp \
 ../astl/arity.hpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/syntax-tree.hpp ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/regex.hpp
astl-run.o: astl-run.cpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/treeloc.hpp \
 ../astl/operators.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/parser.hpp ../astl/yystype.hpp ../astl/syntax-tree.hpp \
 ../astl/attribute.hpp ../astl/exception.hpp ../astl/function.hpp \
 ../astl/arity.hpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/context.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh ../astl/run.hpp ../astl/generator.hpp \
 ../astl/loader.hpp ../astl/scanner.hpp ../astl/yytname.hpp
astl-astl.o: astl-astl.cpp ../astl/generator.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/types.hpp \
 ../astl/loader.hpp ../astl/syntax-tree.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/arity.hpp ../astl/bindings.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/stream.hpp \
 ../astl/context.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/operators.hpp ../astl/parser.hpp ../astl/yystype.hpp \
 ../astl/parser.tab.hpp ../astl/stack.hh ../astl/location.hh \
 ../astl/run.hpp ../astl/scanner.hpp ../astl/yytname.hpp
astl-astl-free.o: astl-astl-free.cpp ../astl/builtin-parse.hpp \
 ../astl/attribute.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/arity.hpp \
 ../astl/bindings.hpp ../astl/types.hpp ../astl/builtin-functions.hpp \
 ../astl/integer.hpp ../astl/stream.hpp ../astl/syntax-tree.hpp \
 ../astl/context.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/execution.hpp ../astl/candidate-set.hpp ../astl/candidate.hpp \
 ../astl/rule.hpp ../astl/opset.hpp ../astl/tree-expressions.hpp \
 ../astl/prg.hpp ../astl/rule-table.hpp ../astl/rules.hpp \
 ../astl/loader.hpp ../astl/operator-table.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp ../astl/sm-execution.hpp ../astl/generator.hpp \
 ../astl/operators.hpp ../astl/parser.hpp ../astl/yystype.hpp \
 ../astl/parser.tab.hpp ../astl/stack.hh ../astl/location.hh \
 ../astl/run.hpp ../astl/scanner.hpp ../astl/yytname.hpp
