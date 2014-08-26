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
 *  test_Fundamental.C
 *
 *  Unit tests for Distiller class. Relies on Breakpoint,
 *  Partial, PartialList and Loris Exceptions. Build with
 *  Partial.C, Breakpoint.C, Exception.C, Distiller.C 
 *  and Notifier.C.
 *
 * Kelly Fitz, 29 Mar 2008
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "AiffFile.h"
#include "Analyzer.h"
#include "LinearEnvelope.h"
#include "Fundamental.h"
#include "Partial.h"
#include "PartialList.h"
#include "SdifFile.h"

#include "loris.h"  // just for version string

using namespace Loris;

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

#include <cstdlib>
using std::atof;

// ---------------------------------------------
//  importSamples
// ---------------------------------------------
//  Read samples from the file at the specified 
//  path and store them in buffer.
//
//  If srate is non-null, store the sample rate
//  there.
//
void
importSamples( const string & path, 
               vector< double > & buffer, 
               double * srate = 0 )
{   
    AiffFile fin( path );
    
    buffer = fin.samples();
	
    if ( 0 != srate )
    {
        *srate = fin.sampleRate();
    }
}

// ---------------------------------------------
//  dumpEnvelope
// ---------------------------------------------
//  Dump envelope statistics, return the mean.

double dumpEnvelope( const LinearEnvelope & freq )
{
    cout << "dumping envelope" << endl;

    unsigned int N = freq.size();
    double sum = 0;
    
    for ( LinearEnvelope::const_iterator it = freq.begin();
          it != freq.end();
          ++it )
    {
        double t = it->first;
        double f = it->second;
        
        cout << "time: " << t << "\t value: " << f << "\n";
        
        sum += f;
    }

    double mean = sum / (double)N;
    cout << "mean: " << mean << endl;

    return mean;
}

// ---------------------------------------------
//  main
// ---------------------------------------------
//  Perform the following three analyses:
//  1) Loris analysis of imported sound samples, 
//      retaining the fundamental estimate
//      captured during analysis.
//  2) fundamental estimation from the Partials
//      created in step 1
//  3) fundamental estimation from the imported
//      sound samples analyzed in step 1


int main( int argc, char * argv[] )
{
    std::cout << "Unit test for fundamental estimation functions." << endl;
    std::cout << "Tests FundamentalFromPartials and FundamentalFromSamples." << endl << endl;
    std::cout << "Relies on AiffFile, Analyzer, Partial, PartialList, and LinearEnvelope." << endl << endl;
    std::cout << "Built: " << __DATE__ << endl << endl;

	string path(""); 
	if ( std::getenv("srcdir") ) 
	{
		path = std::getenv("srcdir");
		path = path + "/";
	}
    
    //  --- parameters that are sample-specific ---
    string fname = path + "clarinet.aiff";
    double fmin = 200;
    double fmax = 500;
    double res = 415*.8;
    double win = 415*1.6;
    double interval = 0.05;  //  50 ms
    
    const double approx = 414;
    
    double x;
    
    try
        {
        //  import (mono) samples        
        std::vector< double > buf;
        double rate;
        importSamples( fname, buf, &rate );


        //  step 1. analyze the samples    
        Analyzer anal( res, win );
        anal.buildFundamentalEnv( fmin, fmax );
        anal.setHopTime( interval );
        cout << "--- step 1 analyzer ---" << endl;
        cout << "analysis resolution is " << anal.freqResolution() << endl;
        cout << "window width is " << anal.windowWidth() << endl;
        cout << "amplitude threshold is " << anal.ampFloor() << endl;    
        cout << "lower bound is " << fmin << endl;
        cout << "upper bound is " << fmax << endl;
        
        anal.analyze( buf, rate );
        PartialList plist = anal.partials();
        LinearEnvelope est1 = anal.fundamentalEnv();

        x = dumpEnvelope( est1 );
        if ( (approx-1) > x || (approx+1) < x )
        {
            throw std::runtime_error( "that isn't right" );
        }
        
        double tbeg = est1.begin()->first;
        double tend = (--est1.end())->first;
        
        //  step 2. estimate fundamental from Partials
        FundamentalFromPartials eparts;
        eparts.setAmpFloor( -65 );
        eparts.setAmpRange( 40 );
        eparts.setFreqCeiling( 5000 );
        
        cout << "--- step 2 fundamental estimator from partials ---" << endl;
        cout << "amplitude threshold is " << eparts.ampFloor() << endl;
        cout << "amplitude range is " << eparts.ampRange() << endl;
        cout << "frequency ceiling is " << eparts.freqCeiling() << endl;
        cout << "precision level is " << eparts.precision() << " Hz" << endl;
        
        LinearEnvelope est2 = eparts.buildEnvelope( plist, tbeg, tend, 
                                                    interval, fmin, fmax, 0.95 );
        x = dumpEnvelope( est2 );
        if ( (approx-1) > x || (approx+1) < x )
        {
            throw std::runtime_error( "that isn't right" );
        }
                
        //  step 3. estimate fundamental from the samples    
        FundamentalFromSamples esamps( win );
        esamps.setAmpFloor( -65 );
        esamps.setAmpRange( 40 );
        esamps.setFreqCeiling( 5000 );
        
        cout << "--- step 3 fundamental estimator from samples ---" << endl;
        cout << "window width is " << esamps.windowWidth() << endl;
        cout << "amplitude threshold is " << esamps.ampFloor() << endl;
        cout << "amplitude range is " << esamps.ampRange() << endl;
        cout << "frequency ceiling is " << esamps.freqCeiling() << endl;
        cout << "precision level is " << esamps.precision() << " Hz" << endl;

        LinearEnvelope est3 = esamps.buildEnvelope( buf, rate, tbeg, tend, 
                                                    interval, fmin, fmax, 0.95 );
        x = dumpEnvelope( est3 );
        if ( (approx-1) > x || (approx+1) < x )
        {
            throw std::runtime_error( "that isn't right" );
        }
        
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
    
    //  return successfully
    cout << "Fundamental estimation passed all tests." << endl;
    return 0;

    return 0;
}