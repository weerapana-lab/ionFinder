
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
EXE := ms2_annotator
#
#
# Directories
#
#   Headers
HEADERDIR := include
#
#   Sources
SRCDIR := src
#
#   Objects
OBJDIR := obj
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

HEADERS := $(wildcard $(HEADERDIR)/*.h)
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS:.cpp=.o))

CXXFLAGS += $(INCLUDEFLAGS) -I$(HEADERDIR)
LDFLAGS += $(LIBFLAGS)

.PHONY: all clean distclean install uninstall

#TARGETS = $(HEADERDIR)/$(GIT_VERSION) $(BINDIR)/$(EXE) $(BINDIR)/DTsetup helpFile.pdf DTarray_pro-Userguide.pdf
TARGETS = $(BINDIR)/$(EXE) helpFile.pdf

all: $(TARGETS)

#DTarray_pro-Userguide.pdf : $(TEX_DIR)/DTarray_pro-Userguide.tex
#ifndef TEX
# 	$(warning "No latexmk in $(PATH), skipping build of DTarray_pro-Userguide.pdf")
# else
# 	cd $(TEX_DIR) && latexmk -pdf DTarray_pro-Userguide.tex
# 	cp $(TEX_DIR)/DTarray_pro-Userguide.pdf .
# endif

$(BINDIR)/$(EXE): $(OBJS)
	mkdir -p $(BINDIR)
	$(CXX) $(LDFLAGS) $? -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $< -o $@

helpFile.pdf : db/helpFile.man
	bash $(SCRIPTS)/updateMan.sh

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/$(EXE)
	rm -f helpFile.pdf
	#cd $(TEX_DIR) && rm -f ./*.aux ./*.dvi ./*.fdb_latexmk ./*.fls ./*.log ./*.out ./*.pdf ./*.toc 

#install: $(BINDIR)/$(EXE)
#	cp $(BINDIR)/$(EXE) $(INSTALL_DIR)/$(EXE)

#uninstall:
#	rm -fv $(INSTALL_DIR)/$(EXE)

distclean: clean
