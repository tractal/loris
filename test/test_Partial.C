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
 *	test_Partial.C
 *
 *	Unit tests for Partial class. Relies on Breakpoint,
 *	Partial::iterator, and Loris Exceptions. Build with
 *	Partial.C, Breakpoint.C, Exception.C, and 
 *	Notifier.C.
 *
 * Kelly Fitz, 15 April 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "Partial.h"
#include "Exception.h"

#include <cmath>
#include <iostream>

using namespace Loris;
using namespace std;

const double Pi = 3.14159265358979324;


// --- macros ---

//	define this to see pages and pages of spew
//#define VERBOSE
#ifdef VERBOSE									
	#define TEST(invariant)									\
		do {													\
			std::cout << "TEST: " << #invariant << endl;		\
			Assert( invariant );								\
			std::cout << " PASS" << endl << endl;			\
		} while (false)
	
	#define TEST_VALUE( expr, val )									\
		do {															\
			std::cout << "TEST: " << #expr << "==" << (val) << endl;\
			Assert( (expr) == (val) );								\
			std::cout << "  PASS" << endl << endl;					\
		} while (false)
#else
	#define TEST(invariant)					\
		do {									\
			Assert( invariant );				\
		} while (false)
	
	#define TEST_VALUE( expr, val )			\
		do {									\
			Assert( (expr) == (val) );		\
		} while (false)
#endif	
	
static bool float_equal( double x, double y )
{
	#ifdef VERBOSE
	cout << "\t" << x << " == " << y << " ?" << endl;
	#endif
	#define EPSILON .0000001
	if ( std::fabs(x) > 0. )
		return std::fabs((x-y)/x) < EPSILON;
	else
		return std::fabs(x-y) < EPSILON;
}

#define SAME_PARAM_VALUES(x,y) TEST( float_equal((x),(y)) )

inline double m2pi( double x )
{   
    const double EPS = .01;
    x += EPS;
	x = fmod( x, 2*Pi );
	if ( x < 0 )
		x = x + (2*Pi);
	return x - EPS;
}

#define SAME_PHASE_VALUES(x,y) SAME_PARAM_VALUES(m2pi(x),m2pi(y))

// ----------- test_parametersAt -----------
//
static void test_parametersAt( void )
{
	std::cout << "\t--- testing Partial::parameterAt members... ---\n\n";

	//	Fabricate a Partial, and verify that parameter estimation works: 
	Partial p1;
	const int NUM_BPTS = 3;
	const double P1_TIMES[] = {0.2, .8, 1.0};
	const double P1_FREQS[] = {100, 100, 120};
	const double P1_AMPS[] = {.2, .2, .4};
	const double P1_BWS[] = {0, 0, .2};			
	const double P1_PHS[] = {.8, .8, .8}; // std::fmod( .8 + (2 * Pi * (0.2*110)), 2. * Pi )};
		
	for (int i = 0; i < NUM_BPTS; ++i )
	{
		p1.insert( P1_TIMES[i], Breakpoint( P1_FREQS[i], P1_AMPS[i], P1_BWS[i], P1_PHS[i] ) );
	}
	
	// check parameters at t = 0.2
	double t = 0.2;
	SAME_PARAM_VALUES( p1.frequencyAt(t), P1_FREQS[0] );
	SAME_PARAM_VALUES( p1.amplitudeAt(t), P1_AMPS[0] );
	SAME_PARAM_VALUES( p1.bandwidthAt(t), P1_BWS[0] );
	SAME_PHASE_VALUES( p1.phaseAt(t), P1_PHS[0] );
	SAME_PARAM_VALUES( p1.parametersAt(t).frequency(), P1_FREQS[0] );
	SAME_PARAM_VALUES( p1.parametersAt(t).amplitude(), P1_AMPS[0] );
	SAME_PARAM_VALUES( p1.parametersAt(t).bandwidth(), P1_BWS[0] );
	SAME_PHASE_VALUES( p1.parametersAt(t).phase(), P1_PHS[0] );

	// check parameters at t = 0.8
	t = 0.8;
	SAME_PARAM_VALUES( p1.frequencyAt(t), P1_FREQS[1] );
	SAME_PARAM_VALUES( p1.amplitudeAt(t), P1_AMPS[1] );
	SAME_PARAM_VALUES( p1.bandwidthAt(t), P1_BWS[1] );
	SAME_PHASE_VALUES( p1.phaseAt(t), P1_PHS[1] );
	SAME_PARAM_VALUES( p1.parametersAt(t).frequency(), P1_FREQS[1] );
	SAME_PARAM_VALUES( p1.parametersAt(t).amplitude(), P1_AMPS[1] );
	SAME_PARAM_VALUES( p1.parametersAt(t).bandwidth(), P1_BWS[1] );
	SAME_PHASE_VALUES( p1.parametersAt(t).phase(), P1_PHS[1] );

	// check parameters at t = 1.0
	t = 1.0;
	SAME_PARAM_VALUES( p1.frequencyAt(t), P1_FREQS[2] );
	SAME_PARAM_VALUES( p1.amplitudeAt(t), P1_AMPS[2] );
	SAME_PARAM_VALUES( p1.bandwidthAt(t), P1_BWS[2] );
	SAME_PHASE_VALUES( p1.phaseAt(t), P1_PHS[2] );
	SAME_PARAM_VALUES( p1.parametersAt(t).frequency(), P1_FREQS[2] );
	SAME_PARAM_VALUES( p1.parametersAt(t).amplitude(), P1_AMPS[2] );
	SAME_PARAM_VALUES( p1.parametersAt(t).bandwidth(), P1_BWS[2] );
	SAME_PHASE_VALUES( p1.parametersAt(t).phase(), P1_PHS[2] );

	// check parameters at t = 0.1
	t = 0.1;
	SAME_PARAM_VALUES( p1.frequencyAt(t), P1_FREQS[0] );
	SAME_PARAM_VALUES( p1.amplitudeAt(t), 0 );
	SAME_PARAM_VALUES( p1.bandwidthAt(t), P1_BWS[0] );
	// no phase change, exactly ten periods
	SAME_PHASE_VALUES( p1.phaseAt(t), P1_PHS[0] );
	SAME_PARAM_VALUES( p1.parametersAt(t).frequency(), P1_FREQS[0] );
	SAME_PARAM_VALUES( p1.parametersAt(t).amplitude(), 0 );
	SAME_PARAM_VALUES( p1.parametersAt(t).bandwidth(), P1_BWS[0] );
	SAME_PHASE_VALUES( p1.parametersAt(t).phase(), P1_PHS[0] );
	
	// check parameters at t = 0.9
	t = 0.9;
	SAME_PARAM_VALUES( p1.frequencyAt(t), .5 * (P1_FREQS[1] + P1_FREQS[2]) );
	SAME_PARAM_VALUES( p1.amplitudeAt(t), .5 * (P1_AMPS[1] + P1_AMPS[2]) );
	SAME_PARAM_VALUES( p1.bandwidthAt(t), .5 * (P1_BWS[1] + P1_BWS[2]) );
	// .1 s at avg 105 Hz equals 10.5 periods, half a period (Pi)
	// different from phase at 0.8
	SAME_PHASE_VALUES( p1.phaseAt(t), P1_PHS[1] + Pi );
	SAME_PARAM_VALUES( p1.parametersAt(t).frequency(), .5 * (P1_FREQS[1] + P1_FREQS[2]) );
	SAME_PARAM_VALUES( p1.parametersAt(t).amplitude(), .5 * (P1_AMPS[1] + P1_AMPS[2]) );
	SAME_PARAM_VALUES( p1.parametersAt(t).bandwidth(), .5 * (P1_BWS[1] + P1_BWS[2]) );
	SAME_PHASE_VALUES( p1.parametersAt(t).phase(), P1_PHS[1] + Pi );

	// check parameters at t = 1.1
	t = 1.1;
	SAME_PARAM_VALUES( p1.frequencyAt(t), P1_FREQS[2] );
	SAME_PARAM_VALUES( p1.amplitudeAt(t), 0 );
	SAME_PARAM_VALUES( p1.bandwidthAt(t), P1_BWS[2] );
	// no phase change, exactly eleven periods
	SAME_PHASE_VALUES( p1.phaseAt(t), P1_PHS[2] );
	SAME_PARAM_VALUES( p1.parametersAt(t).frequency(), P1_FREQS[2] );
	SAME_PARAM_VALUES( p1.parametersAt(t).amplitude(), 0 );
	SAME_PARAM_VALUES( p1.parametersAt(t).bandwidth(), P1_BWS[2] );
	SAME_PHASE_VALUES( p1.parametersAt(t).phase(), P1_PHS[2] );
}

// ----------- test_absorb -----------
//
static void test_absorb( void )
{
	std::cout << "\t--- testing Partial::absorb... ---\n\n";

	//	Fabricate two Partials, and the correct result of aborbing
	//	one into the other, verify that abosrb works: 
	Partial p1, p2;
	const int NUM_BPTS = 3;
	const double P1_TIMES[] = {0, .8, 1};
	const double P1_FREQS[] = {180, 180, 180};
	const double P1_AMPS[] = {.2, .2, .4};
	const double P1_BWS[] = {0, 0, .2};			
	const double P1_PHS[] = {.8, .8, -1.2}; 	
		
	for (int i = 0; i < NUM_BPTS; ++i )
		p1.insert( P1_TIMES[i], Breakpoint( P1_FREQS[i], P1_AMPS[i], P1_BWS[i], P1_PHS[i] ) );
		
	const double P2_TIMES[] = {.2, .5, 1};
	const double P2_FREQS[] = {200, 200, 200};
	const double P2_AMPS[] = {.1, .6, .2};		
	const double P2_BWS[] = {.9, .1, .1};		
	const double P2_PHS[] = {0, 0, 0}; 
	
	for (int i = 0; i < NUM_BPTS; ++i )
		p2.insert( P2_TIMES[i], Breakpoint( P2_FREQS[i], P2_AMPS[i], P2_BWS[i], P2_PHS[i] ) );
		
	//	the fused Partial should have Breakpoints at the same times
	//	and frequencies as the absorbing Partial (p1):
	Partial fuse_by_hand;
	Partial::iterator it = p1.begin();
	while ( it != p1.end() )
	{
		double t = it.time();
		double f = p1.frequencyAt(t);
		
		double e1 = p1.amplitudeAt(t) * p1.amplitudeAt(t);
		double e2 = p2.amplitudeAt(t) * p2.amplitudeAt(t);
		
		// the fused amplitude is the square root of the
		// total energy:
		double a = sqrt( e1 + e2 );
		
		// the fused bandwidth is the ratio of the noise energy
		// to total energy, the noise energy is the noise energy
		// in p1 added to the energy in p2:
		double bw = ((e1 * p1.bandwidthAt(t)) + e2) / (e1 + e2);
		
		double ph = p1.phaseAt(t);
		fuse_by_hand.insert( t, Breakpoint( f, a, bw, ph ) );
		
		++it;
	}
	
	// now absorb p2 into a copy of p1:
	Partial fused = p1;
	fused.absorb( p2 );

	//	check:
	TEST( fused.numBreakpoints() == fuse_by_hand.numBreakpoints() );

	SAME_PARAM_VALUES( fused.startTime(), fuse_by_hand.startTime() );
	SAME_PARAM_VALUES( fused.endTime(), fuse_by_hand.endTime() );
	SAME_PARAM_VALUES( fused.duration(), fuse_by_hand.duration() );
	
	SAME_PARAM_VALUES( fused.frequencyAt(0), fuse_by_hand.frequencyAt(0) );
	SAME_PARAM_VALUES( fused.amplitudeAt(0), fuse_by_hand.amplitudeAt(0) );
	SAME_PARAM_VALUES( fused.bandwidthAt(0), fuse_by_hand.bandwidthAt(0) );
	SAME_PARAM_VALUES( fused.phaseAt(0), fuse_by_hand.phaseAt(0) );
	
	SAME_PARAM_VALUES( fused.frequencyAt(0.1), fuse_by_hand.frequencyAt(0.1) );
	SAME_PARAM_VALUES( fused.amplitudeAt(0.1), fuse_by_hand.amplitudeAt(0.1) );
	SAME_PARAM_VALUES( fused.bandwidthAt(0.1), fuse_by_hand.bandwidthAt(0.1) );
	SAME_PARAM_VALUES( fused.phaseAt(0.1), fuse_by_hand.phaseAt(0.1) );
	
	SAME_PARAM_VALUES( fused.frequencyAt(0.3), fuse_by_hand.frequencyAt(0.3) );
	SAME_PARAM_VALUES( fused.amplitudeAt(0.3), fuse_by_hand.amplitudeAt(0.3) );
	SAME_PARAM_VALUES( fused.bandwidthAt(0.3), fuse_by_hand.bandwidthAt(0.3) );
	SAME_PARAM_VALUES( fused.phaseAt(0.3), fuse_by_hand.phaseAt(0.3) );
	
	SAME_PARAM_VALUES( fused.frequencyAt(0.6), fuse_by_hand.frequencyAt(0.6) );
	SAME_PARAM_VALUES( fused.amplitudeAt(0.6), fuse_by_hand.amplitudeAt(0.6) );
	SAME_PARAM_VALUES( fused.bandwidthAt(0.6), fuse_by_hand.bandwidthAt(0.6) );
	SAME_PARAM_VALUES( fused.phaseAt(0.6), fuse_by_hand.phaseAt(0.6) );
	
	SAME_PARAM_VALUES( fused.frequencyAt(0.85), fuse_by_hand.frequencyAt(0.85) );
	SAME_PARAM_VALUES( fused.amplitudeAt(0.85), fuse_by_hand.amplitudeAt(0.85) );
	SAME_PARAM_VALUES( fused.bandwidthAt(0.85), fuse_by_hand.bandwidthAt(0.85) );
	SAME_PARAM_VALUES( fused.phaseAt(0.85), fuse_by_hand.phaseAt(0.85) );
	
	SAME_PARAM_VALUES( fused.frequencyAt(1), fuse_by_hand.frequencyAt(1) );
	SAME_PARAM_VALUES( fused.amplitudeAt(1), fuse_by_hand.amplitudeAt(1) );
	SAME_PARAM_VALUES( fused.bandwidthAt(1), fuse_by_hand.bandwidthAt(1) );
	SAME_PARAM_VALUES( fused.phaseAt(1), fuse_by_hand.phaseAt(1) );
}

// ----------- test_split -----------
//
static void test_split( void )
{
	std::cout << "\t--- testing Partial::split... ---\n\n";

	//	Fabricate a Partial, split it, and verify that
	//	the two resulting Partials do not overlap, and
	//	that they have the same Breakpoints as the original,
	//	divided between them.
	Partial original;
	const int NUM_BPTS = 4;
	const double P1_TIMES[] = {.2, .4, .7, .9};
	const double P1_FREQS[] = {180, 150, 180, 170};
	const double P1_AMPS[] = {.2, .25, .4, .3};
	const double P1_BWS[] = {0, .1, .2, .3};			
	const double P1_PHS[] = {-.8, .8, -1.2, .8}; 	
	
	for (int i = 0; i < NUM_BPTS; ++i )
		original.insert( P1_TIMES[i], Breakpoint( P1_FREQS[i], P1_AMPS[i], P1_BWS[i], P1_PHS[i] ) );
	
	Partial p1 = original;
	// split into two Partials, two Breakpoints each:
	Partial p2 = p1.split( p1.findNearest( 0.6 ) ); 
	
	// verify the number of Breakpoints
	TEST( p1.numBreakpoints() == 2 );
	TEST( p2.numBreakpoints() == 2 );
	TEST( p1.numBreakpoints() + p2.numBreakpoints() == original.numBreakpoints() );

	// verify that the two do not overlap:
	TEST( p1.endTime() < p2.startTime() );
	
	// verify that the Breakpoints are the same:
	Partial::iterator it = p1.begin();
	while ( it != p1.end() )
	{
		Breakpoint & p1Breakpoint = *it;
		Partial::iterator origit = original.findNearest(it.time());
		Breakpoint & origBreakpoint = *origit;
		
		SAME_PARAM_VALUES( it.time(), origit.time() );
		SAME_PARAM_VALUES( p1Breakpoint.frequency(), origBreakpoint.frequency() );
		SAME_PARAM_VALUES( p1Breakpoint.amplitude(), origBreakpoint.amplitude() );
		SAME_PARAM_VALUES( p1Breakpoint.bandwidth(), origBreakpoint.bandwidth() );
		SAME_PARAM_VALUES( p1Breakpoint.phase(), origBreakpoint.phase() );
		
		++it;
	}
	
	it = p2.begin();
	while ( it != p2.end() )
	{
		Breakpoint & p2Breakpoint = *it;
		Partial::iterator origit = original.findNearest(it.time());
		Breakpoint & origBreakpoint = *origit;
		
		SAME_PARAM_VALUES( it.time(), origit.time() );
		SAME_PARAM_VALUES( p2Breakpoint.frequency(), origBreakpoint.frequency() );
		SAME_PARAM_VALUES( p2Breakpoint.amplitude(), origBreakpoint.amplitude() );
		SAME_PARAM_VALUES( p2Breakpoint.bandwidth(), origBreakpoint.bandwidth() );
		SAME_PARAM_VALUES( p2Breakpoint.phase(), origBreakpoint.phase() );
		
		++it;
	}
}

// ----------- main -----------
//
int main( )
{
	std::cout << "Unit test for Partial class." << endl;
	std::cout << "Relies on Breakpoint and Partial::iterator." << endl << endl;
	std::cout << "Built: " << __DATE__ << endl << endl;
	
	try 
	{
		test_parametersAt();
		test_absorb();
		test_split();
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
	
	//	return successfully
	cout << "Partial passed all tests." << endl;
	return 0;
}


