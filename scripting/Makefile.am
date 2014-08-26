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

PYTHON_WRAPPER = loris_python_wrap.C
SWIG_INTERFACE = $(top_srcdir)/scripting/loris.i
ALL_IFILES = $(SWIG_INTERFACE) $(top_srcdir)/scripting/lorisPartialList.i \
             $(top_srcdir)/scripting/lorisFundamental.i \
             $(top_srcdir)/scripting/lorisEnvelope.i \
             $(top_srcdir)/scripting/lorisChannelizer.i \
             $(top_srcdir)/scripting/lorisSynthesizer.i


EXTRA_DIST = $(ALL_IFILES) 

CLEANFILES = loris.py loris.pyo loris.pyc $(PYTHON_WRAPPER)
MAINTAINERCLEANFILES = Makefile.in


#----------------------------------------------------------------
# If Python is available, define a target for building and
# installing the Python module
#
if BUILD_PYTHON
pythonsitepkgdir = $(pyexecdir)
pyexec_PYTHON = loris.py

pythonsitepkg_LTLIBRARIES = _loris.la
nodist__loris_la_SOURCES = $(PYTHON_WRAPPER)
_loris_la_CXXFLAGS = -I$(top_srcdir)/src $(PYFLAGS) 
_loris_la_LIBADD = $(top_builddir)/src/libloris.la 
_loris_la_LDFLAGS = -module -avoid-version 

endif

#----------------------------------------------------------------
# Define a target for building the
# wrapper from swig interface files.
#

wrapper: $(PYTHON_WRAPPER)


$(PYTHON_WRAPPER): $(ALL_IFILES)
	$(SWIG) -python -c++ -module loris -o $(PYTHON_WRAPPER) $(SWIG_INTERFACE)
