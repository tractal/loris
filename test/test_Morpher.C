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
 *  test_Morpher.C
 *
 *  Unit test for Morpher class. Relies on Partial, Breakpoint, and
 * BreakpointEnvelope, and Loris Exceptions.
 *
 * Kelly Fitz, 22 May 2002
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "Breakpoint.h"
#include "BreakpointEnvelope.h"
#include "Exception.h"
#include "Morpher.h"
#include "Partial.h"

#include <cmath>
#include <iostream>

using namespace Loris;
using namespace std;

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
    #define EPSILON .01 // VERY lax with morphing, should be more rigorous
    bool ret = false;
    if ( std::fabs(x) > EPSILON )
    {
        ret = std::fabs((x-y)/x) < EPSILON;
    }
    else
    {
        ret = std::fabs(x-y) < EPSILON;
    }
    if ( !ret )
    {
        cout << "\tFAILED: " << x << " != " << y << " !" << endl;
    }
    return ret;
}

const double Pi = 3.14159265358979324;
    
static double m2pi( double phi )
{
    while ( phi > Pi )
    {
        phi -= 2 * Pi;
    }
    while ( phi > Pi )
    {
        phi += 2 * Pi;
    }
    return phi;
}

static void computePhaseFwd( Partial::iterator b, Partial::iterator e ); // at bottom


static Partial makep1( void )
{
   Partial p1;
   
   const double fslope = 100, f0 = 100;
   const double aslope = 0, a0 = .2;
   const double bslope = 1, b0 = .1;
   const double p0 = 0.2;
   double t0 = 0, dt = .08;
   
   for ( double t = t0; t <= .8; t += dt )
   {
      Breakpoint bp( f0 + ((t-t0)*fslope), 
                     a0 + ((t-t0)*aslope),
                     b0 + ((t-t0)*bslope),
                     p0 + (2 * Pi * ((f0*t) + (.5*fslope*t*t))) );
      //cout << t << endl;
      p1.insert( t, bp );
   }
   // computePhaseFwd( p1.begin(), p1.end() );
   return p1;
}

static Partial makep2( void )
{
   Partial p2;
   
   const double fslope = 0, f0 = 200;
   const double aslope = .5/.8, a0 = .1;
   const double bslope = -1, b0 = .9;
   const double p0 = 0;
   double t0 = .2, dt = .021;
   
   for ( double t = t0; t <= (t0 + 0.8); t += dt )
   {
      Breakpoint bp( f0 + ((t-t0)*fslope), 
                     a0 + ((t-t0)*aslope),
                     b0 + ((t-t0)*bslope),
                     p0 + (2 * Pi * ((f0*t) + (.5*fslope*t*t))) );
      //cout << t << endl;
      // computePhaseFwd( p2.begin(), p2.end() );      
      p2.insert( t, bp );
   }
   return p2;
}

int main( )
{
    std::cout << "Unit test for Morpher class." << endl;
    std::cout << "Relies on Partial, Breakpoint, and LinearEnvelope." << endl << endl;
    std::cout << "Built: " << __DATE__ << endl << endl;


    try 
    {
        //  construct Morphing envelopes:
        BreakpointEnvelope fenv, aenv, bwenv, otherenv;
        
        //  frequency envelope: (0,0), (.2,0), (.4, .5), (.6, .5), (.8, 1), (1,1)
        const int NUM_ENVPTS = 6;
        const double MENV_TIMES[] = {0, .2, .4, .6, .8, 1};
        const double FENV_WEIGHTS[] = {0, 0, .5, .5, 1, 1};
        for ( int i = 0; i < NUM_ENVPTS; ++i )
        {
            fenv.insertBreakpoint( MENV_TIMES[i], FENV_WEIGHTS[i] );
        }
      
        //  amplitude envelope: (0,0), (.2,.5), (.4, 1), (.6, 1), (.8, 1), (1,1)
        const double AENV_WEIGHTS[] = {0,.5, 1, 1, 1, 1};
        for ( int i = 0; i < NUM_ENVPTS; ++i )
        {
        aenv.insertBreakpoint( MENV_TIMES[i], AENV_WEIGHTS[i] );
        }
        
        //  bandwidth envelope: (0,0), (.2,.5), (.4, 1), (.6, 1), (.8, 0), (1,0)
        const double BWENV_WEIGHTS[] = {0, .5, 1, 1, .0, 0};
        for ( int i = 0; i < NUM_ENVPTS; ++i )
        {
        bwenv.insertBreakpoint( MENV_TIMES[i], BWENV_WEIGHTS[i] );
        }
        
        //  construct Morpher, use linear morphing:
        Morpher testM( fenv, aenv, bwenv );
        testM.enableLogAmpMorphing( false );
        testM.enableLogFreqMorphing( false );
        
        /*                                                */
        /*************** envelope tests *******************/
        /*                                                */
        
        //  check the envelopes at several times to verify that the
        //  morphing envelopes got stored correctly:
        #define SAME_ENV_VALUES(x,y) TEST( float_equal((x),(y)) )
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.3), fenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.6), fenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.9), fenv.valueAt(.9) );
        
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.3), aenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.6), aenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.9), aenv.valueAt(.9) );
        
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.3), bwenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.6), bwenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.9), bwenv.valueAt(.9) );
        
        //  change each envelope, and verify that the changes are
        //  correctly registered:
        //  other envelope: (0,.4), (.5, .2), (1,.3)
        const int OTHER_NUM_ENVPTS = 3;
        const double OTHERENV_TIMES[] = {0, .5, 1};
        const double OTHERENV_WEIGHTS[] = {.4, .2, .3};
        for (int i = 0; i < OTHER_NUM_ENVPTS; ++i )
        {
            otherenv.insertBreakpoint( OTHERENV_TIMES[i], OTHERENV_WEIGHTS[i] );
        }
      
        //  change: 
        testM.setFrequencyFunction( otherenv );
        
        //  verify:
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.3), otherenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.6), otherenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.9), otherenv.valueAt(.9) );
        
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.3), aenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.6), aenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.9), aenv.valueAt(.9) );
        
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.3), bwenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.6), bwenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.9), bwenv.valueAt(.9) );
        
        //  restore
        testM.setFrequencyFunction( fenv );
        
        //  change: 
        testM.setAmplitudeFunction( otherenv );
        
        //  verify:
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.3), fenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.6), fenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.9), fenv.valueAt(.9) );
        
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.3), otherenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.6), otherenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.9), otherenv.valueAt(.9) );
        
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.3), bwenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.6), bwenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.9), bwenv.valueAt(.9) );
        
        //  restore
        testM.setAmplitudeFunction( aenv );
        
        
        //  change: 
        testM.setBandwidthFunction( otherenv );
        
        //  verify:
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.3), fenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.6), fenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.frequencyFunction().valueAt(.9), fenv.valueAt(.9) );
        
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.3), aenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.6), aenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.amplitudeFunction().valueAt(.9), aenv.valueAt(.9) );
        
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.3), otherenv.valueAt(.3) );
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.6), otherenv.valueAt(.6) );
        SAME_ENV_VALUES( testM.bandwidthFunction().valueAt(.9), otherenv.valueAt(.9) );
        
        //  restore
        testM.setBandwidthFunction( bwenv );
        /*                                                */
        /*********** Partial morphing tests ***************/
        /*                                                */
        
        //  Fabricate two Partials and then use the Morpher
        // to construct a Morphed Partial. Evaluate the
        // parameters of that morphed Partial to verify
        // that they are correct. This is striaghtforward
        // for amplitude (with linear amp morphing) and
        // bandwidth, but frequency and phase are not as
        // simple. The frequency controls the morph in the
        // middle (morph function === .5) and the phase
        // controls it at the ends (morph function == 0
        // or 1), and in between its a combination of
        // the two, so there's no point in evaluating the 
        // frequency or phase at times when they are hard
        // to predict.
        
        
        //  and verify that the Morpher produces the correct morph:
        Partial p1 = makep1(), p2 = makep2();
        //  morph p1 and p2 to obtain a morphed Partials: 

        #define LABEL 2
        Partial pmorphed( testM.morphPartial( p1, p2, LABEL ) );
        

        //  the label should be as specified:
        TEST( pmorphed.label() == LABEL );
        
        // since the Partials don't have Breakpoints at the
        // same times, and since the morph functions don't have 
        // common segments at 1 or 0, the number of Breakpoints
        // in the morph should be equal to the sum of the number
        // in the sources:
        TEST( pmorphed.numBreakpoints() == p1.numBreakpoints() + p2.numBreakpoints() );

        #define SAME_PARAM_VALUES(x,y) TEST( float_equal((x),(y)) )
        
        // the morphed Partial should start at the start of p1 
        // and end at the end of p2:
        SAME_PARAM_VALUES( pmorphed.startTime(), p1.startTime() );
        SAME_PARAM_VALUES( pmorphed.endTime(), p2.endTime() );

        // the frequency should start at p1's start frequency,
        // end at p2's end frequency, and at time .5 should be 
        // equal to the average of the two Partial's frequencies:
        SAME_PARAM_VALUES( pmorphed.frequencyAt(0), p1.frequencyAt(0) );
        SAME_PARAM_VALUES( pmorphed.frequencyAt(p2.endTime()), p2.frequencyAt(p2.endTime()) );
        SAME_PARAM_VALUES( pmorphed.frequencyAt(0.5), 
                           0.5 * ( p1.frequencyAt(0.5) + p2.frequencyAt(0.5) ) );
        
        // the amplitude should start at p1's start amp, and be at
        // p2's amp after t==.4. At .2, it should be the average:       
        SAME_PARAM_VALUES( pmorphed.amplitudeAt(p1.startTime()), p1.amplitudeAt(p1.startTime()) );
        SAME_PARAM_VALUES( pmorphed.amplitudeAt(p2.endTime()), p2.amplitudeAt(p2.endTime()) );
        SAME_PARAM_VALUES( pmorphed.amplitudeAt(0.6), p2.amplitudeAt(0.6) );
        SAME_PARAM_VALUES( pmorphed.amplitudeAt(0.2), 
                           0.5 * ( p1.amplitudeAt(0.2) + p2.amplitudeAt(0.2) ) );
        
        // the bandwidth should start and end at p1's bandwidth, at
        // t==.5 it should be at p'2, and it should be the average
        // at t==.2 and .7:     
        SAME_PARAM_VALUES( pmorphed.bandwidthAt(0), p1.bandwidthAt(0) );
        SAME_PARAM_VALUES( pmorphed.bandwidthAt(1), p1.bandwidthAt(1) );
        SAME_PARAM_VALUES( pmorphed.bandwidthAt(0.5), p2.bandwidthAt(0.5) );
        SAME_PARAM_VALUES( pmorphed.bandwidthAt(0.2), 
                           0.5 * ( p1.bandwidthAt(0.2) + p2.bandwidthAt(0.2) ) );
        SAME_PARAM_VALUES( pmorphed.bandwidthAt(0.7), 
                           0.5 * ( p1.bandwidthAt(0.7) + p2.bandwidthAt(0.7) ) );
                
                
        // the phase _should_, ideally, be equal to p1 phase
        // before t==.2 and equalt to p2 phase after t==.8,
        // but unless there is enough time for the frequency
        // and phase to settle down, we won't achieve the 
        // correct phase at the end:
        SAME_PARAM_VALUES( pmorphed.phaseAt(.1), p1.phaseAt(.1) );
        SAME_PARAM_VALUES( pmorphed.phaseAt(.9), p2.phaseAt(.9) );
        
        /*                                                */
        /********* parameter morphing tests ***************/
        /*                                                */
        
        //  verify that morphing between two Partials at a given
        //  time yields the same results as morphing between two
        //  Breakpoints having the parameters of those two
        //  Partials at that same time:
        //
        //  for each Breakpoint in each Partial
        //      create a Breakpoint representing the parameters of
        //          the other Partial at that time
        //      morph the two Breakpoints
        //      compare to a morph between the two Partials at that time
        //
        Partial::iterator it;
        for ( it = p1.begin(); it != p1.end(); ++it )
        {
            //  get Breakpoints:
            Breakpoint & bp1 = it.breakpoint();
            double t = it.time();
            Breakpoint bp2( p2.frequencyAt(t), p2.amplitudeAt(t), p2.bandwidthAt(t), p2.phaseAt(t) );
            
            //  do morphs:
            Breakpoint m1, m2, m3;
            m1 = testM.morphBreakpoints( bp1, bp2, t );
            m2 = testM.morphSrcBreakpoint( bp1, p2, t );
            m3 = testM.morphTgtBreakpoint( bp2, p1, t );
            
            //  verify results:
            SAME_PARAM_VALUES( m1.frequency(), m2.frequency() );
            SAME_PARAM_VALUES( m2.frequency(), m3.frequency() );
            SAME_PARAM_VALUES( m1.amplitude(), m2.amplitude() );
            SAME_PARAM_VALUES( m2.amplitude(), m3.amplitude() );
            SAME_PARAM_VALUES( m1.bandwidth(), m2.bandwidth() );
            SAME_PARAM_VALUES( m2.bandwidth(), m3.bandwidth() );
            SAME_PARAM_VALUES( m1.phase(), m2.phase() );
            SAME_PARAM_VALUES( m2.phase(), m3.phase() );
        }

        for ( it = p2.begin(); it != p2.end(); ++it )
        {
            //  get Breakpoints:
            Breakpoint & bp2 = it.breakpoint();
            double t = it.time();
            Breakpoint bp1( p1.frequencyAt(t), p1.amplitudeAt(t), p1.bandwidthAt(t), p1.phaseAt(t) );
            
            //  do morphs:
            Breakpoint m1, m2, m3;
            m1 = testM.morphBreakpoints( bp1, bp2, t );
            m2 = testM.morphSrcBreakpoint( bp1, p2, t );
            m3 = testM.morphTgtBreakpoint( bp2, p1, t );
            
            //  verify results:
            SAME_PARAM_VALUES( m1.frequency(), m2.frequency() );
            SAME_PARAM_VALUES( m2.frequency(), m3.frequency() );
            SAME_PARAM_VALUES( m1.amplitude(), m2.amplitude() );
            SAME_PARAM_VALUES( m2.amplitude(), m3.amplitude() );
            SAME_PARAM_VALUES( m1.bandwidth(), m2.bandwidth() );
            SAME_PARAM_VALUES( m2.bandwidth(), m3.bandwidth() );
            SAME_PARAM_VALUES( m2pi( m1.phase() ), m2pi( m2.phase() ) );
            SAME_PARAM_VALUES( m2pi( m2.phase() ), m2pi( m3.phase() ) );
        }

        
        /*                                                      */
        /*********** dummy Partial morphing tests ***************/
        /*                                                      */
        
        //  test morphing to a dummy Partial, should just fade the
        //  real Partial in (amp envelope starts at 1):
        Partial to_dummy_by_hand = p1;
        for ( Partial::iterator dummy_iter = to_dummy_by_hand.begin(); 
              dummy_iter != to_dummy_by_hand.end(); 
              ++dummy_iter )
        {
            double t = dummy_iter.time();
            double a = dummy_iter.breakpoint().amplitude();
            dummy_iter.breakpoint().setAmplitude( (1.-aenv.valueAt(t)) * a );
        }
        to_dummy_by_hand.setLabel(3);

        //  morph p1 and a dummy to obtain a morphed Partial, and check its 
        //  parameters against those of to_dummy_by_hand at several times: 
        Partial to_dummy = testM.morphPartial( p1, Partial(), to_dummy_by_hand.label() );
        
        //  check:
        TEST( to_dummy.label() == to_dummy_by_hand.label() );
        TEST( to_dummy.numBreakpoints() == to_dummy_by_hand.numBreakpoints() );

        SAME_PARAM_VALUES( to_dummy.startTime(), to_dummy_by_hand.startTime() );
        SAME_PARAM_VALUES( to_dummy.endTime(), to_dummy_by_hand.endTime() );
        SAME_PARAM_VALUES( to_dummy.duration(), to_dummy_by_hand.duration() );
        
        SAME_PARAM_VALUES( to_dummy.frequencyAt(0), to_dummy_by_hand.frequencyAt(0) );
        SAME_PARAM_VALUES( to_dummy.amplitudeAt(0), to_dummy_by_hand.amplitudeAt(0) );
        SAME_PARAM_VALUES( to_dummy.bandwidthAt(0), to_dummy_by_hand.bandwidthAt(0) );
        SAME_PARAM_VALUES( m2pi( to_dummy.phaseAt(0) ), m2pi( to_dummy_by_hand.phaseAt(0) ) );
        
        SAME_PARAM_VALUES( to_dummy.frequencyAt(0.1), to_dummy_by_hand.frequencyAt(0.1) );
        SAME_PARAM_VALUES( to_dummy.amplitudeAt(0.1), to_dummy_by_hand.amplitudeAt(0.1) );
        SAME_PARAM_VALUES( to_dummy.bandwidthAt(0.1), to_dummy_by_hand.bandwidthAt(0.1) );
        SAME_PARAM_VALUES( m2pi( to_dummy.phaseAt(0.1) ), m2pi( to_dummy_by_hand.phaseAt(0.1) ) );
        
        SAME_PARAM_VALUES( to_dummy.frequencyAt(0.3), to_dummy_by_hand.frequencyAt(0.3) );
        SAME_PARAM_VALUES( to_dummy.amplitudeAt(0.3), to_dummy_by_hand.amplitudeAt(0.3) );
        SAME_PARAM_VALUES( to_dummy.bandwidthAt(0.3), to_dummy_by_hand.bandwidthAt(0.3) );
        SAME_PARAM_VALUES( m2pi( to_dummy.phaseAt(0.3) ), m2pi( to_dummy_by_hand.phaseAt(0.3) ) );
        
        SAME_PARAM_VALUES( to_dummy.frequencyAt(0.6), to_dummy_by_hand.frequencyAt(0.6) );
        SAME_PARAM_VALUES( to_dummy.amplitudeAt(0.6), to_dummy_by_hand.amplitudeAt(0.6) );
        SAME_PARAM_VALUES( to_dummy.bandwidthAt(0.6), to_dummy_by_hand.bandwidthAt(0.6) );
        SAME_PARAM_VALUES( m2pi( to_dummy.phaseAt(0.6) ), m2pi( to_dummy_by_hand.phaseAt(0.6) ) );
        
        SAME_PARAM_VALUES( to_dummy.frequencyAt(0.85), to_dummy_by_hand.frequencyAt(0.85) );
        SAME_PARAM_VALUES( to_dummy.amplitudeAt(0.85), to_dummy_by_hand.amplitudeAt(0.85) );
        SAME_PARAM_VALUES( to_dummy.bandwidthAt(0.85), to_dummy_by_hand.bandwidthAt(0.85) );
        SAME_PARAM_VALUES( m2pi( to_dummy.phaseAt(0.85) ), m2pi( to_dummy_by_hand.phaseAt(0.85) ) );
        
        SAME_PARAM_VALUES( to_dummy.frequencyAt(1), to_dummy_by_hand.frequencyAt(1) );
        SAME_PARAM_VALUES( to_dummy.amplitudeAt(1), to_dummy_by_hand.amplitudeAt(1) );
        SAME_PARAM_VALUES( to_dummy.bandwidthAt(1), to_dummy_by_hand.bandwidthAt(1) );
        SAME_PARAM_VALUES( m2pi( to_dummy.phaseAt(1) ), m2pi( to_dummy_by_hand.phaseAt(1) ) );
        
        //  test morphing from a dummy Partial, should just fade the
        //  real Partial out (amp envelope starts at 1):
        Partial from_dummy_by_hand = p1;
        for ( Partial::iterator dummy_iter = from_dummy_by_hand.begin(); 
              dummy_iter != from_dummy_by_hand.end(); 
              ++dummy_iter )
        {
            double t = dummy_iter.time();
            double a = dummy_iter.breakpoint().amplitude();
            dummy_iter.breakpoint().setAmplitude( aenv.valueAt(t) * a );

            /*
            cout << "from dummy by hand has bp at time " << t
                 << " morphing functions are at " << fenv.valueAt(t) << ", "
                 << aenv.valueAt(t) << ", " << bwenv.valueAt(t) << endl;
            */
        }
        // remove the first Breakpoint, which won't be added in 
        // the morphing process, because all the morphing functions
        // are near 0 at that time:
        from_dummy_by_hand.erase( from_dummy_by_hand.begin() );
        from_dummy_by_hand.setLabel(4);

        //  morph p1 and a dummy to obtain a morphed Partial, and check its 
        //  parameters against those of from_dummy_by_hand at several times: 
        Partial from_dummy = testM.morphPartial( Partial(), p1, from_dummy_by_hand.label() );
        
        //  check:
        TEST( from_dummy.label() == from_dummy_by_hand.label() );
        TEST( from_dummy.numBreakpoints() == from_dummy_by_hand.numBreakpoints() );

        SAME_PARAM_VALUES( from_dummy.startTime(), from_dummy_by_hand.startTime() );
        SAME_PARAM_VALUES( from_dummy.endTime(), from_dummy_by_hand.endTime() );
        SAME_PARAM_VALUES( from_dummy.duration(), from_dummy_by_hand.duration() );
        
        SAME_PARAM_VALUES( from_dummy.frequencyAt(0), from_dummy_by_hand.frequencyAt(0) );
        SAME_PARAM_VALUES( from_dummy.amplitudeAt(0), from_dummy_by_hand.amplitudeAt(0) );
        SAME_PARAM_VALUES( from_dummy.bandwidthAt(0), from_dummy_by_hand.bandwidthAt(0) );
        SAME_PARAM_VALUES( m2pi( from_dummy.phaseAt(0) ), m2pi( from_dummy_by_hand.phaseAt(0) ) );
        
        SAME_PARAM_VALUES( from_dummy.frequencyAt(0.1), from_dummy_by_hand.frequencyAt(0.1) );
        SAME_PARAM_VALUES( from_dummy.amplitudeAt(0.1), from_dummy_by_hand.amplitudeAt(0.1) );
        SAME_PARAM_VALUES( from_dummy.bandwidthAt(0.1), from_dummy_by_hand.bandwidthAt(0.1) );
        SAME_PARAM_VALUES( m2pi( from_dummy.phaseAt(0.1) ), m2pi( from_dummy_by_hand.phaseAt(0.1) ) );
        
        SAME_PARAM_VALUES( from_dummy.frequencyAt(0.3), from_dummy_by_hand.frequencyAt(0.3) );
        SAME_PARAM_VALUES( from_dummy.amplitudeAt(0.3), from_dummy_by_hand.amplitudeAt(0.3) );
        SAME_PARAM_VALUES( from_dummy.bandwidthAt(0.3), from_dummy_by_hand.bandwidthAt(0.3) );
        SAME_PARAM_VALUES( m2pi( from_dummy.phaseAt(0.3) ), m2pi( from_dummy_by_hand.phaseAt(0.3) ) );
        
        SAME_PARAM_VALUES( from_dummy.frequencyAt(0.6), from_dummy_by_hand.frequencyAt(0.6) );
        SAME_PARAM_VALUES( from_dummy.amplitudeAt(0.6), from_dummy_by_hand.amplitudeAt(0.6) );
        SAME_PARAM_VALUES( from_dummy.bandwidthAt(0.6), from_dummy_by_hand.bandwidthAt(0.6) );
        SAME_PARAM_VALUES( m2pi( from_dummy.phaseAt(0.6) ), m2pi( from_dummy_by_hand.phaseAt(0.6) ) );
        
        SAME_PARAM_VALUES( from_dummy.frequencyAt(0.85), from_dummy_by_hand.frequencyAt(0.85) );
        SAME_PARAM_VALUES( from_dummy.amplitudeAt(0.85), from_dummy_by_hand.amplitudeAt(0.85) );
        SAME_PARAM_VALUES( from_dummy.bandwidthAt(0.85), from_dummy_by_hand.bandwidthAt(0.85) );
        SAME_PARAM_VALUES( m2pi( from_dummy.phaseAt(0.85) ), m2pi( from_dummy_by_hand.phaseAt(0.85) ) );
        
        SAME_PARAM_VALUES( from_dummy.frequencyAt(1), from_dummy_by_hand.frequencyAt(1) );
        SAME_PARAM_VALUES( from_dummy.amplitudeAt(1), from_dummy_by_hand.amplitudeAt(1) );
        SAME_PARAM_VALUES( from_dummy.bandwidthAt(1), from_dummy_by_hand.bandwidthAt(1) );
        SAME_PARAM_VALUES( m2pi( from_dummy.phaseAt(1) ), m2pi( from_dummy_by_hand.phaseAt(1) ) );
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
    cout << "Morpher passed all tests." << endl;
    return 0;
}

// helpers:

// ---------------------------------------------------------------------------
//  phaseTravel
//
//  Compute the sinusoidal phase travel between two Breakpoints.
//  Return the total unwrapped phase travel.
//
static double phaseTravel( Partial::const_iterator bp0, Partial::const_iterator bp1 )
{
    double f0 = bp0->frequency();
    double t0 = bp0.time();
    double f1 = bp1->frequency();
    double t1 = bp1.time();
    double favg = .5 * ( f0 + f1 );
    double dt = t1 - t0;
    return 2 * Pi * favg * dt;
}
// ---------------------------------------------------------------------------
//  computePhaseFwd
//
/// Fix all the phases in a half-open Partial::iterator range
/// by computing correct phases from the phase of the first
/// Breakpoint in the range and the Breakpoint frequencies.
///
/// The phases of the Breakpoints in the iterator range 
/// are recalculated by computing the phase travel
/// that would be synthesized between pairs of Breakpoints 
/// according to the frequencies of those Breakpoints, and
/// updating the phase of the later Breakpoint in the pair.
/// The phase of the first Breakpoint is unchanged, phases 
/// of other Breakpoints in the half-open range are 
/// recomputed. Other Breakpoint parameters are unaltered.
///
///     computePhaseFwd( p.begin(), p.end() )
///     
/// recomputes all Breakpoint phases in the Partial p from
/// the phase of the first Breakpoint.
///
/// \pre      The position b must be a valid Breakpoint position
///           in a Partial and the position e must be reachable
///           by incrementing b.
/// \param b  The position of the first Breakpoint in a range
///           whose phases will be recomputed. The phase of the
///           Breakpoint at this position is unchanged.
/// \param e  The position marking the end of a half open range
///           of Breakpoints whose phases will be recomputed.
//
static void computePhaseFwd( Partial::iterator b, Partial::iterator e )
{
    Partial::iterator nxt = b;
    if ( nxt != e )
    {
        for ( Partial::iterator cur = nxt++; nxt != e; cur = nxt++ )
        {
            double dphase = phaseTravel( cur, nxt );
            nxt.breakpoint().setPhase( m2pi( cur.breakpoint().phase() + dphase ) );
        }
    }
}
