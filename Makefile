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
# git version vars
GITVERSION:= $(shell git log -1 --pretty='%h')
GITDATE:= $(shell git log -1 --format=%cd --date=local)
#
# Flags
#
#   Compiler
CXXFLAGS += -c -g -Wall -pthread -std=c++11 -DGIT_VERSION="\"${GITVERSION}\"" -DGIT_DATE="\"${GITDATE}\""
#
#   Linker
LDFLAGS += -g -lpthread
#
#   Library
LIBFLAGS :=
#
#   Include
INCLUDEFLAGS := -I./utils/include
#
#
# Program name
MS2_ANNOTATOR_EXE := ms2_annotator
ION_FINDER_EXE := ionFinder
#
#
# Directories
#
# utils
UTILS_DIR := utils
UTILS_LIB := lib/utils.a
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
#	Libraries
LIBDIR := lib
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
#
################################################################################

HEADERS := $(wildcard $(HEADERDIR)/*.hpp)
SRCS := $(wildcard $(SRCDIR)/*.cpp)
MS2_ANNOTATOR_SRCS := $(wildcard $(MS2_ANNOTATOR_SRCDIR)/*.cpp)
ION_FINDER_SRCS := $(wildcard $(ION_FINDER_SRCDIR)/*.cpp)


OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS:.cpp=.o))
MS2_ANNOTATOR_OBJS += $(OBJS) $(subst src/,obj/,$(MS2_ANNOTATOR_SRCS:.cpp=.o))
ION_FINDER_OBJS += $(OBJS) $(subst src/,obj/,$(ION_FINDER_SRCS:.cpp=.o))
ALL_OBJS = $(OBJS) $(MS2_ANNOTATOR_OBJS) $(ION_FINDER_OBJS)

CXXFLAGS += $(INCLUDEFLAGS) -I$(HEADERDIR)
LDFLAGS += $(LIBFLAGS)

.PHONY: all clean distclean install uninstall doc

TARGETS += $(BINDIR)/$(MS2_ANNOTATOR_EXE) $(BINDIR)/$(ION_FINDER_EXE) ionFinder_help.pdf $(BINDIR)/make_ms2 $(BINDIR)/run_make_ms2 $(BINDIR)/parse_scaffold

all: $(TARGETS)

#DTarray_pro-Userguide.pdf : $(TEX_DIR)/DTarray_pro-Userguide.tex
#ifndef TEX
# 	$(warning "No latexmk in $(PATH), skipping build of DTarray_pro-Userguide.pdf")
# else
# 	cd $(TEX_DIR) && latexmk -pdf DTarray_pro-Userguide.tex
# 	cp $(TEX_DIR)/DTarray_pro-Userguide.pdf .
# endif

$(BINDIR)/$(MS2_ANNOTATOR_EXE): $(UTILS_LIB) $(MS2_ANNOTATOR_OBJS)
	mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $(MS2_ANNOTATOR_OBJS) $(UTILS_LIB) -o $@

$(BINDIR)/$(ION_FINDER_EXE): $(UTILS_LIB) $(ION_FINDER_OBJS)
	mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $(ION_FINDER_OBJS) $(UTILS_LIB) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERDIR)/%.hpp
	mkdir -p $(OBJDIR) $(MS2_ANNOTATOR_OBJDIR) $(ION_FINDER_OBJDIR)
	$(CXX) $(CXXFLAGS) $< -o $@

$(UTILS_LIB):
	cd $(UTILS_DIR); $(MAKE)
	mkdir -p $(LIBDIR)
	cd $(LIBDIR); ln -s ../$(UTILS_DIR)/$(UTILS_LIB)

$(BINDIR)/make_ms2: $(PYTHON_DIR)/make_ms2.py
	bash $(SCRIPTS)/addPythonFiles.sh $(PYTHON_DIR)/make_ms2.py

$(BINDIR)/run_make_ms2: $(PYTHON_DIR)/run_make_ms2.py
	bash $(SCRIPTS)/addPythonFiles.sh $(PYTHON_DIR)/run_make_ms2.py

$(BINDIR)/parse_scaffold: $(PYTHON_DIR)/parse_scaffold.py
	bash $(SCRIPTS)/addPythonFiles.sh $(PYTHON_DIR)/parse_scaffold.py

ionFinder_help.pdf : man/ionFinder/helpFile.roff
	bash $(SCRIPTS)/updateMan.sh man/ionFinder/helpFile.roff ionFinder_help.pdf

doc:
	doxygen doc/doxygen/Doxyfile

clean:
	rm -f $(ALL_OBJS) $(BINDIR)/$(MS2_ANNOTATOR_EXE) $(BINDIR)/$(ION_FINDER_EXE) $(LIBDIR)/*.a
	rm -f $(BINDIR)/make_ms2 $(BINDIR)/run_make_ms2 $(BINDIR)/parse_scaffold
	rm -f ionFinder_help.pdf
	cd $(UTILS_DIR); $(MAKE) clean
	#rm -r lib/*
	#cd $(TEX_DIR) && rm -f ./*.aux ./*.dvi ./*.fdb_latexmk ./*.fls ./*.log ./*.out ./*.pdf ./*.toc 

distclean: clean
