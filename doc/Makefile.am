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

EXTRA_DIST = Doxyfile.in opcodes.html utils.html
			 
noinst_DATA = 


INPUT     = @top_srcdir@/src/AiffFile.h		\
			@top_srcdir@/src/Analyzer.h		\
			@top_srcdir@/src/Breakpoint.h	\
			@top_srcdir@/src/BreakpointUtils.h	\
			@top_srcdir@/src/Channelizer.h	\
			@top_srcdir@/src/Collator.h	\
			@top_srcdir@/src/Dilator.h	\
			@top_srcdir@/src/Distiller.h	\
			@top_srcdir@/src/Envelope.h	\
			@top_srcdir@/src/Exception.h	\
			@top_srcdir@/src/Filter.h	\
			@top_srcdir@/src/FourierTransform.h	\
			@top_srcdir@/src/FrequencyReference.h \
			@top_srcdir@/src/Fundamental.h	\
			@top_srcdir@/src/Harmonifier.h	\
			@top_srcdir@/src/ImportLemur.h	\
			@top_srcdir@/src/KaiserWindow.h	\
			@top_srcdir@/src/LinearEnvelope.h	\
			@top_srcdir@/src/Marker.h	\
			@top_srcdir@/src/Morpher.h	\
			@top_srcdir@/src/NoiseGenerator.h \
			@top_srcdir@/src/Notifier.h	\
			@top_srcdir@/src/Oscillator.h	\
			@top_srcdir@/src/Partial.h	\
			@top_srcdir@/src/PartialList.h	\
			@top_srcdir@/src/PartialPtrs.h	\
			@top_srcdir@/src/PartialUtils.h	\
			@top_srcdir@/src/ReassignedSpectrum.h	\
			@top_srcdir@/src/Resampler.h \
			@top_srcdir@/src/SdifFile.h	\
			@top_srcdir@/src/Sieve.h	\
			@top_srcdir@/src/SpcFile.h	\
			@top_srcdir@/src/SpectralSurface.h	\
			@top_srcdir@/src/Synthesizer.h

# add something to build docs using doxygen, if it is present
if HAVE_DOXYGEN

noinst_DATA += html 


CPP_HTML_DIR = html
HTML = ${CPP_HTML_DIR}/index.html

${HTML}: Doxyfile ${INPUT}
	${DOXYGEN} 

html: ${HTML}

PDF = Loris${LORIS_MAJOR_VERSION}.${LORIS_MINOR_VERSION}.pdf

${PDF}: ${HTML}
	cd latex; make 
	cp latex/refman.pdf $@

pdf: ${PDF}

endif

MAINTAINERCLEANFILES = 	Makefile.in 
DISTCLEANFILES = latex/* html/* Doxyfile
