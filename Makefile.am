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

SUBDIRS = csound src scripting utils doc test

MAINTAINERCLEANFILES = 	Makefile.in aclocal.m4 configure config.h.in \
						stamp-h.in \
 			$(top_srcdir)/$(ac_aux_dir)/config.guess \
            $(top_srcdir)/$(ac_aux_dir)/config.sub \
            $(top_srcdir)/$(ac_aux_dir)/install-sh \
            $(top_srcdir)/$(ac_aux_dir)/ltmain.sh \
            $(top_srcdir)/$(ac_aux_dir)/missing \
            $(top_srcdir)/$(ac_aux_dir)/mkinstalldirs \
            $(top_srcdir)/$(ac_aux_dir)/depcomp

DISTCLEANFILES =


DEMO_DIST_FILES =	Loris/demo/Makefile \
					Loris/demo/README \
					Loris/demo/morphdemo.py \
					Loris/demo/source \
					Loris/demo/reference

EXTRA_DIST = win/Loris_MorphTest.vcproj win/Loris_PythonModule.vcproj win/loris.h win/config.h


dist-demo:
	cd $(top_srcdir)/demo; tar zxf morphsources.tgz; tar zxf morphreference.tgz
	cd $(top_srcdir)/..; tar zcf Loris/$(PACKAGE)-demo-$(VERSION_STR).tgz $(DEMO_DIST_FILES)
