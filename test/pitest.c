/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2010 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *	pitest.C
 *
 *	Very simple Loris instrument tone morphing demonstration using 
 *	the procedural interface to Loris.
 *
 * Kelly Fitz, 2 Feb 2005
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "loris.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

static void notifyAndHalt( const char * msg )
{
   printf( "Loris encountered an error:\n%s\n\n", msg );
   exit( 1 );
}

int main( void )
{
   char filename[ 256 ];
   char * path = getenv("srcdir");

   #define BUFSZ (3*44100)
   double samples[ BUFSZ ]; /* clarinet is about 3 seconds */
   unsigned int N = 0;
   double srate;
   
   PartialList * clar = createPartialList();
   PartialList * flut = createPartialList();
   LinearEnvelope * reference = 0;
   LinearEnvelope * pitchenv = createLinearEnvelope();

   LinearEnvelope * morphenv = createLinearEnvelope();
   PartialList * mrph = createPartialList();   

   double flute_times[] = {0.4, 1.};
   double clar_times[] = {0.2, 1.};
   double tgt_times[] = {0.3, 1.2};
   
   printf( "Loris procedural interface test.\n" );
   printf( "Kelly Fitz 2005\n\n" );
   printf( "Generates a simple linear morph between a \n" );
   printf( "clarinet and a flute using the Loris library.\n\n" );
   
   /* halt if something goes wrong */
   setExceptionHandler( notifyAndHalt );
   
   /* import the clarinet samples */
   if ( 0 != path )
   {
      sprintf( filename, "%s/clarinet.aiff", path );
   }
   else
   {
      sprintf( filename, "clarinet.aiff" );
   }
   printf( "importing clarinet samples\n" );
   N = importAiff( filename, samples, BUFSZ, &srate );
   
   /* analyze the clarinet */
   printf( "analyzing clarinet 4G#\n" );
   analyzer_configure( 415*.8, 415*1.6 );
   analyzer_setFreqDrift( 30 );
   analyzer_setAmpFloor( -80 );
   analyze( samples, N, srate, clar );
   
   /* channelize and distill */
   printf( "distilling\n" );
   reference = createFreqReference( clar, 0, 1000, 20 );
   channelize( clar, reference, 1 );
   distill( clar );
   destroyLinearEnvelope( reference );
   reference = 0;
   
   /* test SDIF import and export */
   N = partialList_size( clar );
   printf( "exporting %u partials to SDIF file\n", N );
   exportSdif( "clarinet.pi.sdif", clar );
   printf( "importing from SDIF file\n" );
   partialList_clear( clar );
   importSdif( "clarinet.pi.sdif", clar );
   if ( N != partialList_size( clar ) )
   {
      printf( "SDIF import yields a different number of "
              "partials than were exported!" );
      return 1;
   }
   
   /* shift pitch of clarinet partials */
   printf( "shifting pitch of clarinet partials down by 600 cents\n" );
   linearEnvelope_insertBreakpoint( pitchenv, 0, -600 );
   shiftPitch( clar, pitchenv );
   
   /* check clarinet synthesis */
   printf( "checking clarinet synthesis\n" );
   bzero( samples, BUFSZ*sizeof( double ) );
   synthesize( clar, samples, BUFSZ, srate );
   exportAiff( "clarOK.pi.aiff", samples, BUFSZ, srate, 16 );

   /* import the flute samples */
   if ( 0 != path )
   {
      sprintf( filename, "%s/flute.aiff", path );
   }
   else
   {
      sprintf( filename, "flute.aiff" );
   }
   printf( "importing flute samples\n" );
   N = importAiff( filename, samples, BUFSZ, &srate );
   
    /* analyze the flute */
   printf( "analyzing flute 4D\n" );
   analyzer_configure( 270, 270 );
   analyzer_setFreqDrift( 30 );
   analyze( samples, N, srate, flut );
   
    /* channelize and distill */
   printf( "distilling\n" );
   reference = createFreqReference( flut, 0, 1000, 20 );
   channelize( flut, reference, 1 );
   distill( flut );
   destroyLinearEnvelope( reference );
   reference = 0;

   /* check flute synthesis */
   printf( "checking flute synthesis\n" );
   bzero( samples, BUFSZ*sizeof( double ) );
   synthesize( flut, samples, BUFSZ, srate );
   exportAiff( "flutOK.pi.aiff", samples, BUFSZ, srate, 16 );
   
   /* dilate sounds */
   printf( "dilating sounds to match (%lf, %lf)\n", tgt_times[0], tgt_times[1] );
   printf( "clarinet times: (%lf, %lf)\n", clar_times[0], clar_times[1] );
   dilate( clar, clar_times, tgt_times, 2 );
   printf( "flute times: (%lf, %lf)\n", flute_times[0], flute_times[1] );
   dilate( flut, flute_times, tgt_times, 2 );
   
   /* perform morph */
   printf( "morphing clarinet with flute\n" );
   linearEnvelope_insertBreakpoint( morphenv, 0.6, 0 );
   linearEnvelope_insertBreakpoint( morphenv, 2, 1 );
   morph( clar, flut, morphenv, morphenv, morphenv, mrph );
   
   /* synthesize and export samples */
   printf( "synthesizing %lu morphed partials\n", 
           partialList_size( mrph ) );
   bzero( samples, BUFSZ*sizeof( double ) );
   synthesize( mrph, samples, BUFSZ, srate );
   exportAiff( "morph.pi.aiff", samples, BUFSZ, srate, 16 );
   
   printf( "Done, bye.\n\n" );
   return 0;
}

