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

EXTRA_DIST = clarinet.aiff flute.aiff fromKyma.spc morphtest.py \
			 one_synth_phase_test.sdif csound_test.csd

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/src

# simple morphing test
test_cpp_SOURCES = morphtest.C
test_cpp_LDADD = $(top_builddir)/src/libloris.la

# procedural interface test
test_pi_SOURCES = pitest.c
test_pi_LDADD = $(top_builddir)/src/libloris.la -lstdc++

# Morpher unit tests
test_morpher_SOURCES = test_Morpher.C
# Darwin is special, dynamic linking sometimes seems to fail 
# on this test, can't figure it out.
test_morpher_LDADD = $(top_builddir)/src/libloris.la

# Partial unit tests
test_partial_SOURCES = test_Partial.C
test_partial_LDADD = $(top_builddir)/src/libloris.la

# Distiller unit tests
test_distiller_SOURCES = test_Distiller.C
test_distiller_LDADD = $(top_builddir)/src/libloris.la

# SdifFile unit tests
test_sdiffile_SOURCES = test_SdifFile.C
test_sdiffile_LDADD = $(top_builddir)/src/libloris.la

# AiffFile (and SpcFile) unit tests
test_aiff_SOURCES = test_Aiff.C
test_aiff_LDADD = $(top_builddir)/src/libloris.la

# synthesis/analysis identity test
test_identity_SOURCES = test_Identity.C
test_identity_LDADD = $(top_builddir)/src/libloris.la

# fundamental frequency estimation tests
test_fundamental_SOURCES = test_Fundamental.C
test_fundamental_LDADD = $(top_builddir)/src/libloris.la

# digital filter test
test_filter_SOURCES = test_Filter.C
test_filter_LDADD = $(top_builddir)/src/libloris.la

# Synthesizer unit tests
test_synthesizer_SOURCES = test_Synthesizer.C
test_synthesizer_LDADD = $(top_builddir)/src/libloris.la

# Cropper unit tests
test_crop_SOURCES = test_Cropper.C
test_crop_LDADD = $(top_builddir)/src/libloris.la

# Resampler unit tests
test_resample_SOURCES = test_Resampler.C
test_resample_LDADD = $(top_builddir)/src/libloris.la

# Test Python module only if that module was built.
if BUILD_PYTHON
PYTHON_TEST = run_pytest

SET_PYTHON_ENV = "env PYTHONPATH=$(top_srcdir)/scripting:$(top_builddir)/scripting/.libs"

if DARWIN
SET_DARWIN_ENV = "env DYLD_LIBRARY_PATH=$(top_builddir)/src/.libs"
endif

$(PYTHON_TEST):
	echo "$(SET_PYTHON_ENV) $(SET_DARWIN_ENV) \
              $(PYTHON) $(top_srcdir)/test/morphtest.py" >> $@
	chmod +x $@
endif

# Test Csound if Csound opcodes were built into the library.
if HAVE_CSOUND
CSOUND_TEST = run_csound_test


if DARWIN
CSOUND_OPCODELIB = "--opcode-lib=$(top_builddir)/src/.libs/libloris.dylib"
else
CSOUND_OPCODELIB = "--opcode-lib=$(top_builddir)/src/.libs/libloris.so"
endif
	
$(CSOUND_TEST):
	echo "$(CSOUND) $(CSOUND_OPCODELIB) $(DARWIN_CSOUND_OPCODELIB) $(top_srcdir)/test/csound_test.csd" >> $@
	chmod +x $@
endif

check_PROGRAMS = test_cpp test_pi test_aiff test_partial test_distiller \
                 test_sdiffile test_morpher test_identity test_fundamental \
                 test_filter test_synthesizer test_crop test_resample

check_SCRIPTS = $(PYTHON_TEST) $(CSOUND_TEST)

TESTS = $(check_PROGRAMS) $(check_SCRIPTS) 

CLEANFILES = $(PYTHON_TEST) $(CSOUND_TEST)

clean-local:
	-rm -fr *.ctest.* *.pytest.* *.pi.* tmp.sdif csound_opcode_test.aiff
