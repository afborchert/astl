System := $(shell uname -s)
BisonSources :=	$(wildcard *.ypp)
GeneratedCPPSourcesFromBison := $(patsubst %.ypp,%.tab.cpp,$(BisonSources))
GeneratedCPPSources := $(GeneratedCPPSourcesFromBison) \
   yytname.cpp operators.cpp
GeneratedHPPSources := $(patsubst %.ypp,%.tab.hpp,$(BisonSources)) \
   operators.hpp $(wildcard *.hh)
CPPSources := $(GeneratedCPPSources) \
   error.cpp scanner.cpp syntax-tree.cpp keywords.cpp \
   rule-table.cpp tree-expressions.cpp printer.cpp \
   loader.cpp rule.cpp rules.cpp operator-table.cpp \
   parenthesizer.cpp subtractive-rg.cpp \
   treeloc.cpp selective-printer.cpp cloner.cpp \
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
run_objs := astl-run.o
MainSources := testlex.cpp testparser.cpp astl-run.cpp
MainObjects := $(patsubst %.cpp,%.o,$(MainSources))
Lib := libastl.a
Utils := ../bison-scripts

CXX :=		g++
CXXFLAGS :=	-Wall -g -O2
LDFLAGS :=
CPPFLAGS +=	-I.. -std=gnu++11
LDLIBS := -lgmp
BISON := bison

.PHONY:		all clean realclean depend
all:		$(Lib) testlex testparser astl-astl
clean:		; rm -f $(Objects) testlex.o testparser.o \
			$(GeneratedCPPSources) $(MainObjects)
realclean:	clean
		rm -f $(Lib) $(GeneratedCPPSources) $(GeneratedHPPSources) \
		   $(wildcard *.output) testlex testparser astl-astl

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
astl-astl:	$(run_objs) $(Lib)
		$(CXX) $(LDFLAGS) -o $@ $(run_objs) $(Lib) $(LDLIBS)
depend:		$(CPPSources)
		perl ../gcc-makedepend/gcc-makedepend.pl $(CPPFLAGS) $(CPPSources) $(MainSources)
murks:
		echo $(CPPFLAGS) $(CPPSources) $(MainSources)
# DO NOT DELETE
parser.tab.o: parser.tab.cpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/treeloc.hpp \
 ../astl/syntax-tree.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/yystype.hpp \
 ../astl/scanner.hpp ../astl/parser.hpp ../astl/parser.tab.hpp \
 ../astl/stack.hh ../astl/location.hh ../astl/operators.hpp \
 ../astl/yytname.hpp ../astl/error.hpp parser.tab.hpp
yytname.o: yytname.cpp
operators.o: operators.cpp ../astl/operator.hpp ../astl/token.hpp
error.o: error.cpp ../astl/error.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/parser.tab.hpp \
 ../astl/stack.hh ../astl/location.hh
scanner.o: scanner.cpp ../astl/scanner.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/parser.tab.hpp \
 ../astl/stack.hh ../astl/location.hh ../astl/error.hpp \
 ../astl/keywords.hpp
syntax-tree.o: syntax-tree.cpp ../astl/syntax-tree.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp
keywords.o: keywords.cpp ../astl/scanner.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/parser.tab.hpp \
 ../astl/stack.hh ../astl/location.hh ../astl/keywords.hpp
rule-table.o: rule-table.cpp ../astl/syntax-tree.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/rule-table.hpp \
 ../astl/rule.hpp ../astl/opset.hpp ../astl/operators.hpp
tree-expressions.o: tree-expressions.cpp ../astl/tree-expressions.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/operators.hpp ../astl/opset.hpp \
 ../astl/expression.hpp ../astl/syntax-tree.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/designator.hpp
printer.o: printer.cpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/attribute.hpp ../astl/syntax-tree.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/printer.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/tree-expressions.hpp ../astl/scanner.hpp ../astl/parser.hpp \
 ../astl/yystype.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh ../astl/default-bindings.hpp ../astl/operators.hpp \
 ../astl/expression.hpp ../astl/designator.hpp ../astl/rules.hpp \
 ../astl/operator-table.hpp ../astl/loader.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp ../astl/cloner.hpp ../astl/parenthesizer.hpp
loader.o: loader.cpp ../astl/loader.hpp ../astl/syntax-tree.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/operator.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/context.hpp \
 ../astl/parser.hpp ../astl/yystype.hpp ../astl/parser.tab.hpp \
 ../astl/stack.hh ../astl/location.hh ../astl/scanner.hpp
rule.o: rule.cpp ../astl/rule.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operator.hpp \
 ../astl/token.hpp ../astl/opset.hpp ../astl/operators.hpp \
 ../astl/syntax-tree.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp
rules.o: rules.cpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/rules.hpp ../astl/syntax-tree.hpp \
 ../astl/types.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/context.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/operator-table.hpp ../astl/loader.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp ../astl/operators.hpp
operator-table.o: operator-table.cpp ../astl/operator-table.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/operator.hpp ../astl/token.hpp \
 ../astl/operators.hpp ../astl/opset.hpp ../astl/rules.hpp \
 ../astl/syntax-tree.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/loader.hpp ../astl/state-machine.hpp ../astl/flow-graph.hpp
parenthesizer.o: parenthesizer.cpp ../astl/parenthesizer.hpp \
 ../astl/syntax-tree.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operator.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/operator-table.hpp
subtractive-rg.o: subtractive-rg.cpp ../astl/subtractive-rg.hpp \
 ../astl/prg.hpp
treeloc.o: treeloc.cpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh
selective-printer.o: selective-printer.cpp ../astl/selective-printer.hpp \
 ../astl/syntax-tree.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operator.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/printer.hpp
cloner.o: cloner.cpp ../astl/cloner.hpp ../astl/syntax-tree.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/operator.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/context.hpp
candidate.o: candidate.cpp ../astl/candidate.hpp ../astl/rule.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/operator.hpp ../astl/token.hpp \
 ../astl/opset.hpp ../astl/tree-expressions.hpp ../astl/context.hpp \
 ../astl/bindings.hpp ../astl/attribute.hpp ../astl/syntax-tree.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/cloner.hpp ../astl/operators.hpp ../astl/expression.hpp \
 ../astl/designator.hpp ../astl/execution.hpp ../astl/rule-table.hpp \
 ../astl/candidate-set.hpp ../astl/prg.hpp
execution.o: execution.cpp ../astl/execution.hpp ../astl/attribute.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/syntax-tree.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/tree-expressions.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/candidate-set.hpp ../astl/candidate.hpp \
 ../astl/prg.hpp ../astl/expression.hpp ../astl/designator.hpp \
 ../astl/operators.hpp ../astl/parser.hpp ../astl/yystype.hpp \
 ../astl/parser.tab.hpp ../astl/stack.hh ../astl/location.hh
candidate-set.o: candidate-set.cpp ../astl/tree-expressions.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/default-bindings.hpp ../astl/candidate-set.hpp \
 ../astl/bindings.hpp ../astl/attribute.hpp ../astl/syntax-tree.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/candidate.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/rule-table.hpp ../astl/prg.hpp
context.o: context.cpp ../astl/context.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/operator.hpp ../astl/token.hpp ../astl/bindings.hpp \
 ../astl/attribute.hpp ../astl/syntax-tree.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp \
 ../astl/tree-expressions.hpp
attribute.o: attribute.cpp ../astl/attribute.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/syntax-tree.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/context.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/flow-graph.hpp
expression.o: expression.cpp ../astl/expression.hpp \
 ../astl/syntax-tree.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operator.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/tree-expressions.hpp ../astl/designator.hpp \
 ../astl/operators.hpp ../astl/arithmetic-ops.hpp ../astl/set-ops.hpp \
 ../astl/string-ops.hpp ../astl/list-ops.hpp ../astl/flow-graph.hpp
bindings.o: bindings.cpp ../astl/bindings.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/attribute.hpp ../astl/syntax-tree.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp
designator.o: designator.cpp ../astl/designator.hpp ../astl/attribute.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/syntax-tree.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/flow-graph.hpp
builtin-functions.o: builtin-functions.cpp ../astl/builtin-functions.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/attribute.hpp ../astl/syntax-tree.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/context.hpp \
 ../astl/function.hpp ../astl/integer.hpp ../astl/bindings.hpp
function.o: function.cpp ../astl/function.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/builtin-functions.hpp ../astl/attribute.hpp \
 ../astl/syntax-tree.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/context.hpp ../astl/integer.hpp ../astl/bindings.hpp \
 ../astl/execution.hpp ../astl/tree-expressions.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/candidate-set.hpp ../astl/candidate.hpp ../astl/prg.hpp
std-functions.o: std-functions.cpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/std-functions.hpp ../astl/builtin-functions.hpp \
 ../astl/attribute.hpp ../astl/syntax-tree.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/integer.hpp ../astl/bindings.hpp ../astl/flow-graph.hpp \
 ../astl/printer.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp
default-bindings.o: default-bindings.cpp ../astl/default-bindings.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/attribute.hpp ../astl/syntax-tree.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/context.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/std-functions.hpp ../astl/rules.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/operator-table.hpp ../astl/loader.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp ../astl/atrules-function.hpp \
 ../astl/trrules-function.hpp ../astl/prrules-function.hpp
run.o: run.cpp ../astl/run.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/types.hpp ../astl/loader.hpp ../astl/syntax-tree.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/context.hpp \
 ../astl/generator.hpp ../astl/printer.hpp ../astl/rule-table.hpp \
 ../astl/rule.hpp ../astl/opset.hpp ../astl/candidate-set.hpp \
 ../astl/candidate.hpp ../astl/tree-expressions.hpp ../astl/prg.hpp \
 ../astl/execution.hpp ../astl/sm-execution.hpp ../astl/rules.hpp \
 ../astl/operator-table.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp ../astl/parenthesizer.hpp \
 ../astl/subtractive-rg.hpp ../astl/default-bindings.hpp
arithmetic-ops.o: arithmetic-ops.cpp ../astl/arithmetic-ops.hpp \
 ../astl/operators.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/syntax-tree.hpp \
 ../astl/context.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp
string-ops.o: string-ops.cpp ../astl/string-ops.hpp ../astl/operators.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp
flow-graph.o: flow-graph.cpp ../astl/flow-graph.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/attribute.hpp ../astl/syntax-tree.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp
list-ops.o: list-ops.cpp ../astl/list-ops.hpp ../astl/operators.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp
state-machine.o: state-machine.cpp ../astl/rules.hpp \
 ../astl/syntax-tree.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operator.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/operator-table.hpp ../astl/loader.hpp \
 ../astl/state-machine.hpp ../astl/flow-graph.hpp ../astl/expression.hpp \
 ../astl/tree-expressions.hpp ../astl/designator.hpp \
 ../astl/execution.hpp ../astl/candidate-set.hpp ../astl/candidate.hpp \
 ../astl/prg.hpp ../astl/operators.hpp
sm-execution.o: sm-execution.cpp ../astl/sm-execution.hpp \
 ../astl/rules.hpp ../astl/syntax-tree.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/rule-table.hpp \
 ../astl/rule.hpp ../astl/opset.hpp ../astl/operator-table.hpp \
 ../astl/loader.hpp ../astl/state-machine.hpp ../astl/flow-graph.hpp \
 ../astl/execution.hpp ../astl/tree-expressions.hpp \
 ../astl/candidate-set.hpp ../astl/candidate.hpp ../astl/prg.hpp \
 ../astl/expression.hpp ../astl/designator.hpp
opset.o: opset.cpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/rules.hpp ../astl/syntax-tree.hpp \
 ../astl/types.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/context.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/operator-table.hpp ../astl/loader.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp ../astl/operators.hpp
atrules-function.o: atrules-function.cpp ../astl/atrules-function.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/attribute.hpp ../astl/syntax-tree.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/integer.hpp \
 ../astl/bindings.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/execution.hpp ../astl/tree-expressions.hpp \
 ../astl/candidate-set.hpp ../astl/candidate.hpp ../astl/prg.hpp
trrules-function.o: trrules-function.cpp ../astl/trrules-function.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/attribute.hpp ../astl/syntax-tree.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/integer.hpp \
 ../astl/bindings.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/execution.hpp ../astl/tree-expressions.hpp \
 ../astl/candidate-set.hpp ../astl/candidate.hpp ../astl/prg.hpp
set-ops.o: set-ops.cpp ../astl/set-ops.hpp ../astl/operators.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/syntax-tree.hpp ../astl/context.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp
prrules-function.o: prrules-function.cpp ../astl/prrules-function.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/attribute.hpp ../astl/syntax-tree.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/integer.hpp \
 ../astl/bindings.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/printer.hpp
testlex.o: testlex.cpp ../astl/scanner.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/parser.tab.hpp \
 ../astl/stack.hh ../astl/location.hh ../astl/yytname.hpp \
 ../astl/candidate.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/tree-expressions.hpp
testparser.o: testparser.cpp ../astl/scanner.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/parser.tab.hpp \
 ../astl/stack.hh ../astl/location.hh ../astl/yytname.hpp \
 ../astl/operators.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/printer.hpp ../astl/rules.hpp \
 ../astl/operator-table.hpp ../astl/loader.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp
astl-run.o: astl-run.cpp ../astl/scanner.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/parser.tab.hpp \
 ../astl/stack.hh ../astl/location.hh ../astl/yytname.hpp ../astl/run.hpp \
 ../astl/loader.hpp ../astl/generator.hpp ../astl/operators.hpp
