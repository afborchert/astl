#------------------------------------------------------------------------------
# configuration parameters
#------------------------------------------------------------------------------
PREFIX = /usr/local
ASTL_LIBPREFIX = $(PREFIX)/share/astl
DESTDIR =
#------------------------------------------------------------------------------
EPSFigures :=		$(shell find . -name '*.eps')
MPFigures :=		$(shell find . -name '*.mp')
ifeq ($(EPSFigures),)
EPS2PDFFigures :=
else
EPS2PDFFigures :=	$(patsubst %.eps,%.pdf,$(EPSFigures))
endif
ifeq ($(MPFigures),)
MP2PDFFigures :=
else
MP2PDFFigures :=	$(patsubst %.mp,%.pdf,$(MPFigures))
endif
PDFFigures :=		$(EPS2PDFFigures)
TeXDeps :=		$(wildcard *.tex) refman.cls
ifeq ($(findstring keywords.tex,$(TeXDeps)),)
TeXDeps +=		keywords.tex
endif
#------------------------------------------------------------------------------
.PHONY:			pdf
pdf:			astl-refman.pdf
astl-refman.pdf:	$(TeXDeps) $(PDFFigures) $(MPFigures)
			rm -f main.out
			pdflatex main
			makeindex main
			./fixbookmarks 2>fixbookmarks.debug
			bibtex main
			pdflatex main
			pdflatex main
			mv main.pdf astl-refman.pdf
#------------------------------------------------------------------------------
keywords.tex:		../astl/keywords.cpp
			./mk_keywords.pl >$@
#------------------------------------------------------------------------------
define epstopdf
GS_OPTIONS=-dPDFSETTINGS=/printer epstopdf $^ >$@
endef
$(EPS2PDFFigures): %.pdf: %.eps;				$(epstopdf)
#------------------------------------------------------------------------------
.PHONY:			clean realclean
clean:
			rm -f *.aux *.out *.log *.ind *.lof *.tpt \
			*.idx *.lol *.ilg *.toc \
			main.bbl main.blg \
			fixbookmarks.debug main.pdf main.ps
realclean:		clean
			rm -f astl-refman.pdf
#------------------------------------------------------------------------------
.PHONY:			install
Manuals := astl-refman.pdf
ifeq ($(DESTDIR),)
LibDir := $(ASTL_LIBPREFIX)
else
BinDir := $(DESTDIR)/$(ASTL_LIBPREFIX)
endif
InstalledManuals := $(patsubst %,$(LibDir)/%,$(Manuals))
.PHONY:		install
install:	$(InstalledManuals)
$(InstalledManuals): $(LibDir)/%: %
		@mkdir -p $(LibDir)
		cp $< $@
		chmod 644 $@
#------------------------------------------------------------------------------
.PHONY:			depend
depend:
