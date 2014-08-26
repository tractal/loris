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

bin_PROGRAMS  = loris-analyze loris-synthesize loris-spewmarkers \
                loris-mark loris-unmark loris-dilate

# loris-analyze: a utility program to analyze
# samples and store the partials in an SDIF file.
loris_analyze_SOURCES = loris_analyze.C
loris_analyze_CXXFLAGS = -I$(top_srcdir)/src
loris_analyze_LDADD = $(top_builddir)/src/libloris.la 
loris_analyze_LDFLAGS = -static

# loris-dilate: a utility program to dilate Partials
# stored in a SDIF file, optionally with amplitude
# and frequency scaling by a constant factor.
loris_dilate_SOURCES = loris_dilate.C
loris_dilate_CXXFLAGS = -I$(top_srcdir)/src
loris_dilate_LDADD = $(top_builddir)/src/libloris.la
loris_dilate_LDFLAGS = -static

# loris-synthesize: a utility program to render Partials
# stored in a SDIF file, optionally with dilation and amplitude
# and frequency scaling by a constant factor.
loris_synthesize_SOURCES = loris_synthesize.C
loris_synthesize_CXXFLAGS = -I$(top_srcdir)/src
loris_synthesize_LDADD = $(top_builddir)/src/libloris.la
loris_synthesize_LDFLAGS = -static

# loris-spewmarkers: a utility program to read the
# markers stored in a AIFF, SPC, or SDIF file and
# print them to standard output.
loris_spewmarkers_SOURCES = loris_spewmarkers.C
loris_spewmarkers_CXXFLAGS = -I$(top_srcdir)/src
loris_spewmarkers_LDADD = $(top_builddir)/src/libloris.la
loris_spewmarkers_LDFLAGS = -static

# loris-mark: a utility program to add a
# marker to a AIFF, SPC, or SDIF file.
loris_mark_SOURCES = loris_mark.C
loris_mark_CXXFLAGS = -I$(top_srcdir)/src
loris_mark_LDADD = $(top_builddir)/src/libloris.la
loris_mark_LDFLAGS = -static


# loris-unmark: a utility program to remove a
# marker or markers from a AIFF, SPC, or SDIF file.
loris_unmark_SOURCES = loris_unmark.C
loris_unmark_CXXFLAGS = -I$(top_srcdir)/src
loris_unmark_LDADD = $(top_builddir)/src/libloris.la
loris_unmark_LDFLAGS = -static


MAINTAINERCLEANFILES = 	Makefile.in

