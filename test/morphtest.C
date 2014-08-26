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
 *	morphtest.C
 *
 *	Very simple Loris instrument tone morphing demonstration using 
 *	the C++ interface to Loris.
 *
 * Kelly Fitz, 7 Dec 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "AiffFile.h"
#include "Analyzer.h"
#include "BreakpointEnvelope.h"
#include "Channelizer.h"
#include "Dilator.h"
#include "Distiller.h"
#include "LorisExceptions.h"
#include "FrequencyReference.h"
#include "Morpher.h"
#include "Notifier.h"
#include "Partial.h"
#include "PartialList.h"
#include "PartialUtils.h"
#include "SdifFile.h"

#include "loris.h"  // just for version string

#include <iostream>
#include <list>
#include <cstdlib>
#include <string>
#include <stdexcept>

#define ESTIMATE_F0 1
#define ESTIMATE_AMP 0

using std::cout;
using std::endl;

using namespace Loris;

int main( void )
{
	cout << "Loris C++ API test, using " << LORIS_VERSION_STR << endl;
	cout << "Kelly Fitz 2006" << endl << endl;
	cout << "Generates a simple linear morph between a " << endl;
	cout << "clarinet and a flute using the C++ library." << endl << endl;
	
	std::string path(""); 
	if ( std::getenv("srcdir") ) 
	{
		path = std::getenv("srcdir");
		path = path + "/";
	}

	try 
	{
        //	analyze clarinet tone
        cout << "importing clarinet samples" << endl;
        Analyzer a( 415*.8, 415*1.6 );
        AiffFile f( path + "clarinet.aiff" );

        // analyze the clarinet 
        cout << "analyzing clarinet 4G#" << endl;
        a.analyze( f.samples(), f.sampleRate() );
        PartialList clar = a.partials();

        // channelize and distill
        cout << "distilling" << endl;
        FrequencyReference clarRef( clar.begin(), clar.end(), 415*.8, 415*1.2, 50 );
        Channelizer::channelize( clar, clarRef , 1 );		
        Distiller::distill( clar, 0.001 );

        //	test SDIF import and export
        cout << "exporting " << clar.size() << " partials to SDIF file" << endl;
        SdifFile::Export( "clarinet.ctest.sdif", clar );
        cout << "importing from SDIF file" << endl;
        SdifFile ip("clarinet.ctest.sdif");
        if ( clar.size() != ip.partials().size() )
        {
        	throw std::runtime_error( "SDIF import yields a different number of partials than were exported!" );
        }
        clar = ip.partials();

        // shift pitch of clarinet partials
        cout << "shifting pitch of " << clar.size() << " Partials by 600 cents" << endl;
        PartialUtils::shiftPitch( clar.begin(), clar.end(), -600 );

        // check clarinet synthesis
        cout << "checking clarinet synthesis" << endl;
        AiffFile clarout( clar.begin(), clar.end(), f.sampleRate() );
        clarout.write( "clarOK.ctest.aiff" );

        //	analyze flute tone
        cout << "importing flute samples" << endl;
        f = AiffFile( path + "flute.aiff" );

        // analyze the flute
        cout << "analyzing flute 4D" << endl;
        a = Analyzer( 270 );
#if defined(ESTIMATE_F0) && ESTIMATE_F0
		cout << "Analyzer will build a fundamental frequency estimate for the flute" << endl;
        a.buildFundamentalEnv( 270, 310 );
#endif
#if defined(ESTIMATE_AMP) && ESTIMATE_AMP
		a.buildAmpEnv( true );
#endif
        a.analyze( f.samples(), f.sampleRate() );
        PartialList flut = a.partials();

        // channelize and distill
        cout << "distilling" << endl;
#if defined(ESTIMATE_F0) && ESTIMATE_F0
		const LinearEnvelope & flutRef = a.fundamentalEnv();	
		double est_time = flutRef.begin()->first;
		cout << "flute fundamental envelope starts at time " << est_time << endl;
		while ( est_time < 2 )
		{
			cout << "flute fundamental estimate at time " 
				 << est_time << " is " << flutRef.valueAt( est_time ) << endl;
			est_time += 0.35;
		}		
#else        
        FrequencyReference flutRef( flut.begin(), flut.end(), 291*.8, 291*1.2, 50 );
#endif
        Channelizer::channelize( flut, flutRef, 1 );
        Distiller::distill( flut, 0.001 );
        cout << "obtained " << flut.size() << " distilled flute Partials" << endl;

#if defined(ESTIMATE_F0) && ESTIMATE_F0
#if defined(ESTIMATE_AMP) && ESTIMATE_AMP
		//  generate a sinusoid that tracks the fundamental
		//	and amplitude envelopes obtained during analysis
		cout << "synthesizing sinusoid from flute amp and fundamental estimates" << endl;
		Partial boo;
		LinearEnvelope fund = a.fundamentalEnv();
		LinearEnvelope::iterator it;
		for ( it = fund.begin(); it != fund.end(); ++it )
		{
			Breakpoint bp( it->second, a.ampEnv().valueAt( it->first ), 0, 0 );
			boo.insert( it->first, bp );
		}
		
		PartialList boolist;
		boolist.push_back( boo );
		AiffFile boofile( boolist.begin(), boolist.end(), 44100 );
		boofile.write( "flutefundamental.aiff" );

#endif
#endif

        cout << "exporting " << flut.size() << " partials to SDIF file" << endl;
        SdifFile::Export( "flute.ctest.sdif", flut );

        // check flute synthesis:
        cout << "checking flute synthesis" << endl;
        AiffFile flutout( flut.begin(), flut.end(), f.sampleRate() );
        flutout.write( "flutOK.ctest.aiff" );
        	
        // perform temporal dilation
        double flute_times[] = { 0.4, 1. };
        double clar_times[] = { 0.2, 1. };
        double tgt_times[] = { 0.3, 1.2 };

        cout << "dilating sounds to match (" << tgt_times[0] << ", " 
           << tgt_times[1] << ")" << endl;
        cout << "flute times: (" << flute_times[0] << ", " 
           << flute_times[1] << ")" << endl;
        Dilator::dilate(  flut.begin(), flut.end() , flute_times, flute_times+2, tgt_times );

        cout << "clarinet times: (" << clar_times[0] << ", " 
           << clar_times[1] << ")" << endl;
        Dilator::dilate(  clar.begin(), clar.end(), clar_times, clar_times+2, tgt_times );			

        // perform morph
        cout << "morphing flute and clarinet" << endl;
        BreakpointEnvelope mf;
        mf.insertBreakpoint( 0.6, 0 );
        mf.insertBreakpoint( 2, 1 );

        Morpher m( mf );
        m.setMinBreakpointGap( 0.002 );
        m.setSourceReferencePartial( clar, 3 );
        m.setTargetReferencePartial( flut, 1 );
        m.morph( clar.begin(), clar.end(), flut.begin(), flut.end() );


        // synthesize and export samples
        cout << "synthesizing " << m.partials().size() << " morphed partials" << endl;
        AiffFile morphout( m.partials().begin(), m.partials().end(), f.sampleRate() );
        morphout.write( "morph.ctest.aiff" );

	}
	catch( Exception & ex ) 
	{
		cout << "Caught Loris exception: " << ex.what() << endl;
		return 1;
	}
	catch( std::exception & ex ) 
	{
		cout << "Caught std C++ exception: " << ex.what() << endl;
		return 1;
	}

	cout << "Done, bye." << endl;
	return 0;
}
