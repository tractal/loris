/* 
	config.h for windows

   Normally generated from config.h.in by configure, but this
   one is hand-written for Visual C++ (2008).  Include this
   as a prefix to all source files in Loris build projects, or
   define HAVE_CONFIG_H and put this header in your project 
   include path, or define these symbols in the "PreprocessorDefinitions"
   properties.
   
   Some of these symbols can be defined or un-defined to customize
   your build. 
*/


/* Define to enable internal Loris debugging code (not recommended). */
/* #undef Debug_Loris */

/* Define to 1 if you have the <csdl.h> header file. */
/* #undef HAVE_CSDL_H */

/* Define to 1 if you have the <csound/cs.h> header file. */
/* #undef HAVE_CSOUND_CS_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #define HAVE_DLFCN_H 1 */

/* Define to 1 if you have the <fftw3.h> header file. */
/* #define HAVE_FFTW3_H 1 */

/* Define to 1 if you have the <fftw.h> header file. */
/* #undef HAVE_FFTW_H */

/* Define to 1 if you have the <inttypes.h> header file. */
/* #define HAVE_INTTYPES_H 1 */

/* Define 1 if the isfinite macro is defined in cmath, 0 otherwise. */
/* #define HAVE_ISFINITE 1 */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define 1 if M_PI defined in cmath, 0 otherwise. */
/* #define HAVE_M_PI 1 */

/* Define to 1 if you have the <Python.h> header file. */
#define HAVE_PYTHON_H 1

/* Define to 1 if you have the <stdint.h> header file. */
/* #define HAVE_STDINT_H 1 */

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
/* #define HAVE_SYS_STAT_H 1 */

/* Define to 1 if you have the <sys/types.h> header file. */
/* #define HAVE_SYS_TYPES_H 1 */

/* Define to 1 if you have the <unistd.h> header file. */
/* #define HAVE_UNISTD_H 1 */

/* Define for compilers that do not support template member functions. */
/* #undef NO_TEMPLATE_MEMBERS */

/* Name of package */
#define PACKAGE "loris"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "loris@cerlsoundgroup.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "Loris"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Loris 1.5.3"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "loris"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.5.3"

/* The size of `bool', as computed by sizeof. */
#define SIZEOF_BOOL 1

/* The size of `char', as computed by sizeof. */
#define SIZEOF_CHAR 1

/* The size of `double', as computed by sizeof. */
#define SIZEOF_DOUBLE 8

/* The size of `float', as computed by sizeof. */
#define SIZEOF_FLOAT 4

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `int *', as computed by sizeof. */
#define SIZEOF_INT_P 4

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 4

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* The size of `size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T 4

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.5.3"

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define to 1 if type `char' is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
/* # undef __CHAR_UNSIGNED__ */
#endif
