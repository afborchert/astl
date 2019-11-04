#------------------------------------------------------------------------------
# configuration parameters
#------------------------------------------------------------------------------
PREFIX = /usr/local
ASTL_LIBPREFIX = $(PREFIX)/share/astl
ASTL_ASTL_LIBDIR = $(ASTL_LIBPREFIX)/astl
BINDIR = $(PREFIX)/bin
DESTDIR =
#------------------------------------------------------------------------------
SubDirs := astl refman

define propagate
@for dir in $(SubDirs); do $(MAKE) -C $$dir $@; done
endef

.PHONY:	all clean depend realclean install
all:		; $(propagate)
clean:		; $(propagate)
depend:		; $(propagate)
realclean:	; $(propagate)
install:	; $(propagate)
