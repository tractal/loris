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
 *	test_Cropper.C
 *
 *	Unit tests for Loris PartialList cropping operations.
 *
 *
 * Kelly Fitz, 25 Jan 2010
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "Partial.h"
#include "PartialUtils.h"
#include "Exception.h"

#include "loris.h"

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

// ----------- test_Cropper -----------
//
static void test_Cropper( void )
{
	cout << "\t--- testing Cropper class in PartialUtils... ---\n\n";
	
	
	//  build five Partials
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
    p5.insert( 5.5, Breakpoint( 500, 0.1, 0, 0 ) );
    p5.insert( 3.0, Breakpoint( 500, 0.1, 0, 0 ) );
    p5.setLabel( 5 );
    
    PartialList l;
    l.push_back( p1 );
    l.push_back( p2 );
    l.push_back( p3 );
    l.push_back( p4 );
    l.push_back( p5 );
    
    
    //  crop between 1 and 2s
    PartialUtils::Cropper cc( 1, 2 );
    for ( PartialList::iterator iter = l.begin(); iter != l.end(); ++iter )
    {
        cc( *iter );
    }
    
    //  check for the correct number of Partials after cropping:
    //  second and fifth Partials (labeled 2 and 5) should be empty, 
    //  but not absent
    TEST_VALUE( l.size(), 5 );
    
    //  check each Partial for correct cropping
    PartialList::iterator iter = l.begin();
    
    //  #1
    Partial & cp1 = *iter++;
    TEST_VALUE( cp1.label(), 1 );
    TEST_VALUE( cp1.numBreakpoints(), 2 );
    TEST_SAME_TIMES( cp1.startTime(), 1 );

    //  #2 is empty
    Partial & cp2 = *iter++;
    TEST_VALUE( cp2.label(), 2 );
    TEST_VALUE( cp2.numBreakpoints(), 0 );
    
    //  #3
    Partial & cp3 = *iter++;
    TEST_VALUE( cp3.label(), 3 );
    TEST_VALUE( cp3.numBreakpoints(), 2 );
    TEST_SAME_TIMES( cp3.endTime(), 2 );
    
    //  #4
    Partial & cp4 = *iter++;
    TEST_VALUE( cp4.label(), 4 );
    TEST_VALUE( cp4.numBreakpoints(), 2 );
    TEST_SAME_TIMES( cp4.startTime(), 1 );
    TEST_SAME_TIMES( cp4.endTime(), 2 );     

    //  #5 is empty
    Partial & cp5 = *iter++;
    TEST_VALUE( cp5.label(), 5 );
    TEST_VALUE( cp5.numBreakpoints(), 0 );
       
}    

// ----------- test_crop_pi -----------
//
static void test_crop_pi( void )
{
	cout << "\t--- testing crop operation in procedural interface... ---\n\n";
	
	
	//  build four Partials
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
    p5.insert( 5.5, Breakpoint( 500, 0.1, 0, 0 ) );
    p5.insert( 3.0, Breakpoint( 500, 0.1, 0, 0 ) );
    p5.setLabel( 5 );
    
    PartialList l;
    l.push_back( p1 );
    l.push_back( p2 );
    l.push_back( p3 );
    l.push_back( p4 );
    l.push_back( p5 );
    
    
    //  crop between 1 and 2s
    crop( &l, 1, 2 );
    
    //  check for the correct number of Partials after cropping:
    //  second Partial (labeled 2) should have been removed
    TEST_VALUE( l.size(), 3 );
    
    //  check each Partial for correct cropping
    PartialList::iterator iter = l.begin();
    
    //  #1
    Partial & cp1 = *iter++;
    TEST_VALUE( cp1.label(), 1 );
    TEST_VALUE( cp1.numBreakpoints(), 2 );
    TEST_SAME_TIMES( cp1.startTime(), 1 );

    //  #2 is gone
    
    //  #3
    Partial & cp3 = *iter++;
    TEST_VALUE( cp3.label(), 3 );
    TEST_VALUE( cp3.numBreakpoints(), 2 );
    TEST_SAME_TIMES( cp3.endTime(), 2 );
    
    //  #4
    Partial & cp4 = *iter++;
    TEST_VALUE( cp4.label(), 4 );
    TEST_VALUE( cp4.numBreakpoints(), 2 );
    TEST_SAME_TIMES( cp4.startTime(), 1 );
    TEST_SAME_TIMES( cp4.endTime(), 2 );     
       
}    
    
// ----------- main -----------
//
int main( )
{
    std::cout << "Unit test for cropping operations." << endl;
    std::cout << "Relies on Partial." << endl << endl;
    std::cout << "Built: " << __DATE__ << endl << endl;
    
    try 
    {
        test_Cropper();
        test_crop_pi();
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
    cout << "Cropper passed all tests." << endl;
    return 0;
}

