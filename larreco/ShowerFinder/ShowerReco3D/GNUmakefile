#
# This is an example GNUmakefile for my packages
#

# specific names for this package
DICT  = ShowerReco3DCint
SHLIB = libRecoTool_ShowerReco3D.so
SOURCES = $(filter-out $(DICT).cxx, $(wildcard *.cxx))
FMWK_HEADERS = LinkDef.h $(DICT).h
HEADERS = $(filter-out $(FMWK_HEADERS), $(wildcard *.h))
OBJECTS = $(SOURCES:.cxx=.o)

# include options for this package
INCFLAGS = -I.                       #Include itself
INCFLAGS += -I$(LARLITE_COREDIR)
INCFLAGS += -I$(LARLITE_USERDEVDIR)/BasicTool
INCFLAGS += -I$(LARLITE_USERDEVDIR)/RecoTool
INCFLAGS += -I$(LARLITE_USERDEVDIR)/RecoTool/FANN/include

# platform-specific options
OSNAME          = $(shell uname -s)
HOST            = $(shell uname -n)
OSNAMEMODE      = $(OSNAME)

include $(LARLITE_BASEDIR)/Makefile/Makefile.${OSNAME}

# set compiler options for ROOT
CXXFLAGS +=  `root-config --cflags`

# CINT flag
CINTFLAGS += "-p"

# call the common GNUmakefile
include $(LARLITE_BASEDIR)/Makefile/GNUmakefile.CORE



