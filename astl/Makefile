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
CXXFLAGS :=	-g -O2
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
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp yystype.hpp scanner.hpp ../astl/parser.hpp \
 ../astl/yystype.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh operators.hpp yytname.hpp error.hpp parser.tab.hpp
yytname.o: yytname.cpp
operators.o: operators.cpp ../astl/operator.hpp ../astl/token.hpp
error.o: error.cpp ../astl/error.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh
scanner.o: scanner.cpp scanner.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh error.hpp keywords.hpp syntax-tree.hpp token.hpp
syntax-tree.o: syntax-tree.cpp syntax-tree.hpp ../astl/config.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/syntax-tree.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp
keywords.o: keywords.cpp scanner.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh parser.hpp keywords.hpp
rule-table.o: rule-table.cpp ../astl/syntax-tree.hpp ../astl/config.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/operators.hpp
tree-expressions.o: tree-expressions.cpp tree-expressions.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/context.hpp ../astl/operator.hpp \
 ../astl/token.hpp operators.hpp opset.hpp expression.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/tree-expressions.hpp \
 ../astl/designator.hpp
printer.o: printer.cpp bindings.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/attribute.hpp ../astl/config.hpp ../astl/syntax-tree.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/token.hpp \
 ../astl/context.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp printer.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 tree-expressions.hpp scanner.hpp ../astl/parser.hpp ../astl/yystype.hpp \
 ../astl/parser.tab.hpp ../astl/stack.hh ../astl/location.hh \
 syntax-tree.hpp operator.hpp context.hpp default-bindings.hpp \
 operators.hpp expression.hpp ../astl/tree-expressions.hpp \
 ../astl/designator.hpp rules.hpp ../astl/operator-table.hpp \
 ../astl/loader.hpp ../astl/state-machine.hpp ../astl/flow-graph.hpp \
 cloner.hpp parenthesizer.hpp
loader.o: loader.cpp loader.hpp ../astl/syntax-tree.hpp \
 ../astl/config.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operator.hpp \
 ../astl/token.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp parser.hpp ../astl/yystype.hpp \
 ../astl/parser.tab.hpp ../astl/stack.hh ../astl/location.hh scanner.hpp \
 ../astl/parser.hpp
rule.o: rule.cpp rule.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operator.hpp \
 ../astl/token.hpp ../astl/opset.hpp operator.hpp operators.hpp \
 syntax-tree.hpp ../astl/config.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/syntax-tree.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp
rules.o: rules.cpp exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh rules.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/operator-table.hpp ../astl/loader.hpp \
 ../astl/state-machine.hpp ../astl/flow-graph.hpp operator.hpp \
 operators.hpp
operator-table.o: operator-table.cpp operator-table.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/operator.hpp ../astl/token.hpp operators.hpp opset.hpp rules.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/context.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/operator-table.hpp ../astl/loader.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp
parenthesizer.o: parenthesizer.cpp ../astl/parenthesizer.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/operator.hpp ../astl/token.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/context.hpp \
 ../astl/operator-table.hpp
subtractive-rg.o: subtractive-rg.cpp ../astl/subtractive-rg.hpp \
 ../astl/prg.hpp
treeloc.o: treeloc.cpp treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/treeloc.hpp
selective-printer.o: selective-printer.cpp selective-printer.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/operator.hpp ../astl/token.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/context.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/printer.hpp
cloner.o: cloner.cpp ../astl/cloner.hpp ../astl/syntax-tree.hpp \
 ../astl/config.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operator.hpp \
 ../astl/token.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp
candidate.o: candidate.cpp candidate.hpp ../astl/rule.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/operator.hpp ../astl/token.hpp \
 ../astl/opset.hpp ../astl/tree-expressions.hpp ../astl/context.hpp \
 ../astl/bindings.hpp ../astl/attribute.hpp ../astl/config.hpp \
 ../astl/syntax-tree.hpp ../astl/token.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp cloner.hpp \
 operators.hpp expression.hpp ../astl/designator.hpp execution.hpp \
 ../astl/rule-table.hpp ../astl/candidate-set.hpp ../astl/candidate.hpp \
 ../astl/prg.hpp
execution.o: execution.cpp execution.hpp ../astl/attribute.hpp \
 ../astl/config.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/syntax-tree.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/token.hpp \
 ../astl/context.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/tree-expressions.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/candidate-set.hpp ../astl/candidate.hpp ../astl/prg.hpp \
 expression.hpp ../astl/designator.hpp operators.hpp parser.hpp \
 ../astl/yystype.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh designator.hpp
candidate-set.o: candidate-set.cpp tree-expressions.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/context.hpp ../astl/operator.hpp ../astl/token.hpp \
 default-bindings.hpp candidate-set.hpp ../astl/bindings.hpp \
 ../astl/attribute.hpp ../astl/config.hpp ../astl/syntax-tree.hpp \
 ../astl/token.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/integer.hpp ../astl/candidate.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/tree-expressions.hpp ../astl/rule-table.hpp \
 ../astl/prg.hpp operator.hpp
context.o: context.cpp context.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/operator.hpp ../astl/token.hpp bindings.hpp \
 ../astl/attribute.hpp ../astl/config.hpp ../astl/syntax-tree.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 tree-expressions.hpp
attribute.o: attribute.cpp attribute.hpp ../astl/config.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/syntax-tree.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/context.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp flow-graph.hpp
expression.o: expression.cpp expression.hpp ../astl/syntax-tree.hpp \
 ../astl/config.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operator.hpp \
 ../astl/token.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/tree-expressions.hpp \
 ../astl/designator.hpp operators.hpp arithmetic-ops.hpp \
 ../astl/operators.hpp set-ops.hpp string-ops.hpp list-ops.hpp \
 flow-graph.hpp
bindings.o: bindings.cpp bindings.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/attribute.hpp ../astl/config.hpp ../astl/syntax-tree.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/token.hpp \
 ../astl/context.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp exception.hpp
designator.o: designator.cpp designator.hpp ../astl/attribute.hpp \
 ../astl/config.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/syntax-tree.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/token.hpp \
 ../astl/context.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp flow-graph.hpp
builtin-functions.o: builtin-functions.cpp builtin-functions.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/attribute.hpp ../astl/config.hpp \
 ../astl/syntax-tree.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/integer.hpp ../astl/bindings.hpp
function.o: function.cpp function.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/builtin-functions.hpp ../astl/attribute.hpp ../astl/config.hpp \
 ../astl/syntax-tree.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/integer.hpp ../astl/bindings.hpp execution.hpp \
 ../astl/tree-expressions.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/candidate-set.hpp ../astl/candidate.hpp \
 ../astl/prg.hpp attribute.hpp
std-functions.o: std-functions.cpp types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh std-functions.hpp \
 ../astl/builtin-functions.hpp ../astl/types.hpp ../astl/attribute.hpp \
 ../astl/config.hpp ../astl/syntax-tree.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/token.hpp ../astl/context.hpp \
 ../astl/function.hpp ../astl/integer.hpp ../astl/bindings.hpp \
 flow-graph.hpp printer.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp
default-bindings.o: default-bindings.cpp default-bindings.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/attribute.hpp ../astl/config.hpp \
 ../astl/syntax-tree.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/std-functions.hpp ../astl/rules.hpp ../astl/rule-table.hpp \
 ../astl/rule.hpp ../astl/opset.hpp ../astl/operator-table.hpp \
 ../astl/loader.hpp ../astl/state-machine.hpp ../astl/flow-graph.hpp \
 ../astl/atrules-function.hpp ../astl/trrules-function.hpp \
 ../astl/prrules-function.hpp
run.o: run.cpp run.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/types.hpp ../astl/loader.hpp ../astl/syntax-tree.hpp \
 ../astl/config.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/generator.hpp \
 ../astl/printer.hpp ../astl/rule-table.hpp ../astl/rule.hpp \
 ../astl/opset.hpp ../astl/candidate-set.hpp ../astl/candidate.hpp \
 ../astl/tree-expressions.hpp ../astl/prg.hpp ../astl/execution.hpp \
 ../astl/sm-execution.hpp ../astl/rules.hpp ../astl/operator-table.hpp \
 ../astl/state-machine.hpp ../astl/flow-graph.hpp \
 ../astl/parenthesizer.hpp ../astl/subtractive-rg.hpp \
 ../astl/default-bindings.hpp
arithmetic-ops.o: arithmetic-ops.cpp arithmetic-ops.hpp \
 ../astl/operators.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/syntax-tree.hpp ../astl/token.hpp ../astl/context.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp operators.hpp integer.hpp
string-ops.o: string-ops.cpp string-ops.hpp ../astl/operators.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/config.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/syntax-tree.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 operators.hpp
flow-graph.o: flow-graph.cpp flow-graph.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 attribute.hpp ../astl/config.hpp ../astl/syntax-tree.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 treeloc.hpp exception.hpp
list-ops.o: list-ops.cpp list-ops.hpp ../astl/operators.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/config.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/syntax-tree.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 operators.hpp
state-machine.o: state-machine.cpp rules.hpp ../astl/syntax-tree.hpp \
 ../astl/config.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/operator.hpp \
 ../astl/token.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/rule-table.hpp \
 ../astl/rule.hpp ../astl/opset.hpp ../astl/operator-table.hpp \
 ../astl/loader.hpp ../astl/state-machine.hpp ../astl/flow-graph.hpp \
 attribute.hpp state-machine.hpp exception.hpp expression.hpp \
 ../astl/tree-expressions.hpp ../astl/designator.hpp execution.hpp \
 ../astl/candidate-set.hpp ../astl/candidate.hpp ../astl/prg.hpp \
 operators.hpp
sm-execution.o: sm-execution.cpp sm-execution.hpp ../astl/rules.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/treeloc.hpp ../astl/location.hpp \
 ../astl/position.hh ../astl/location.hh ../astl/position.hh \
 ../astl/operator.hpp ../astl/token.hpp ../astl/token.hpp \
 ../astl/attribute.hpp ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/bindings.hpp ../astl/integer.hpp ../astl/context.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp \
 ../astl/operator-table.hpp ../astl/loader.hpp ../astl/state-machine.hpp \
 ../astl/flow-graph.hpp execution.hpp ../astl/tree-expressions.hpp \
 ../astl/candidate-set.hpp ../astl/candidate.hpp ../astl/prg.hpp \
 flow-graph.hpp tree-expressions.hpp expression.hpp \
 ../astl/designator.hpp
opset.o: opset.cpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/rules.hpp ../astl/syntax-tree.hpp \
 ../astl/config.hpp ../astl/types.hpp ../astl/operator.hpp \
 ../astl/token.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/function.hpp ../astl/builtin-functions.hpp ../astl/bindings.hpp \
 ../astl/integer.hpp ../astl/context.hpp ../astl/rule-table.hpp \
 ../astl/rule.hpp ../astl/opset.hpp ../astl/operator-table.hpp \
 ../astl/loader.hpp ../astl/state-machine.hpp ../astl/flow-graph.hpp \
 ../astl/operators.hpp
atrules-function.o: atrules-function.cpp atrules-function.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/attribute.hpp ../astl/config.hpp ../astl/syntax-tree.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/token.hpp \
 ../astl/context.hpp ../astl/integer.hpp ../astl/bindings.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp attribute.hpp \
 execution.hpp ../astl/tree-expressions.hpp ../astl/candidate-set.hpp \
 ../astl/candidate.hpp ../astl/prg.hpp
trrules-function.o: trrules-function.cpp trrules-function.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/attribute.hpp ../astl/config.hpp ../astl/syntax-tree.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/token.hpp \
 ../astl/context.hpp ../astl/integer.hpp ../astl/bindings.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp attribute.hpp \
 execution.hpp ../astl/tree-expressions.hpp ../astl/candidate-set.hpp \
 ../astl/candidate.hpp ../astl/prg.hpp
set-ops.o: set-ops.cpp set-ops.hpp ../astl/operators.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/attribute.hpp \
 ../astl/config.hpp ../astl/types.hpp ../astl/exception.hpp \
 ../astl/treeloc.hpp ../astl/location.hpp ../astl/position.hh \
 ../astl/location.hh ../astl/position.hh ../astl/syntax-tree.hpp \
 ../astl/token.hpp ../astl/context.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 operators.hpp integer.hpp
prrules-function.o: prrules-function.cpp prrules-function.hpp \
 ../astl/types.hpp ../astl/exception.hpp ../astl/treeloc.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/function.hpp ../astl/builtin-functions.hpp \
 ../astl/attribute.hpp ../astl/config.hpp ../astl/syntax-tree.hpp \
 ../astl/operator.hpp ../astl/token.hpp ../astl/token.hpp \
 ../astl/context.hpp ../astl/integer.hpp ../astl/bindings.hpp \
 ../astl/rule-table.hpp ../astl/rule.hpp ../astl/opset.hpp attribute.hpp \
 printer.hpp
testlex.o: testlex.cpp scanner.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh parser.hpp yytname.hpp ../astl/candidate.hpp \
 ../astl/rule.hpp ../astl/opset.hpp ../astl/tree-expressions.hpp
testparser.o: testparser.cpp scanner.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh parser.hpp yytname.hpp operators.hpp rule-table.hpp \
 ../astl/rule.hpp ../astl/opset.hpp printer.hpp ../astl/rule-table.hpp \
 rules.hpp ../astl/operator-table.hpp ../astl/loader.hpp \
 ../astl/state-machine.hpp ../astl/flow-graph.hpp
astl-run.o: astl-run.cpp scanner.hpp ../astl/parser.hpp \
 ../astl/location.hpp ../astl/position.hh ../astl/location.hh \
 ../astl/position.hh ../astl/treeloc.hpp ../astl/yystype.hpp \
 ../astl/syntax-tree.hpp ../astl/config.hpp ../astl/types.hpp \
 ../astl/exception.hpp ../astl/operator.hpp ../astl/token.hpp \
 ../astl/token.hpp ../astl/attribute.hpp ../astl/function.hpp \
 ../astl/builtin-functions.hpp ../astl/bindings.hpp ../astl/integer.hpp \
 ../astl/context.hpp ../astl/parser.tab.hpp ../astl/stack.hh \
 ../astl/location.hh parser.hpp yytname.hpp location.hpp ../astl/run.hpp \
 ../astl/loader.hpp ../astl/generator.hpp operators.hpp
