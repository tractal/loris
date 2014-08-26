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
 *  test_Filter.C
 *
 *  Verify that the digital filter class (Filter) is working correctly.
 *
 * Kelly Fitz, 9 Oct 2009
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
 #include "Filter.h"
 
#include <cmath>
 #include <iostream>

using namespace std;
using namespace Loris;

//  tacky global error variable
int ERR = 0;


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
}

// ------------------- random_input_check_output ---------------------------
//
//  Run a pseudo-random signal through the filter, verify the output.

static int random_input_check_output( void )
{
    cout << "Random signal I/O test." << endl;
    
    enum { NSAMPS = 20 };
    
    const double x[NSAMPS] = { 
    	0.936935655012751, 0.038788797332850289, -3.3905846945159652, 0.63512194733877647, -0.95466701587583913, -1.4158189130418939, 1.172563134238223, -0.33188671804999648, -2.3810993167665941, 2.3449771750569814, -0.15130209563149374, 0.21705681062905663, 1.52058260494527, 0.51183339499885039, -1.0326941787595467, 0.11269207950479941, -1.3981235484992118, -0.65981814247037818, -1.3639575892248843, 1.4780184095953759 
    };    
    const double y1ref[NSAMPS] = { 
    	0.84324208951147595, -1.8108533227755543, 1.5951524811091939, 4.7975977507386967, -10.694351857545564, 5.3030467438975357, -17.778505791335014, 8.9754345596335092, -32.25708182680183, 37.726304442600771, -75.461502298578026, 95.090535385478447, -151.5894902205132, 216.54318261258194, -327.03502610452892, 490.04592954826938, -727.81116395927131, 1084.2931847451682, -1617.237743011206, 2401.5428267364841 
    };
    const double y2ref[NSAMPS] = { 
    	0.49077581929239339, 0.14566760959102207, -1.8100850952755096, -0.36366526867073734, -0.4706851836695024, -0.075011441052535655, 1.0880509268008796, 0.47353306131082279, -1.1213704247988554, 0.46044085473080498, -0.15428157176981891, 0.36456819005594143, 0.84216423350531611, 0.47178017069782047, -0.68577114358346958, -0.60194523802503608, -1.1428952901667246, -0.38224994316913324, -0.32397484846462088, 1.3598462795041937
    };
    const double y3ref[NSAMPS] = { 
    	0.23423391375318775, 0.57185859234086311, -1.3865342882369318, -2.1330775220277829, 2.1670600318106721, -0.46018193256025153, -0.14233084157477127, 1.7087243028245309, -1.1439900123041067, -0.93642404902526888, 2.880792050698771, -0.84822853056417236, 0.014916701228402385, 0.947899349247314, -0.91768727331500255, -1.2786891756151559, 0.20974251908409214, -0.81327036773011452, 0.073820826434879305, 0.29655182147094317
    };
    
    const double EPS = 1E-12;

    //  try a variety of filters having the same and different numbers
    //  of numerator and denominator coefficients
    enum { N = 4 };
    
    const double B1[N] = { 0.9, -1.7, 3.1, 2.0 };
    const double A1[N] = { 1.0, 0.3, -1.5, 0.4 };

    const double B2[N] = { 1.1, -0.4, 0, 0 };
    const double A2[N] = { 2.1, -1.3, 0.5, 0.8 };

    const double B3[N] = { 0.25, 0.6, -0.6, -0.25 };
    const double A3[N] = { 1.0, 0, 0, 0 };


    cout << "--- filter 1 ---" << endl;
    Filter f1( B1, B1+N, A1, A1+N );
    for ( unsigned int k = 0; k < NSAMPS; ++k )
    {
        float_abs_equal( f1.apply( x[k] ), y1ref[k], EPS );
    }
    cout << "--- filter 2 ---" << endl;
    Filter f2( B2, B2+N, A2, A2+N );
    for ( unsigned int k = 0; k < NSAMPS; ++k )
    {
        float_abs_equal( f2.apply( x[k] ), y2ref[k], EPS );
    }
    cout << "--- filter 3 ---" << endl;
    Filter f3( B3, B3+N, A3, A3+N );
    for ( unsigned int k = 0; k < NSAMPS; ++k )
    {
        float_abs_equal( f3.apply( x[k] ), y3ref[k], EPS );
    }
    
    cout << "Done." << endl;
}


// ----------- main -----------
//
int main( void )
{
    std::cout << "Test of Loris digital filter class." << endl;
    std::cout << "Built: " << __DATE__ << endl << endl;
    
    
    
    try 
    {
        random_input_check_output( );


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
        cout << "Filter passed all tests." << endl;
    }
    else
    {
        cout << "Filter FAILED tests." << endl;
    }
    return ERR;
}