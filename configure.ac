dnl Loris is Copyright (c) 1999-2010 by Kelly Fitz and Lippold Haken 
dnl <loris@cerlsoundgroup.org>
dnl  
dnl This file is free software; as a special exception the author gives
dnl unlimited permission to copy and/or distribute it, with or without 
dnl modifications, as long as this notice is preserved.
dnl 
dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
dnl implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


dnl initialization
AC_INIT(Loris, 1.8, loris@cerlsoundgroup.org, loris)

AC_CONFIG_AUX_DIR(config)
AC_SUBST(ac_aux_dir)
AM_INIT_AUTOMAKE(1.6)
AM_CONFIG_HEADER(config.h)

dnl LT_INIT
dnl LT_INIT doesn't seem to work?
AC_SUBST([LIBTOOL_DEPS])

dnl----------------------------------------------------------------
dnl Package Name And Version
dnl----------------------------------------------------------------

AC_MSG_RESULT(----- Package Name And Version -----)

AC_MSG_RESULT(this is $PACKAGE_STRING)

dnl Create loris.h from loris.h.in, subsituting the 
dnl values of the version macros.
dnl Make sure these are consistent with the version number
dnl in AC_INIT above, and make sure to check them before 
dnl each release!!!!!
LORIS_MAJOR_VERSION=1
LORIS_MINOR_VERSION=8
LORIS_SUBMINOR_VERSION=
LORIS_PRERELEASE_STR=
LORIS_VERSION_STR="$PACKAGE_STRING$LORIS_PRERELEASE_STR"
AC_SUBST(LORIS_MAJOR_VERSION)
AC_SUBST(LORIS_MINOR_VERSION)
AC_SUBST(LORIS_SUBMINOR_VERSION)
AC_SUBST(LORIS_VERSION_STR)
AC_CONFIG_FILES([src/loris.h doc/Doxyfile])

dnl----------------------------------------------------------------
dnl Checks for programs.
dnl----------------------------------------------------------------

AC_MSG_RESULT(----- Program Checks -----)
AC_PROG_CXX
AM_PROG_LIBTOOL
AC_LIBTOOL_CXX

AC_ARG_VAR([DOXYGEN], 
       [Path to doxygen when available])
AC_PATH_PROG([DOXYGEN], [doxygen])

AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

AC_ARG_VAR([SWIG], 
       [Path to swig when available])
AC_PATH_PROG([SWIG], [swig])

AM_CONDITIONAL([HAVE_SWIG], [test -n "$SWIG"])

if test -z "$SWIG" ; then
        AC_MSG_WARN([Cannot find SWIG. Python module cannot be built.])
fi

dnl----------------------------------------------------------------
dnl Perform special libtool checks under Darwin.
dnl----------------------------------------------------------------

dnl  AC_DEFUN([PATCH_LIBTOOL_ON_DARWIN_ZSH_OVERQUOTING],
dnl  [# libtool-1.4 specific, on zsh target the final requoting does one too much
dnl  case "$host_os" in
dnl    darwin*)
dnl      if grep "1.920" libtool ; then
dnl        AC_MSG_RESULT(patching libtool on .so-sharedlib creation (zsh overquoting))
dnl        test -f libtool.old || (mv libtool libtool.old && cp libtool.old libtool)
dnl        sed -e '/archive_cmds=/s:\\\\":\\":g' libtool > libtool.new
dnl        mv libtool.new libtool
dnl        test -f libtool || (test -f libtool.old && mv libtool.old libtool)
dnl      fi
dnl    ;;
dnl  esac
dnl  ])
dnl  PATCH_LIBTOOL_ON_DARWIN_ZSH_OVERQUOTING
dnl  
dnl don't seem to need this anymore
dnl scratch that, I think we do still need it
dnl
AC_DEFUN([KELLYS_GREAT_DARWIN_FIX],
[
dnl Still need this because the dynamic library path is not set correctly
dnl under Darwin, so the tests don't find and link the uninstalled library 
dnl unless I set DYLD_LIBRARY_PATH before launching them.
dnl See test/Makefile.am.
case "$host_os" in
*darwin*)
AC_MSG_RESULT(Using Darwin-specific kludges to avoid broken libtool.)
DARWIN=true
;;
esac
])

KELLYS_GREAT_DARWIN_FIX

AM_CONDITIONAL(DARWIN, test -n "$DARWIN")

dnl----------------------------------------------------------------
dnl Checks for compiler features.
dnl----------------------------------------------------------------

AC_MSG_RESULT(----- Compiler Feature Checks -----)
AC_LANG_SAVE
AC_LANG_CPLUSPLUS

dnl SIZEOF buildin integer types, min is the minmimum according ISO
dnl SIZEOF_BOOL
AC_CHECK_SIZEOF(bool)
dnl SIZEOF_CHAR, min = 1
AC_CHECK_SIZEOF(char)
dnl define __CHAR_UNSIGNED__ , unless defined by compiler
AC_C_CHAR_UNSIGNED
dnl SIZEOF_SHORT, min = 2
AC_CHECK_SIZEOF(short)
dnl SIZEOF_INT, min = 4
AC_CHECK_SIZEOF(int)
dnl SIZEOF_LONG, min = 4
AC_CHECK_SIZEOF(long)
dnl assign default, if not defined
dnl AC_CHECK_TYPE([long long], long)
dnl AC_CHECK_TYPE([unsigned long long], [unsigned long])
dnl SIZEOF_LONG_LONG, 0 if undefined
dnl AC_CHECK_SIZEOF([long long])
dnl SIZEOF_INT_P
AC_CHECK_SIZEOF(int *) 
dnl SIZEOF_SIZE_T
AC_CHECK_SIZEOF(size_t) 

dnl SIZEOF_FLOAT, should always be 4 I think
AC_CHECK_SIZEOF(float)
dnl SIZEOF_DOUBLE, should always be 8 I think
AC_CHECK_SIZEOF(double)

dnl Check for isfinite macro (C99 standard, not in Visual Studio).
AH_TEMPLATE([HAVE_ISFINITE],
            [Define 1 if the isfinite macro is defined in cmath, 0 otherwise.])
AC_MSG_CHECKING(for isfinite defined in cmath)
AC_TRY_COMPILE([ #include <cmath>;], [bool b = std::isfinite(0);], [
    AC_MSG_RESULT(yep)
    AC_DEFINE([HAVE_ISFINITE], 1) ], [
    AC_MSG_RESULT(nope using local definition of infinite)
    AC_DEFINE([HAVE_ISFINITE], 0) ] )


dnl Check for endian-ness of system.
AC_C_BIGENDIAN()

dnl Check for a definition of M_PI in cmath, if not use our own.
AH_TEMPLATE([HAVE_M_PI],
            [Define 1 if M_PI defined in cmath, 0 otherwise.])
AC_MSG_CHECKING(for M_PI defined in cmath)
AC_TRY_COMPILE([ #include <cmath>;], [double pi = M_PI;], [
    AC_MSG_RESULT(yep)
    AC_DEFINE([HAVE_M_PI], 1) ], [
    AC_MSG_RESULT(nope using local definition of pi)
    AC_DEFINE([HAVE_M_PI], 0) ] )


dnl check whether template member functions should
dnl be used
AH_TEMPLATE([NO_TEMPLATE_MEMBERS],
            [Define for compilers that do not support template member functions.])
AC_MSG_CHECKING(whether C++ compiler supports template member functions)
AC_COMPILE_IFELSE([
    AC_LANG_SOURCE([[
        #include <vector>
        struct C
        {
            int z;
            template< class T >
            C( T b, T e ) : z( e-b ) {}
        };]], [[
        std::vector< int > v(100, 1);
        C c( v.begin(), v .end() );
        return 0;
        ]])], [
    AC_MSG_RESULT(yes)], [
    AC_MSG_RESULT(no, not using template members)
    AC_DEFINE([NO_TEMPLATE_MEMBERS])])

dnl restore this
AC_LANG_RESTORE

dnl----------------------------------------------------------------
dnl Check for librairies
dnl----------------------------------------------------------------

AC_MSG_RESULT(----- Library Checks -----)

dnl----------------------------------------------------------------
dnl Look for FFTW
dnl
dnl----------------------------------------------------------------

AC_ARG_WITH(fftw,
    AC_HELP_STRING( [--with-fftw],
                    [use FFTW library if available (default is YES)] ),
    [TRYFFTW="$withval"], [TRYFFTW="yes"])

if test "$TRYFFTW" == "yes" ; then
    dnl Look for FFTW library, prefer version 3.
    
    dnl Remember that fftw library won't link without -lm.
    AC_SEARCH_LIBS([sin], [m])

    AC_CHECK_LIB([fftw3], [fftw_execute], [ 
            FFTW_VERSION=3
            FFTW_HDR="fftw3.h"
            LINK_FFTW=-lfftw3
        ],
        AC_CHECK_LIB([fftw], [fftw_create_plan_specific],[
                FFTW_VERSION=2
                FFTW_HDR="fftw.h"
                LINK_FFTW=-lfftw
            ],
            AC_MSG_WARN([Not using the FFTW library.  Infrequent non-power-of-two DFTs will be slow.])))
fi

if test "$FFTW_VERSION" == "3"; then
    AC_CHECK_HEADERS([fftw3.h],,
        AC_MSG_ERROR([Cannot find FFTW3 headers.  Add something to CPPFLAGS.]))
elif test "$FFTW_VERSION" == "2"; then
    AC_CHECK_HEADERS([fftw.h],,
        AC_MSG_ERROR([Cannot find FFTW headers.  Add something to CPPFLAGS.]))
fi

AC_SUBST(LINK_FFTW)

dnl----------------------------------------------------------------
dnl Check for scripting languages
dnl----------------------------------------------------------------

AC_MSG_RESULT(----- Language Checks -----)

dnl----------------------------------------------------------------
dnl Look for Python
dnl----------------------------------------------------------------
dnl This new way is not as flexible as the old way, but it much 
dnl easier to maintain. Thanks to John Ramsdell for helping to
dnl clean this up.

AC_ARG_VAR([PYTHON], 
       [Path to python when available])

AC_ARG_WITH(python,
    AC_HELP_STRING( [--with-python],
                    [build Python module (default is YES)] ),
    [USEPYTHON="$withval"], [USEPYTHON="yes"])

if test -z "$SWIG" ; then
	USEPYTHON="no"      
fi

if test "$USEPYTHON" == "yes" ; then
    AC_PATH_PROG([PYTHON], [python])
    
    if test -n "$PYTHON" ; then
        dnl use automake tools for finding python, set 
        dnl installation directory for extension modules
        AM_PATH_PYTHON(1.5)
        
        dnl look for the header Python.h
        PYBINDIR="`AS_DIRNAME([$PYTHON])`"
        PYPREFDIR="`AS_DIRNAME([$PYBINDIR])`"
        PYINCLUDE="$PYPREFDIR/include/python$PYTHON_VERSION"  

        dnl try compiling with $PYINCLUDE in the header path
        SAVEFLAGS="$CPPFLAGS"
        CPPFLAGS="-I$PYINCLUDE $CPPFLAGS"
        AC_MSG_CHECKING(for Python.h header file)
        AC_CHECK_HEADERS([Python.h],[
            dnl check whether this version of Python supports
            dnl the new style of iterators
            AC_MSG_CHECKING(for PyExc_StopIteration defined in Python.h)
            AC_TRY_COMPILE( [ #include <Python.h>;], 
                            [ PyErr_SetString( PyExc_StopIteration, ""); ], 
                [
                    AC_MSG_RESULT(yep)
                    PYFLAGS="-I$PYINCLUDE"  
                ], [
                    AC_MSG_RESULT(nope)
                    PYFLAGS="-I$PYINCLUDE -DNO_PYTHON_EXC_STOPITER"
                ] )     
        ],[
            AC_MSG_WARN([Cannot find Python headers. Python support disabled.])
            PYTHON=
        ])
        CPPFLAGS="$SAVEFLAGS"
    else
        AC_MSG_WARN([Cannot find Python. Python support disabled.])
    fi
fi

AM_CONDITIONAL(BUILD_PYTHON, test -n "$PYTHON")
AC_SUBST(PYFLAGS)

dnl----------------------------------------------------------------
dnl Look for csound.
dnl
dnl If csound (header) is available, then prepare to build the csound 
dnl opcodes. Prefer Csound5.
dnl----------------------------------------------------------------

AC_ARG_WITH(csound,
    AC_HELP_STRING( [--with-csound5],
                    [build Csound5 opcodes (default is YES)] ),
    [USECSOUND5="$withval"], [USECSOUND5="yes"])
    
if test "$USECSOUND5" == "yes" ; then
	AC_CHECK_HEADERS([csdl.h],
				FOUND_CSOUND5="yes",
				AC_MSG_WARN([Cannot find Csound5 headers. Csound5 support disabled.]))
fi

AM_CONDITIONAL([HAVE_CSOUND5], [test -n "$FOUND_CSOUND5"])

AC_ARG_VAR([CSOUND_CONFIG], 
       [Path to the Csound4 configuration script when available (Linux only)])


dnl This configure code, parts provided by John Ramsdell,
dnl works for Csound GBS versions 4.23f12gbs.10 or later.

AC_ARG_WITH(csound4,
    AC_HELP_STRING( [--with-csound4],
                    [build Csound 4 opcodes (default is NO)] ),
    [USECSOUND4="$withval"], [USECSOUND4="no"])

if test "$FOUND_CSOUND5" == "yes" ; then
	USECSOUND4="no"
fi

if test "$USECSOUND4" == "yes" ; then
    AC_PATH_PROG([CSOUND_CONFIG], [csound-config])
    FOUND_CSOUND4="$CSOUND_CONFIG"

    dnl try to use csound config to get build flags
    dnl for the csound opcodes
    if test -n "$CSOUND_CONFIG"; then
        CSOUND_CXXFLAGS="`$CSOUND_CONFIG --cflags`"
        
        CSOUND_PREFIX="`$CSOUND_CONFIG --prefix`"   
    else
        AC_CHECK_HEADERS([csound/cs.h],
            FOUND_CSOUND4="yes",
            AC_MSG_WARN([Cannot find Csound4 headers. Csound4 support disabled.]))
    fi
fi
AC_SUBST([CSOUND_CXXFLAGS])
AC_SUBST([CSOUND_PREFIX])

AM_CONDITIONAL([HAVE_CSOUND4], [test -n "$FOUND_CSOUND4"])

if test "$FOUND_CSOUND5" == "yes" ; then
	HAVE_CSOUND="yes"
fi
if test "$FOUND_CSOUND4" == "yes" ; then
	HAVE_CSOUND="yes"
fi

if test "$HAVE_CSOUND" == "yes" ; then
	AC_PATH_PROG([CSOUND], [csound])
fi

AM_CONDITIONAL([HAVE_CSOUND], [test -n "$HAVE_CSOUND"])



dnl----------------------------------------------------------------
dnl Check for Loris debugging flag
dnl----------------------------------------------------------------
AC_ARG_ENABLE(debugloris,
    AC_HELP_STRING( [--enable-debugloris],
                    [enable internal Loris debugging code (not recommended)] ),
    [DEBUG_LORIS="$enableval" ], [DEBUG_LORIS=] )

AH_TEMPLATE([Debug_Loris],
            [Define to enable internal Loris debugging code (not recommended).])

if test -n "$DEBUG_LORIS"; then
    if test "$DEBUG_LORIS"="yes"; then
        AC_MSG_RESULT(enabling debugging code (defining Debug_Loris))
        AC_DEFINE([Debug_Loris])
    else
        AC_MSG_RESULT(enabling debugging code (defining Debug_Loris to $DEBUG_LORIS))
        AC_DEFINE([Debug_Loris], "$DEBUG_LORIS")
    fi
fi

dnl----------------------------------------------------------------
dnl Generate Makefiles
dnl----------------------------------------------------------------

AC_MSG_RESULT(----- Creating Output Files -----)

AC_OUTPUT(Makefile doc/Makefile src/Makefile \
scripting/Makefile csound/Makefile test/Makefile \
utils/Makefile )

AC_MSG_RESULT(----- Configuration Summary -----)

if test -n "$FFTW_VERSION" ; then
    echo FFTW support: Enabled, using version $FFTW_VERSION.
else
    echo FFTW support: Disabled.
    echo Add something to CPPFLAGS and LDFLAGS to enable.  
    echo See "./configure --help".
fi


if test -n "$PYTHON" ; then
    echo Python support: Enabled.
    echo Installing Python extension modules in $pyexecdir
    echo You may need to add $pyexecdir to your PYTHONPATH 
    echo before Python will be able to load them.
else
    echo Python support: Disabled.
    echo Define PYTHON or add something to CPPFLAGS to enable.  
    echo See "./configure --help".
fi

if test -n "$HAVE_CSOUND" ; then
    echo Csound support: Enabled.
    echo To use Loris opcodes in your instruments, invoke csound with the argument:
    echo   --opcode-lib=$prefix/lib/libloris.so
    echo   or libloris.dylib on Mac OS X
else
    echo Csound support: Disabled.
    echo Define CSOUND_CONFIG or add something to CPPFLAGS to enable.  
	echo See "./configure --help".
fi


AC_MSG_RESULT(----- End Of Configuration -----)
