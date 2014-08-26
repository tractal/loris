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
 *  test_Distiller.C
 *
 *  Unit tests for Distiller class. Relies on Breakpoint,
 *  Partial, PartialList and Loris Exceptions. Build with
 *  Partial.C, Breakpoint.C, Exception.C, Distiller.C 
 *  and Notifier.C.
 *
 * Kelly Fitz, 17 April 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include "Collator.h"
#include "Distiller.h"
#include "Exception.h"
#include "Partial.h"
#include "PartialList.h"

#include <cmath>
#include <iostream>

using namespace Loris;
using namespace std;


#if defined(HAVE_M_PI) && (HAVE_M_PI)
	const double Pi = M_PI;
#else
	const double Pi = 3.14159265358979324;
#endif

// --- macros ---

//  define this to see pages and pages of spew
//#define VERBOSE
#ifdef VERBOSE                                  
    #define TEST(invariant)                                 \
        do {                                                    \
            std::cout << "TEST: " << #invariant << endl;        \
            Assert( invariant );                                \
            std::cout << " PASS" << endl << endl;           \
        } while (false)
    
    #define TEST_VALUE( expr, val )                                 \
        do {                                                            \
            std::cout << "TEST: " << #expr << "==" << (val) << endl;\
            Assert( (expr) == (val) );                              \
            std::cout << "  PASS" << endl << endl;                  \
        } while (false)
#else
    #define TEST(invariant)                 \
        do {                                    \
            Assert( invariant );                \
        } while (false)
    
    #define TEST_VALUE( expr, val )         \
        do {                                    \
            Assert( (expr) == (val) );      \
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

#define SAME_PARAM_VALUES(x,y) TEST( float_equal((x),(y)) )

// ---------------------------------------------------------------------------
//  wrapPi
// ---------------------------------------------------------------------------
//  O'Donnell's phase wrapping function.
//
static inline double wrapPi( double x )
{
    using namespace std; // floor should be in std
    #define ROUND(x) (floor(.5 + (x)))
    const double TwoPi = 2.0*Pi;
    return x + ( TwoPi * ROUND(-x/TwoPi) );
}


// ----------- test_distill_manylabels -----------
//
static void test_distill_manylabels( void )
{
    std::cout << "\t--- testing distill on "
                 "Partials having different labels... ---\n\n";

    //  Fabricate several Partials with different labels
    //  and distill them. Verify that Partials having
    //  different labels are not combined.
    Partial p1;
    p1.insert( 0.1, Breakpoint( 100, 0.1, 0, 0 ) );
    p1.insert( 1.1, Breakpoint( 110, 0.2, 0.2, .1 ) );
    p1.setLabel( 1 );
    
    Partial p2;
    p2.insert( 0.2, Breakpoint( 200, 0.1, 0, 0 ) );
    p2.insert( 1.2, Breakpoint( 210, 0.2, 0.2, .1 ) );
    p2.setLabel( 2 );
    
    Partial p3;
    p3.insert( 0.3, Breakpoint( 300, 0.1, 0, 0 ) );
    p3.insert( 1.3, Breakpoint( 310, 0.2, 0.2, .1 ) );
    p3.setLabel( 3 );
    
    PartialList l;
    l.push_back( p1 );
    l.push_back( p3 );
    l.push_back( p2 );
    
    Distiller d;
    d.distill( l );
    
    //  compare Partials (distilled Partials
    //  should be in label order):
    TEST( l.size() == 3 );
    PartialList::iterator it = l.begin();
    TEST( it->label() == p1.label() );
    TEST( it->numBreakpoints() == p1.numBreakpoints() );
    ++it;
    TEST( it->label() == p2.label() );
    TEST( it->numBreakpoints() == p2.numBreakpoints() );
    ++it;
    TEST( it->label() == p3.label() );
    TEST( it->numBreakpoints() == p3.numBreakpoints() );
}

// ----------- test_distill_nonoverlapping -----------
//
static void test_distill_nonoverlapping( void )
{
    std::cout << "\t--- testing distill on "
                 "non-overlapping Partials... ---\n\n";
                 
    //  Fabricate three non-overlapping Partials, give
    //  them all the same label, and distill them. Also
    //  add a fourth Partial with a different label, verify
    //  that it remains unaffacted.
    Partial p1;
    p1.insert( 0, Breakpoint( 100, 0.1, 0, 0 ) );
    p1.insert( .1, Breakpoint( 110, 0.2, 0.2, .1 ) );
    p1.setLabel( 123 );
    
    Partial p2;
    p2.insert( 0.2, Breakpoint( 200, 0.1, 0, 0 ) );
    p2.insert( 0.3, Breakpoint( 210, 0.2, 0.2, .1 ) );
    p2.setLabel( 123 );
    
    Partial p3;
    p3.insert( 0.4, Breakpoint( 300, 0.1, 0, 0 ) );
    p3.insert( 0.5, Breakpoint( 310, 0.2, 0.2, .1 ) );
    p3.setLabel( 123 );
    
    Partial p4;
    p4.insert( 0, Breakpoint( 400, 0.1, 0, 0 ) );
    p4.insert( 0.5, Breakpoint( 410, 0.2, 0.2, .1 ) );
    p4.setLabel( 4 );
    
    PartialList l;
    l.push_back( p1 );
    l.push_back( p3 );
    l.push_back( p4 );
    l.push_back( p2 );
    
    const double fade = .01; // 10 ms
    Distiller d( fade );
    d.distill( l );
    
    //  Fabricate the Partial that the distillation should 
    //  produce.
    Partial compare;
    compare.insert( 0, Breakpoint( 100, 0.1, 0, 0 ) );
    compare.insert( 0.1, Breakpoint( 110, 0.2, 0.2, .1 ) );
    double t = 0.1 + fade;
    compare.insert( t, Breakpoint( p1.frequencyAt(t), 0, 
                                   0, // p1.bandwidthAt(t), 
                                   p1.phaseAt(t) ) );
    t = 0.2 - fade;
    compare.insert( t, Breakpoint( p2.frequencyAt(t), 0, 
                                   0, // p2.bandwidthAt(t), 
                                   p2.phaseAt(t) ) );
    compare.insert( 0.2, Breakpoint( 200, 0.1, 0, 0 ) );
    compare.insert( 0.3, Breakpoint( 210, 0.2, 0.2, .1 ) );
    t = 0.3 + fade;
    compare.insert( t, Breakpoint( p2.frequencyAt(t), 0, 
                                   0, // p2.bandwidthAt(t), 
                                   p2.phaseAt(t) ) );
    t = 0.4 - fade;
    compare.insert( t, Breakpoint( p3.frequencyAt(t), 0, 
                                   0, // p3.bandwidthAt(t), 
                                   p3.phaseAt(t) ) );    
    compare.insert( 0.4, Breakpoint( 300, 0.1, 0, 0 ) );
    compare.insert( 0.5, Breakpoint( 310, 0.2, 0.2, .1 ) );
    compare.setLabel( 123 );
    
    //  compare Partials (distilled Partials
    //  should be in label order):
    TEST( l.size() == 2 );
    PartialList::iterator it = l.begin();
    TEST( it->label() == p4.label() );
    TEST( it->numBreakpoints() == p4.numBreakpoints() );
    ++it;
    
    for ( Partial::iterator distit = it->begin(); distit != it->end(); ++distit )
    {
        cout << distit.time() << " " << distit.breakpoint().frequency() << endl;
    }
    
    TEST( it->numBreakpoints() == compare.numBreakpoints() );
    
    Partial::iterator distit = it->begin();
    Partial::iterator compareit = compare.begin();
    while ( compareit != compare.end() )
    {
        SAME_PARAM_VALUES( distit.time(), compareit.time() );
        SAME_PARAM_VALUES( distit->frequency(), compareit->frequency() );
        SAME_PARAM_VALUES( distit->amplitude(), compareit->amplitude() );
        SAME_PARAM_VALUES( distit->bandwidth(), compareit->bandwidth() );
        SAME_PARAM_VALUES( distit->phase(), compareit->phase() );
        
        ++compareit;
        ++distit;
    }
}

// ----------- test_distill_overlapping2 -----------
//
static void test_distill_overlapping2( void )
{
    std::cout << "\t--- testing distill on two "
                 "temporally-overlapping Partials... ---\n\n";

    //  Fabricate two Partials, overlapping temporally, give
    //  them the same label, and distill them.
    Partial p1;
    p1.insert( 0, Breakpoint( 100, 0.4, 0, 0 ) );
    p1.insert( 0.3, Breakpoint( 100, 0.4, 0, .1 ) );
    p1.setLabel( 12 );
    
    Partial p2;
    p2.insert( 0.2, Breakpoint( 200, 0.3, 0, 0 ) );
    p2.insert( 0.35, Breakpoint( 210, 0.3, 0.2, .1 ) );
    p2.setLabel( 12 );

    PartialList l;
    l.push_back( p1 );
    l.push_back( p2 );

    const double fade = .01; // 10 ms
    Distiller d( fade );
    d.distill( l );
    
    for ( Partial::iterator distit = l.front().begin(); distit != l.front().end(); ++distit )
    {
        cout << distit.time() << " " << distit.breakpoint().frequency() << endl;
    }
    
    //  Fabricate the Partial that the distillation should 
    //  produce.
    Partial compare;
    
    //  first Breakpoint from p1
    compare.insert( 0, Breakpoint( 100, 0.4, 0, 0 ) );
    
    //  null Breakpoint at 0+fade
    double t = 0 + fade;
    compare.insert( t, Breakpoint( p1.frequencyAt(t), 0, 
                                   p1.bandwidthAt(t), p1.phaseAt(t) ) );

    //  interpolated Breakpoint at .18 (.2 - 2*fade)
    //double t = 0.2 - (2*fade);
    //compare.insert( t, p1.parametersAt( t ) );
    
    //  null Breakpoint at .19 (.2-fade)
    //  bandwidth introduced in the overlap region:
    //  0.4^2 / (0.3^2 + 0.4^2) = 0.64
    //  amp = sqrt(0.3^2 + 0.4^2) = .5
    //  no, actually zero-amplitude Breakpoints are
    //  introduced with zero bandwidth.
    t = 0.2 - fade;
    compare.insert( t, Breakpoint( p2.frequencyAt(t), 0, 
                                   0, // 0.64, 
                                   p2.phaseAt(t) ) );
                                   
    //  first Breakpoint from p2:
    compare.insert( 0.2, Breakpoint( 200, 0.5, 0.64, 0 ) );
    
    //  second Breakpoint from p2
    compare.insert( 0.35, Breakpoint( 210, 0.3, 0.2, .1 ) );
    compare.setLabel( 12 );

    //  compare Partials (distilled Partials
    //  should be in label order):
    TEST( l.size() == 1 );
    TEST( l.begin()->numBreakpoints() == compare.numBreakpoints() );
    TEST( l.begin()->label() == compare.label() );
    
    Partial::iterator distit = l.begin()->begin();
    Partial::iterator compareit = compare.begin();
    while ( compareit != compare.end() )
    {
        SAME_PARAM_VALUES( distit.time(), compareit.time() );
        SAME_PARAM_VALUES( distit->frequency(), compareit->frequency() );
        SAME_PARAM_VALUES( distit->amplitude(), compareit->amplitude() );
        SAME_PARAM_VALUES( distit->bandwidth(), compareit->bandwidth() );
        SAME_PARAM_VALUES( distit->phase(), compareit->phase() );
        
        ++compareit;
        ++distit;
    }
}

// ----------- test_distill_overlapping3 -----------
//
static void test_distill_overlapping3( void )
{
    std::cout << "\t--- testing distill on three "
                 "temporally-overlapping Partials... ---\n\n";

    //  Fabricate three Partials, overlapping temporally, give
    //  them the same label, and distill them.
    Partial p1;
    p1.insert( 0, Breakpoint( 100, 0.4, 0, 0 ) );
    p1.insert( 0.28, Breakpoint( 100, 0.4, 0, .1 ) );
    p1.setLabel( 123 );
    
    Partial p2;
    p2.insert( 0.2, Breakpoint( 200, 0.3, 0.2, 0 ) );
    p2.insert( 0.29, Breakpoint( 200, 0.3, 0.2, .1 ) );
    p2.insert( 0.35, Breakpoint( 200, 0.3, 0.2, .1 ) );
    p2.setLabel( 123 );

    Partial p3;
    p3.insert( 0.32, Breakpoint( 300, 0.3, 0, 0 ) );
    p3.insert( 0.4, Breakpoint( 310, 0.3, 0.2, .1 ) );
    p3.insert( 0.7, Breakpoint( 310, 0.3, 0.2, .1 ) );
    p3.setLabel( 123 );

    PartialList l;
    l.push_back( p3 );
    l.push_back( p1 );
    l.push_back( p2 );

    const double fade = .008; // 8 ms
    Distiller d( fade );
    d.distill( l );
    
    //  Fabricate the Partial that the distillation should 
    //  produce.
    Partial compare;
    
    //  first Breakpoint from p1
    compare.insert( 0, Breakpoint( 100, 0.4, 0, 0 ) );

    //  null Breakpoint at 0+fade
    double t = 0 + fade;
    compare.insert( t, Breakpoint( p1.frequencyAt(t), 0, 
                                   p1.bandwidthAt(t), p1.phaseAt(t) ) );
    //  interpolated Breakpoint at .18 (.2 - 2*fade)
    //double t = 0.2 - (2*fade);
    //compare.insert( t, p1.parametersAt( t ) );

    //  null Breakpoint at .19 (.2-fade)
    //  bandwidth introduced in the overlap region:
    //  (0.4^2 + 0.2*0.3^2) / (0.3^2 + 0.4^2)) = 0.712
    //  amp = sqrt(0.3^2 + 0.4^2) = .5
    //  no, actually zero-amplitude Breakpoints are
    //  introduced with zero bandwidth.
    t = 0.2 - fade;
    compare.insert( t, Breakpoint( p2.frequencyAt(t), 0, 
                                   0, // 0.712, 
                                   p2.phaseAt(t) ) );
                                   
    //  first Breakpoint from p2:
    compare.insert( 0.2, Breakpoint( 200, 0.5, 0.712, 0 ) );
                                   
    //  second Breakpoint from p2:
    compare.insert( 0.29, Breakpoint( 200, 0.3, 0.2, 0.1 ) );

    //  null Breakpoint at .29 + fade
    t = 0.29 + fade;
    compare.insert( t, Breakpoint( p2.frequencyAt(t), 0, 
                                   0, // p2.bandwidthAt(t), 
                                   p2.phaseAt(t) ) );

    //  null Breakpoint at .31 (.32-fade)
    t = 0.32 - fade;
    compare.insert( t, Breakpoint( p3.frequencyAt(t), 0, 
                                   0, p3.phaseAt(t) ) );

    //  first Breakpoint from p3 (with bandwidth):
    compare.insert( 0.32, Breakpoint( 300, std::sqrt(0.18), 0.5, 0 ) );
                                   
    //  second Breakpoint from p3:
    compare.insert( 0.4, Breakpoint( 310, 0.3, 0.2, .1 ) );
                                   
    //  third Breakpoint from p3:
    compare.insert( 0.7, Breakpoint( 310, 0.3, 0.2, .1 ) );
    compare.setLabel( 123 );

    //  compare Partials (distilled Partials
    //  should be in label order):
    TEST_VALUE( l.size(), 1 );
    TEST_VALUE( l.begin()->numBreakpoints(), compare.numBreakpoints() );
    
    Partial::iterator distit = l.begin()->begin();
    Partial::iterator compareit = compare.begin();
    while ( compareit != compare.end() )
    {
        SAME_PARAM_VALUES( distit.time(), compareit.time() );
        SAME_PARAM_VALUES( distit->frequency(), compareit->frequency() );
        SAME_PARAM_VALUES( distit->amplitude(), compareit->amplitude() );
        SAME_PARAM_VALUES( distit->bandwidth(), compareit->bandwidth() );
        SAME_PARAM_VALUES( wrapPi( distit->phase() ), wrapPi( compareit->phase() ) );
        
        ++compareit;
        ++distit;
    }
}

// ----------- test_collate -----------
//
static void test_collate( void )
{
    std::cout << "\t--- testing collate on three "
                 "temporally-overlapping Partials... ---\n\n";

    //  Fabricate three Partials, overlapping temporally, 
    //  leave them unlabeled, and distill (collate) them.
    Partial p1;
    p1.insert( 0, Breakpoint( 100, 0.4, 0, 0 ) );
    p1.insert( 0.3, Breakpoint( 110, 0.4, 0, .1 ) );
    p1.setLabel( 0 );
    
    Partial p2;
    p2.insert( 0.2, Breakpoint( 200, 0.3, 0, 0 ) );
    p2.insert( 0.35, Breakpoint( 210, 0.3, 0.2, .1 ) );
    p2.setLabel( 0 );

    Partial p3;
    p3.insert( 0.33, Breakpoint( 300, 0.3, 0, 0 ) );
    p3.insert( 0.4, Breakpoint( 310, 0.3, 0.2, .1 ) );
    p3.setLabel( 0 );

    PartialList l;
    l.push_back( p3 );
    l.push_back( p1 );
    l.push_back( p2 );

    const double fade = .01; // 10 ms
    Collator d( fade );
    d.collate( l );

    //  Fabricate the Partials that the distillation should 
    //  produce.
    Partial compare1;
    compare1.insert( 0, Breakpoint( 100, 0.4, 0, 0 ) );
    compare1.insert( 0.3, Breakpoint( 110, 0.4, 0, .1 ) );
    double t = 0.3 + fade;
    compare1.insert( t, Breakpoint( p1.frequencyAt(t), 0, 
                                    p1.bandwidthAt(t), p1.phaseAt(t) ) );
    t = 0.33 - fade;
    compare1.insert( t, Breakpoint( p3.frequencyAt(t), 0, 
                                    p3.bandwidthAt(t), p3.phaseAt(t) ) );
    compare1.insert( 0.33, Breakpoint( 300, 0.3, 0, 0 ) );
    compare1.insert( 0.4, Breakpoint( 310, 0.3, 0.2, .1 ) );
    compare1.setLabel(1);

    Partial compare2 = p2;
    compare2.setLabel(2);
    
    //  compare Partials, the first one will be
    //  the one that was constructed from the 
    //  Partial with the earliest end (p1):
    TEST( l.size() == 2 );
    PartialList::iterator it = l.begin();
    TEST( it->label() == compare1.label() );
    TEST( it->numBreakpoints() == compare1.numBreakpoints() );
    
    Partial::iterator distit = it->begin();
    Partial::iterator compareit = compare1.begin();
    while ( compareit != compare1.end() )
    {
        SAME_PARAM_VALUES( distit.time(), compareit.time() );
        SAME_PARAM_VALUES( distit->frequency(), compareit->frequency() );
        SAME_PARAM_VALUES( distit->amplitude(), compareit->amplitude() );
        SAME_PARAM_VALUES( distit->bandwidth(), compareit->bandwidth() );
        SAME_PARAM_VALUES( distit->phase(), compareit->phase() );
        
        ++compareit;
        ++distit;
    }
    
    ++it;
    TEST( it->numBreakpoints() == compare2.numBreakpoints() );
    TEST( it->label() == compare2.label() );
    distit = it->begin();
    compareit = compare2.begin();
    while ( compareit != compare2.end() )
    {
        SAME_PARAM_VALUES( distit.time(), compareit.time() );
        SAME_PARAM_VALUES( distit->frequency(), compareit->frequency() );
        SAME_PARAM_VALUES( distit->amplitude(), compareit->amplitude() );
        SAME_PARAM_VALUES( distit->bandwidth(), compareit->bandwidth() );
        SAME_PARAM_VALUES( distit->phase(), compareit->phase() );
        
        ++compareit;
        ++distit;
    }
}


// ----------- main -----------
//
int main( )
{
    std::cout << "Unit test for Distiller class." << endl;
    std::cout << "Relies on Breakpoint and Partial." << endl << endl;
    std::cout << "Built: " << __DATE__ << endl << endl;
    
    try 
    {
        test_distill_manylabels();
        test_distill_nonoverlapping();
        test_distill_overlapping2();
        test_distill_overlapping3();
        test_collate();
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
    cout << "Distiller passed all tests." << endl;
    return 0;
}


