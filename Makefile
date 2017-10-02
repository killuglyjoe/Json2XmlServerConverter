MAKEFILE      = Makefile

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -pipe -g -Wall -W -D_REENTRANT -fPIC $(DEFINES)
CXXFLAGS      = -pipe -g -std=gnu++0x -Wall -W -D_REENTRANT -fPIC $(DEFINES)
DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = cp -f
COPY_DIR      = cp -f -R
INSTALL_FILE  = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
INSTALL_DIR   = cp -f -R
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
TAR           = tar -cf
COMPRESS      = gzip -9f
DISTNAME      = nweb1.0.0
LINK          = g++
LFLAGS        = 
LIBS          = $(SUBLIBS) -lpthread 
AR            = ar cqs
RANLIB        = 
SED           = sed
STRIP         = strip

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = nweb.cpp
OBJECTS       = nweb.o

DESTDIR       = 
TARGET        = nweb


first: all
####### Build rules

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: 

all: Makefile $(TARGET)

dist: distdir FORCE
	(cd `dirname $(DISTDIR)` && $(TAR) $(DISTNAME).tar $(DISTNAME) && $(COMPRESS) $(DISTNAME).tar) && $(MOVE) `dirname $(DISTDIR)`/$(DISTNAME).tar.gz . && $(DEL_FILE) -r $(DISTDIR)

distdir: FORCE
	@test -d $(DISTDIR) || mkdir -p $(DISTDIR)
	$(COPY_FILE) --parents $(DIST) $(DISTDIR)/


clean: compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


distclean: clean 
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


####### Sub-libraries

check: first

benchmark: first

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: 

####### Compile

nweb.o: nweb.cpp convutils/json2xml.hpp \
		convutils/rapidxml/rapidxml.hpp \
		convutils/rapidxml/rapidxml_print.hpp \
		convutils/rapidjson/document.h \
		convutils/rapidjson/reader.h \
		convutils/rapidjson/allocators.h \
		convutils/rapidjson/rapidjson.h \
		convutils/rapidjson/msinttypes/stdint.h \
		convutils/rapidjson/msinttypes/inttypes.h \
		convutils/rapidjson/stream.h \
		convutils/rapidjson/encodings.h \
		convutils/rapidjson/encodedstream.h \
		convutils/rapidjson/memorystream.h \
		convutils/rapidjson/internal/meta.h \
		convutils/rapidjson/internal/stack.h \
		convutils/rapidjson/internal/swap.h \
		convutils/rapidjson/internal/strtod.h \
		convutils/rapidjson/internal/ieee754.h \
		convutils/rapidjson/internal/biginteger.h \
		convutils/rapidjson/internal/diyfp.h \
		convutils/rapidjson/internal/pow10.h \
		convutils/rapidjson/error/error.h \
		convutils/rapidjson/internal/strfunc.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o nweb.o nweb.cpp

####### Install

install:  FORCE

uninstall:  FORCE

FORCE:

