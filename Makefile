#
# C++ Compiler
CXX := g++
#
# Make command
MAKE := make
#
# latexmk
TEX := $(shell command -v latexmk 2> /dev/null)
#
# Flags
#
#   Compiler
CXXFLAGS += -c -g -Wall -pthread -std=c++11
#
#   Linker
LDFLAGS += -g -lpthread
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
ION_FINDER_EXE := ionFinder
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
ION_FINDER_SRCDIR = $(SRCDIR)/ionFinder
#
#
#python dir
#
PYTHON_DIR = python
#   Objects
OBJDIR := obj
MS2_ANNOTATOR_OBJDIR := $(OBJDIR)/ms2_annotator
ION_FINDER_OBJDIR := $(OBJDIR)/ionFinder
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
ION_FINDER_SRCS := $(wildcard $(ION_FINDER_SRCDIR)/*.cpp)
#ALL_SRCS = $(SRCS) $(MS2_ANNOTATOR_SRCS)

OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS:.cpp=.o))
MS2_ANNOTATOR_OBJS += $(OBJS) $(subst src/,obj/,$(MS2_ANNOTATOR_SRCS:.cpp=.o))
ION_FINDER_OBJS += $(OBJS) $(subst src/,obj/,$(ION_FINDER_SRCS:.cpp=.o))
ALL_OBJS = $(OBJS) $(MS2_ANNOTATOR_OBJS) $(ION_FINDER_OBJS)

CXXFLAGS += $(INCLUDEFLAGS) -I$(HEADERDIR)
LDFLAGS += $(LIBFLAGS)

.PHONY: all clean distclean install uninstall multi

#TARGETS = $(HEADERDIR)/$(GIT_VERSION) $(BINDIR)/$(EXE) $(BINDIR)/DTsetup helpFile.pdf DTarray_pro-Userguide.pdf
TARGETS += $(BINDIR)/$(MS2_ANNOTATOR_EXE) $(BINDIR)/$(ION_FINDER_EXE) ionFinder_help.pdf $(BINDIR)/make_ms2 $(BINDIR)/run_make_ms2

all: $(TARGETS)

multi:
	$(MAKE) -j8 all
	$(MAKE)

#DTarray_pro-Userguide.pdf : $(TEX_DIR)/DTarray_pro-Userguide.tex
#ifndef TEX
# 	$(warning "No latexmk in $(PATH), skipping build of DTarray_pro-Userguide.pdf")
# else
# 	cd $(TEX_DIR) && latexmk -pdf DTarray_pro-Userguide.tex
# 	cp $(TEX_DIR)/DTarray_pro-Userguide.pdf .
# endif

$(BINDIR)/$(MS2_ANNOTATOR_EXE): $(MS2_ANNOTATOR_OBJS)
	mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $(MS2_ANNOTATOR_OBJS) -o $@

$(BINDIR)/$(ION_FINDER_EXE): $(ION_FINDER_OBJS)
	mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $(ION_FINDER_OBJS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERDIR)/%.hpp
	mkdir -p $(OBJDIR) $(MS2_ANNOTATOR_OBJDIR) $(ION_FINDER_OBJDIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(BINDIR)/make_ms2: $(PYTHON_DIR)/make_ms2.py
	bash $(SCRIPTS)/addPythonFiles.sh $(PYTHON_DIR)/make_ms2.py

$(BINDIR)/run_make_ms2: $(PYTHON_DIR)/run_make_ms2.py
	bash $(SCRIPTS)/addPythonFiles.sh $(PYTHON_DIR)/run_make_ms2.py

ionFinder_help.pdf : man/ionFinder/helpFile.roff
	bash $(SCRIPTS)/updateMan.sh man/ionFinder/helpFile.roff ionFinder_help.pdf

clean:
	rm -f $(ALL_OBJS) $(BINDIR)/$(MS2_ANNOTATOR_EXE) $(BINDIR)/$(ION_FINDER_EXE)
	rm -f $(BINDIR)/make_ms2 $(BINDIR)/run_make_ms2
	rm -f ionFinder_help.pdf
	rm -r lib/*
	#cd $(TEX_DIR) && rm -f ./*.aux ./*.dvi ./*.fdb_latexmk ./*.fls ./*.log ./*.out ./*.pdf ./*.toc 

#install: $(BINDIR)/$(EXE)
#	cp $(BINDIR)/$(EXE) $(INSTALL_DIR)/$(EXE)

#uninstall:
#	rm -fv $(INSTALL_DIR)/$(EXE)

distclean: clean
