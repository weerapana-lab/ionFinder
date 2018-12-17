
#
# C++ Compiler
CXX := g++
#
# latexmk
TEX := $(shell command -v latexmk 2> /dev/null)
#
# Flags
#
#   Compiler
CXXFLAGS += -c -g -Wall -std=c++11
#
#   Linker
LDFLAGS += -g
#
#   Library
LIBFLAGS :=
#
#   Include
INCLUDEFLAGS :=
#
#
# Program name
MS2_ANNOTATOR_EXE := ms2_annotator
CIT_FINDER_EXE := citFinder
#
#
# Directories
#
#   Headers
HEADERDIR := include
#
#   Sources
SRCDIR = src
MS2_ANNOTATOR_SRCDIR = $(SRCDIR)/ms2_annotator
CIT_FINDER_SRCDIR = $(SRCDIR)/citFinder
#
#   Objects
OBJDIR := obj
MS2_ANNOTATOR_OBJDIR := $(OBJDIR)/ms2_annotator
CIT_FINDER_OBJDIR := $(OBJDIR)/citFinder
#
#   Binary
BINDIR := bin
#
#   Build scripts
SCRIPTS := scripts
#
#   Tex dir
TEX_DIR := doc/tex
#
#   Install dirrectory
INSTALL_DIR := /usr/local/bin/
#
#
################################################################################

#HEADERS := $(wildcard $(HEADERDIR)/*.h)
SRCS := $(wildcard $(SRCDIR)/*.cpp)
MS2_ANNOTATOR_SRCS := $(wildcard $(MS2_ANNOTATOR_SRCDIR)/*.cpp)
CIT_FINDER_SRCS := $(wildcard $(CIT_FINDER_SRCDIR)/*.cpp)
#ALL_SRCS = $(SRCS) $(MS2_ANNOTATOR_SRCS)

OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS:.cpp=.o))
MS2_ANNOTATOR_OBJS += $(OBJS) $(subst src/,obj/,$(MS2_ANNOTATOR_SRCS:.cpp=.o))
CIT_FINDER_OBJS += $(OBJS) $(subst src/,obj/,$(CIT_FINDER_SRCS:.cpp=.o))
ALL_OBJS = $(OBJS) $(MS2_ANNOTATOR_OBJS) $(CIT_FINDER_OBJS)

CXXFLAGS += $(INCLUDEFLAGS) -I$(HEADERDIR)
LDFLAGS += $(LIBFLAGS)

.PHONY: all clean distclean install uninstall

#TARGETS = $(HEADERDIR)/$(GIT_VERSION) $(BINDIR)/$(EXE) $(BINDIR)/DTsetup helpFile.pdf DTarray_pro-Userguide.pdf
TARGETS += $(BINDIR)/$(MS2_ANNOTATOR_EXE) $(BINDIR)/$(CIT_FINDER_EXE) helpFile.pdf

all: $(TARGETS)

#DTarray_pro-Userguide.pdf : $(TEX_DIR)/DTarray_pro-Userguide.tex
#ifndef TEX
# 	$(warning "No latexmk in $(PATH), skipping build of DTarray_pro-Userguide.pdf")
# else
# 	cd $(TEX_DIR) && latexmk -pdf DTarray_pro-Userguide.tex
# 	cp $(TEX_DIR)/DTarray_pro-Userguide.pdf .
# endif

$(BINDIR)/$(MS2_ANNOTATOR_EXE): $(MS2_ANNOTATOR_OBJS)
	mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $? -o $@

$(BINDIR)/$(CIT_FINDER_EXE): $(CIT_FINDER_OBJS)
	mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $? -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR) $(MS2_ANNOTATOR_OBJDIR) $(CIT_FINDER_OBJDIR)
	$(CXX) $(CXXFLAGS) $< -o $@

helpFile.pdf : db/helpFile.man
	bash $(SCRIPTS)/updateMan.sh

clean:
	rm -f $(ALL_OBJS) $(BINDIR)/$(MS2_ANNOTATOR_EXE) $(BINDIR)/$(CIT_FINDER_EXE) 
	rm -f helpFile.pdf
	#cd $(TEX_DIR) && rm -f ./*.aux ./*.dvi ./*.fdb_latexmk ./*.fls ./*.log ./*.out ./*.pdf ./*.toc 

#install: $(BINDIR)/$(EXE)
#	cp $(BINDIR)/$(EXE) $(INSTALL_DIR)/$(EXE)

#uninstall:
#	rm -fv $(INSTALL_DIR)/$(EXE)

distclean: clean
