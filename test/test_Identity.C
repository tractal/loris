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
 *	test_Identity.C
 *
 *	Verify that Loris synthesis/analysis is an (nearly) identity
 *  process. Analysis of samples rendered from synthetic Partials
 *  should yield Partials very much like the originals.
 *  The accuracy of the analysis will, of course, depend on the
 *  Analyzer parameters. The ones chosen here should easily pass the 
 *  tests given. 
 *
 * Kelly Fitz, 8 Dec 2004
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>

#include "Analyzer.h"
#include "Breakpoint.h"
#include "Channelizer.h"
#include "Distiller.h"
#include "FrequencyReference.h"
#include "Partial.h"
#include "PartialUtils.h"
#include "PartialList.h"
#include "Synthesizer.h"

#include "Exception.h"

using namespace std;
using namespace Loris;

const double pi = 3.14159265358979324;

//  tacky global error variable
int ERR = 0;
	
// #define VERBOSE

// #define FAIL_ON_ERROR
	
// --- helpers ---

//  It takes some kludging around to make sure that 
//  phases near 2pi are wrapped down near zero.
inline double mpi( double x )
{   
    const double EPS = .01;
    x += EPS;
	x = fmod( x, 2*pi );
	if ( x < 0 )
		x = x + (2*pi);
	return x - EPS;
}

inline double env( double tau )
{
	double t = 2*(tau-.4);
	return (.1+.3*exp( - pi * t * t ))*min( 500*tau, 1.0 );  
}

static void float_rel_equal( double x, double y, double pct )
{
	#ifdef VERBOSE
	cout << "\t" << x << " == " << y << " ?" << endl;
	#endif

    double eps = 0;
	if ( x != 0. )
	{
	    eps = pct * std::fabs(x);
	}
	
	if ( std::fabs(x-y) > eps )
	{
	    cout << "\t" << x << " != " << y << " within "
	         << pct*100 << "%" << endl;
	    ERR = 1;
	}
	
	#ifdef FAIL_ON_ERROR
	Assert( 0 == ERR );
	#endif
}

static void float_abs_equal( double x, double y, double eps )
{
	#ifdef VERBOSE
	cout << "\t" << x << " == " << y << " ?" << endl;
	#endif

	if ( std::fabs(x-y) > eps )
	{
	    cout << "\t" << x << " != " << y << " within "
	         << eps << endl;
	    ERR = 1;
	}

	#ifdef FAIL_ON_ERROR
	Assert( 0 == ERR );
	#endif
	
}

// ----------- one_partial -----------
//
static void one_partial( void )
{
    cout << "Single-partial analysis/synthesis identity check." << endl;
    
	// make a partial with 
	// slowly-varying frequency
	Partial p1;
	p1.insert( .1, Breakpoint( 375, .2, 0, 0 ) );
	p1.insert( .85, Breakpoint( 425, .2, 0, 0 ) );
	p1.insert( .85235, Breakpoint( 426.06, 0, 0, 0 ) );
	p1.insert( .899, Breakpoint( 500, 0, 0, pi ) );
	p1.insert( .9, Breakpoint( 500, .3, 0, 0 ) );
	p1.insert( 1.1, Breakpoint( 520, .3, 0, 0 ) );
	
	PartialUtils::fixPhaseAfter( p1, 0 );
	
	
	// synthesize the fake partial
	vector< double > v;
	Synthesizer synth( 44100, v );
	synth.synthesize( p1 );

	// analyze the synthesized partial
	Analyzer anal( 300, 400 );
	anal.setAmpFloor( -50 );
	anal.setBwRegionWidth( 0 );
    cout << "Using analysis window width " << anal.windowWidth() << " Hz" << endl;
	anal.analyze( v, 44100 );
	PartialList & partials = anal.partials();
	
	//  need to distill, because the fake partial fades out
	//  and back in again
	PartialList fake;
	fake.push_back( p1 );
	FrequencyReference ref( fake.begin(), fake.end(), 300, 600, 100 );
	Channelizer chan( ref, 1 );
	chan.channelize( partials.begin(), partials.end() );
	Distiller still( 0.001, 0.001 ); //  use 1 ms fade time for these tests
	still.distill( partials );
	
	if ( partials.size() != 1 )
	{
		cout << "ERROR: should find only one Partial" << endl;
	    ERR = 2;
	    return;
	}
	
	Partial a1 = partials.front();
	
	// compare parameters of the original and analyzed partials
	iostream::fmtflags flags = cout.flags();
	fixed( cout );
	streamsize prec = cout.precision();
	cout << setprecision(3);
	
	cout << "START TIMES (p1 a1) (testing within 3ms)" << endl;
	cout << p1.startTime() << "  " << a1.startTime() << endl;
	float_abs_equal( p1.startTime(), a1.startTime(), 0.003 );

	cout << "END TIMES (p1 a1) (testing within 3ms)" << endl;
	cout << p1.endTime() << "  " << a1.endTime() << endl;
	float_abs_equal( p1.endTime(), a1.endTime(), 0.003 );
	
	cout << "AMPLITUDES (time p1 a1) (testing within 2%)" << endl;
	const double dt = 0.042;
	double t = p1.startTime() + dt;
	while ( t <= p1.endTime() )
	{
		cout << t << "\t" << p1.amplitudeAt(t, 0.01) << "  " << a1.amplitudeAt(t,0.01) << endl;
		float_rel_equal( p1.amplitudeAt(t, 0.01), a1.amplitudeAt(t,0.01), 0.02 );
		t = t + dt;
	}
		
	cout << "FREQUENCIES (time p1 a1) (testing within 0.1 Hz)" << endl;
	t = p1.startTime() + dt;
	while ( t <= p1.endTime() )
	{
	    if ( p1.amplitudeAt(t) > 0 )
	    {
    		cout << t << "\t" << p1.frequencyAt(t) << "  " << a1.frequencyAt(t) << endl;
    		float_abs_equal( p1.frequencyAt(t), a1.frequencyAt(t), 0.1 );
        }
		t = t + dt;
	}
		
	cout << "PHASES / pi (time p1 a1) (testing within 1% of pi)" << endl;
	t = p1.startTime() + dt;
	while ( t <= p1.endTime() )
	{
	    if ( p1.amplitudeAt(t) > 0 )
	    {
    		cout << t << "\t" << mpi(p1.phaseAt(t))/pi << " " << mpi(a1.phaseAt(t))/pi << endl;
    		float_abs_equal( mpi(p1.phaseAt(t))/pi, mpi(a1.phaseAt(t))/pi, 0.01*pi );
        }
		t = t + dt;
	}
	
	cout << setprecision(prec);
	cout.flags( flags );
	cout << "Done." << endl;
}

// ----------- two_partials -----------
//
//  Note: it takes some planning to ensure that the evaluation
//  times do not coincide with the beginning and end of either
//  Partial. Abrupt turn-on and turn-off always corrupt the 
//  frequency and phase estimates at the ends of Partials, so
//  there is no point in testing the parameters very near the ends.
//  
static void two_partials( void )
{
    cout << "Two-partial analysis/synthesis identity check." << endl;
    
	// make a constant-amplitude partial with 
	// slowly-varying frequency
	Partial p1;
	p1.insert( .1, Breakpoint( 375, .2, 0, 0 ) );
	p1.insert( .875, Breakpoint( 425, .2, 0, 0 ) );
	
	// make another Partial with fancier envelopes
	Partial p2;	
	
    double f = 1000;
    double phi = 0;
    double t = 0;
    p2.insert( t + .225, Breakpoint( f, env( t ), 0, phi ) );

    double dt = 0.05;
    while ( t <= .75 )
    {
	    double fprev = f;
    	f = f + (dt * 300); //  - (env( t ) * 100)
	                        //  reassignment does chirps really, really well,
                            //  add this extra perturbation to mix things up
	                        //  a little -- still matches frequencies within 
	                        //  .1 Hz most of the time.
	    double favg = .5 * (f + fprev);
	    phi = phi + (2 * pi * favg * dt);
	    t = t + dt;
	    p2.insert( t + .225, Breakpoint( f, env( t ), 0, phi ) );
	}
	
	PartialList fake;
	fake.push_back( p1 );
	fake.push_back( p2 );
	
	// synthesize the fake partial
	vector< double > v;
	Synthesizer synth( 44100, v );
	synth.synthesize( fake.begin(), fake.end() );
	
	// analyze the synthesized partial
	Analyzer anal( 300, 400 );
	anal.setAmpFloor( -50 );
	anal.setBwRegionWidth( 0 );
    cout << "Using analysis window width " << anal.windowWidth() << " Hz" << endl;
	anal.analyze( v, 44100 );
	PartialList & partials = anal.partials();
	
	/*
	// no need to distill, these partials should pop right out
	// of the analysis
	FrequencyReference ref( fake.begin(), fake.end(), 300, 600, 100 );
	Channelizer chan( ref, 1 );
	chan.channelize( partials.begin(), partials.end() );
	Distiller still;
	still.distill( partials );
	*/
	if ( partials.size() != 2 )
	{
		cout << "ERROR: should find only two Partials" << endl;
	    ERR = 2;
	    return;
	}
	
	Partial a1 = partials.front();
	Partial a2 = partials.back();
	
	// compare parameters of the original and analyzed partials
	iostream::fmtflags flags = cout.flags();
    fixed( cout );
	streamsize prec = cout.precision();
	cout << setprecision(3);
	
	double hop = anal.hopTime() * 2; // turns on too fast for 2.5 ms
	cout << "START TIMES (p1 a1 p2 a2) (testing within " << hop*1000 << "ms)" << endl;
	cout << p1.startTime() << "  " << a1.startTime() << "\t"
	     << p2.startTime() << "  " << a2.startTime() << endl;
	float_abs_equal( p1.startTime(), a1.startTime(), hop );
	float_abs_equal( p2.startTime(), a2.startTime(), hop );

	cout << "END TIMES (p1 a1 p2 a2) (testing within " << hop*1000 << "ms)" << endl;
	cout << p1.endTime() << "  " << a1.endTime() << "\t"
	     << p2.endTime() << "  " << a2.endTime() << endl;
	float_abs_equal( p1.endTime(), a1.endTime(), hop );
	float_abs_equal( p2.endTime(), a2.endTime(), hop );
	
    const double tmin = min( p1.startTime(), p2.startTime() );
    const double tmax = max( p1.endTime(), p2.endTime() );


	cout << "AMPLITUDES (time p1 a1 p2 a2) (testing within 2%)" << endl;
	dt = 0.05;
	t = tmin + dt;
	while ( t < tmax - dt )
	{
		cout << t << "\t" << p1.amplitudeAt(t, 0.01) << "  " << a1.amplitudeAt(t,0.01) << "\t"
		     << p2.amplitudeAt(t, 0.01) << "  " << a2.amplitudeAt(t,0.01) << endl;
		float_rel_equal( p1.amplitudeAt(t, 0.01), a1.amplitudeAt(t,0.01), 0.02 );
		float_rel_equal( p2.amplitudeAt(t, 0.01), a2.amplitudeAt(t,0.01), 0.02 );
		t = t + dt;
	}
		
	cout << "FREQUENCIES (time p1 a1 p2 a2) (testing within 0.1 Hz)" << endl;
	t = tmin + dt;
	while ( t < tmax - dt )
	{
	    cout << t << "\t";
	    if ( p1.amplitudeAt(t) > 0 )
	    {
    		cout << p1.frequencyAt(t) << "  " << a1.frequencyAt(t) << "\t";
    		float_abs_equal( p1.frequencyAt(t), a1.frequencyAt(t), 0.1 );
        }
        else
        {
            cout << "\t\t\t";
        }
        
	    if ( p2.amplitudeAt(t) > 0 )
	    {
    		cout << p2.frequencyAt(t) << "  " << a2.frequencyAt(t);
    		float_abs_equal( p2.frequencyAt(t), a2.frequencyAt(t), 0.1 );
        }
        cout << endl;

		t = t + dt;
	}
		
	cout << "PHASES / pi (time p1 a1) (testing within 1% of pi)" << endl;
	t = tmin + dt;
	while ( t < tmax - dt )
	{   
	    cout << t << "\t";
	    if ( p1.amplitudeAt(t) > 0 )
	    {
    		cout << mpi(p1.phaseAt(t))/pi << "  " << mpi(a1.phaseAt(t))/pi << "\t";
    		float_abs_equal( mpi(p1.phaseAt(t))/pi, mpi(a1.phaseAt(t))/pi, 0.01*pi );
        }
        else
        {
            cout << "\t\t";
        }
        
	    if ( p2.amplitudeAt(t) > 0 )
	    {
    		cout << mpi(p2.phaseAt(t))/pi << "  " << mpi(a2.phaseAt(t))/pi;
    		float_abs_equal( mpi(p2.phaseAt(t))/pi, mpi(a2.phaseAt(t))/pi, 0.01*pi );
        }
        cout << endl;        

		t = t + dt;
	}
	
	cout << setprecision(prec);
	cout.flags( flags );
	cout << "Done." << endl;
}


// ----------- main -----------
//
int main( void )
{
    std::cout << "Identity test for Loris analysis and synthesis." << endl;
	std::cout << "Renders synthetic Partials, analyzes the samples," << endl;
	std::cout << "and compares the analyzed Partials to the originals." << endl << endl;
	std::cout << "Built: " << __DATE__ << endl << endl;
	
	try 
	{
		one_partial();
		two_partials();
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
    
    if ( 0 == ERR )
    {
    	cout << "Passed identity tests." << endl;
    }
    else
    {
    	cout << "FAILED identity tests." << endl;
    }
    return ERR;
}