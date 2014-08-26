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
 *	test_Synthesizer.C
 *
 *	Unit tests for Loris Synthesizer class.
 *
 *
 * Kelly Fitz, 23 Jan 2010
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "Partial.h"
#include "Exception.h"
#include "SdifFile.h"
#include "Synthesizer.h"

#include <cmath>
#include <iostream>
#include <vector>
#include <cstdlib>

using namespace Loris;
using namespace std;

const double Pi = 3.14159265358979324;


// --- macros ---

//	define this to see pages and pages of spew
// #define VERBOSE
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
	
#define EPSILON 0.000030518 // 16-bit sample resolution	    
	
static bool sample_equal( double x, double y )
{
	#ifdef VERBOSE
	cout << "\t" << x << " == " << y << " ?" << endl;
	#endif
	// #define EPSILON 0.000030518 // 16-bit sample resolution
	return std::fabs(x-y) < EPSILON;
}

#define SAME_SAMP_VALUES(x,y) TEST( sample_equal((x),(y)) )

// ----------- test_synth_phase -----------
//
static void test_synth_phase( void )
{
	cout << "\t--- testing synthesis phase accuracy using a single Partial... ---\n\n";

	//	Import the test Partial:
	std::string path(""); 
	if ( std::getenv("srcdir") ) 
	{
		path = std::getenv("srcdir");
		path = path + "/";
	}
	
	SdifFile f( path + "one_synth_phase_test.sdif" );
	Partial p1 = f.partials().front();
	
	vector< double > v;
	const double fs = 44100;
	Synthesizer syn( fs, v );
	
	cout << "rendering" << endl;
    syn.synthesize( p1 );
	
	unsigned int N = v.size();
	
	unsigned int count_errs = 0;
	
	unsigned int nstart = 1 + (p1.startTime() * fs);
	unsigned int nend = (p1.endTime() * fs);
	for ( unsigned int n = nstart; n < nend; ++n )
	{
	    double t = n / fs;
	    double at = p1.amplitudeAt( t );
	    double pht = p1.phaseAt( t );
	    double precise = at * cos( pht );

	    double moo = v[n];
	    	    
	    if ( ! sample_equal(moo,precise) )
	    {
	        ++count_errs;
	    }	    
    }
	
    cout << count_errs << " sample errors larger than 16-bit resolution" << endl;    	
}

// ----------- main -----------
//
int main( )
{
	std::cout << "Unit test for Synthesizer class." << endl;
	std::cout << "Relies on Partial and SdifFile." << endl << endl;
	std::cout << "Built: " << __DATE__ << endl << endl;
	
	try 
	{
		test_synth_phase();
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
	cout << "Synthesizer passed all tests." << endl;
	return 0;
}


