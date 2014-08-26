# Loris is Copyright (c) 1999-2010 by Kelly Fitz and Lippold Haken
# <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = tryit.csd trymorph.csd lorisgens5.C lorisgens5.h lorisgens4.C lorisgens4.h

MAINTAINERCLEANFILES = Makefile.in

if HAVE_CSOUND5
# source code for the Csound5 opcodes
CSOUND_SRC = lorisgens5.C lorisgens5.h
# CSOUND_SRC = extendedgens5.C extendedgens5.h
endif

if HAVE_CSOUND4
# source code for the Csound4 opcodes
CSOUND_SRC = lorisgens4.C lorisgens4.h
endif

# build a convenience library containing the Csound opcodes
if HAVE_CSOUND
noinst_LTLIBRARIES = liblorisops.la
liblorisops_la_SOURCES = $(CSOUND_SRC)
liblorisops_la_CXXFLAGS = $(CSOUND_CXXFLAGS) $(AM_CXXFLAGS) -I$(top_srcdir)/src
endif
