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
 *	test_Resampler.C
 *
 *	Unit tests for Loris PartialList resampling operations.
 *
 *
 * Kelly Fitz, 14 Feb 2010
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
#include "Partial.h"
#include "Resampler.h"
#include "Exception.h"
#include "LinearEnvelope.h"


#include <cmath>
#include <iostream>

using namespace Loris;
using namespace std;

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
	
	
static bool float_equal( double x, double y )
{
    #ifdef VERBOSE
    cout << "\t" << x << " == " << y << " ?" << endl;
    #endif
    #define EPSILON .0000001
    if ( std::fabs(x) > EPSILON*EPSILON )
        return std::fabs((x-y)/x) < EPSILON;
    else
        return std::fabs(x-y) < EPSILON;
}

#define TEST_SAME_TIMES(x,y) TEST( float_equal((x),(y)) )
#define TEST_SAME_FLOAT(x,y) TEST( float_equal((x),(y)) )

// ----------- test_dense_resample -----------
//
static void test_dense_resample( void )
{
	cout << "\t--- testing dense resampling of a Partial... ---\n\n";
		
	//  build some Partials
    Partial p1;
    p1.insert( 0.45, Breakpoint( 100, 0.1, 0, 0 ) );
    p1.insert( 1.45, Breakpoint( 200, 0.1, 0, 0 ) );
    p1.setLabel( 1 );
        
    //  resample
    Resampler R( 0.2 );
    R.setPhaseCorrect( false );
    R.resample( p1 );
    
    //  check the Partial for correct resampling    
    //  should have 6 Breakpoints, starting at 0.4, ending 1.4
    TEST_VALUE( p1.label(), 1 );
    TEST_VALUE( p1.numBreakpoints(), 6 );
    TEST_SAME_TIMES( p1.startTime(), 0.4 );
    TEST_SAME_TIMES( p1.endTime(), 1.4 );
    
    for ( Partial::iterator pit = p1.begin(); pit != p1.end(); ++pit )
    {
        cout << pit->frequency() << endl;
    }
    
    //  spot check the parameters
    Partial::iterator iter = p1.begin();
    
    TEST_SAME_FLOAT( iter->frequency(), 100 );
    TEST_SAME_FLOAT( (++iter)->frequency(), 115 );
    TEST_SAME_FLOAT( (++iter)->frequency(), 135 );
    TEST_SAME_FLOAT( (++iter)->frequency(), 155 );
    TEST_SAME_FLOAT( (++iter)->frequency(), 175 );
    TEST_SAME_FLOAT( (++iter)->frequency(), 195 );
} 

// ----------- test_dense_resample_list -----------
//
static void test_dense_resample_list( void )
{
	cout << "\t--- testing dense resampling of PartialList... ---\n\n";
		
	//  build some Partials
    Partial p1;
    p1.insert( 0.5, Breakpoint( 100, 0.1, 0, 0 ) );
    p1.insert( 1.5, Breakpoint( 100, 0.1, 0, 0 ) );
    p1.setLabel( 1 );
    
    Partial p2;
    p2.insert( 0.5, Breakpoint( 200, 0.1, 0, 0 ) );
    p2.insert( 0.75, Breakpoint( 200, 0.1, 0, 0 ) );
    p2.setLabel( 2 );
    
    Partial p3;
    p3.insert( 1.5, Breakpoint( 300, 0.1, 0, 0 ) );
    p3.insert( 3.0, Breakpoint( 300, 0.1, 0, 0 ) );
    p3.setLabel( 3 );

    PartialList l;
    l.push_back( p1 );
    l.push_back( p2 );
    l.push_back( p3 );
    
    
    //  resample
    Resampler R( 0.3 );
    R.resample( l.begin(), l.end() );

    
    //  check each Partial for correct resampling
    PartialList::iterator iter = l.begin();
    
    //  #1
    //  should have 4 Breakpoints, starting at 0.6, ending 1.5
    Partial & cp1 = *iter++;
    TEST_VALUE( cp1.label(), 1 );
    TEST_VALUE( cp1.numBreakpoints(), 4 );
    TEST_SAME_TIMES( cp1.startTime(), 0.6 );
    TEST_SAME_TIMES( cp1.endTime(), 1.5 );

    //  #2
    //  should have 1 Breakpoint, starting at 0.6, ending 0.6
    Partial & cp2 = *iter++;
    TEST_VALUE( cp2.label(), 2 );
    TEST_VALUE( cp2.numBreakpoints(), 2 );
    TEST_SAME_TIMES( cp2.startTime(), 0.6 );
    TEST_SAME_TIMES( cp2.endTime(), 0.9 );
    
    //  #3
    //  should have 6 Breakpoints, starting at 1.5, ending 3.0
    Partial & cp3 = *iter++;
    TEST_VALUE( cp3.label(), 3 );
    TEST_VALUE( cp3.numBreakpoints(), 6 );
    TEST_SAME_TIMES( cp3.startTime(), 1.5 );
    TEST_SAME_TIMES( cp3.endTime(), 3.0 );

}    

// ----------- test_resample_with_timing -----------
//
static void test_resample_with_timing( void )
{
	cout << "\t--- testing resampling with timing envelope... ---\n\n";
		
	//  build some Partials
    Partial p1;
    p1.insert( 0.5, Breakpoint( 100, 0.1, 0, 0 ) );
    p1.insert( 1.5, Breakpoint( 100, 0.1, 0, 0 ) );
    p1.setLabel( 1 );
    
    Partial p2;
    p2.insert( 0.5, Breakpoint( 200, 0.1, 0, 0 ) );
    p2.insert( 0.75, Breakpoint( 200, 0.1, 0, 0 ) );
    p2.setLabel( 2 );
    
    Partial p3;
    p3.insert( 1.5, Breakpoint( 300, 0.1, 0, 0 ) );
    p3.insert( 3.0, Breakpoint( 300, 0.1, 0, 0 ) );
    p3.setLabel( 3 );

   
    Partial p4;
    p4.insert( 0.5, Breakpoint( 400, 0.1, 0, 0 ) );
    p4.insert( 3.0, Breakpoint( 400, 0.1, 0, 0 ) );
    p4.setLabel( 4 );
    
    Partial p5;
    p5.insert( 3.0, Breakpoint( 500, 0.1, 0, 0 ) );
    p5.insert( 5.5, Breakpoint( 500, 0.1, 0, 0 ) );
    p5.setLabel( 5 );
   
    PartialList l;
    l.push_back( p1 );
    l.push_back( p2 );
    l.push_back( p3 );
    l.push_back( p4 );
    l.push_back( p5 );
    
    //  build a timing envelope
    LinearEnvelope env;
    env.insert( 0, 1 );
    env.insert( 1.5, 2.5 );
    env.insert( 3, 1 );    
    
    //  resample
    Resampler R( 0.2 );
    R.resample( l.begin(), l.end(), env );
    
    //  check each Partial for correct resampling
    PartialList::iterator iter = l.begin();
    
    //  #1
    //  should have 16 Breakpoints, starting at 0.0, ending 3.0
    Partial & cp1 = *iter++;
    TEST_VALUE( cp1.label(), 1 );
    TEST_VALUE( cp1.numBreakpoints(), 16 );
    TEST_SAME_TIMES( cp1.startTime(), 0.0 );
    TEST_SAME_TIMES( cp1.endTime(), 3.0 );

    //  #2
    //  should be empty
    Partial & cp2 = *iter++;
    TEST_VALUE( cp2.label(), 2 );
    TEST_VALUE( cp2.numBreakpoints(), 0 );
    
    //  #3
    //  should have 10 Breakpoints, starting at .6, ending 2.4
    Partial & cp3 = *iter++;
    TEST_VALUE( cp3.label(), 3 );
    TEST_VALUE( cp3.numBreakpoints(), 10 );
    TEST_SAME_TIMES( cp3.startTime(), 0.6 );
    TEST_SAME_TIMES( cp3.endTime(), 2.4 );

    //  #4
    //  should have 16 Breakpoints, starting at 0.0, ending 3.0
    Partial & cp4 = *iter++;
/*
    cout << "partial " << cp4.label() << " has " << cp4.numBreakpoints() << " breakpoints" << endl;
    for ( Partial::iterator pit = cp4.begin(); pit != cp4.end(); ++pit )
    {
        cout << pit.time() << endl;
    }
*/
    TEST_VALUE( cp4.label(), 4 );
    TEST_VALUE( cp4.numBreakpoints(), 16 );
    TEST_SAME_TIMES( cp4.startTime(), 0.0 );
    TEST_SAME_TIMES( cp4.endTime(), 3.0 );

    //  #5
    //  should be empty
    Partial & cp5 = *iter++;
    TEST_VALUE( cp5.label(), 5 );
    TEST_VALUE( cp5.numBreakpoints(), 0 );
    
}    

// ----------- test_quantize_list -----------
//
static void test_quantize_list( void )
{
	cout << "\t--- testing quantizing Breakpoint times of PartialList... ---\n\n";
		
	//  build some  Partials
    Partial p1;
    p1.insert( 0.5, Breakpoint( 100, 0.1, 0, 0 ) );
    p1.insert( 1.5, Breakpoint( 100, 0.1, 0, 0 ) );
    p1.setLabel( 1 );
    
    Partial p2;
    p2.insert( 0.5, Breakpoint( 200, 0.1, 0, 0 ) );
    p2.insert( 1.55, Breakpoint( 200, 0.1, 0, 0 ) );
    p2.insert( 1.75, Breakpoint( 200, 0.1, 0, 0 ) );
    p2.setLabel( 2 );
    
    PartialList l;
    l.push_back( p1 );
    l.push_back( p2 );    
    
    //  resample
    Resampler R( 0.4 );
    R.quantize( l.begin(), l.end() );

    //  check each Partial for correct quantizing
    PartialList::iterator iter = l.begin();
    
    //  #1
    //  should have 2 Breakpoints, starting at 0.4, ending 1.6
    Partial & cp1 = *iter++;
    TEST_VALUE( cp1.label(), 1 );
    TEST_VALUE( cp1.numBreakpoints(), 2 );
    TEST_SAME_TIMES( cp1.startTime(), 0.4 );
    TEST_SAME_TIMES( cp1.endTime(), 1.6 );

    //  #2
    //  should have 2 Breakpoints, starting at 0.4, ending 1.6
    Partial & cp2 = *iter++;
    TEST_VALUE( cp2.label(), 2 );
    TEST_VALUE( cp2.numBreakpoints(), 2 );
    TEST_SAME_TIMES( cp2.startTime(), 0.4 );
    TEST_SAME_TIMES( cp2.endTime(), 1.6 );

}    


   
// ----------- main -----------
//
int main( )
{
    std::cout << "Unit test for resampling operations." << endl;
    std::cout << "Uses Partial, PartialList, and LinearEnvelope." << endl << endl;
    std::cout << "Built: " << __DATE__ << endl << endl;
    
    try 
    {
        test_dense_resample();
        test_dense_resample_list();
        test_resample_with_timing();
        test_quantize_list();
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
    cout << "Resampler passed all tests." << endl;
    return 0;
}

