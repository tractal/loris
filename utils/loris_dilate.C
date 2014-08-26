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
 * loris_synthesize.C
 *
 * main() function for a utility program to render Partials
 * stored in a SDIF file, optionally with dilation and amplitude
 * and frequency scaling by a constant factor.
 *
 * Kelly Fitz, 20 Dec 2004
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <algorithm>
using std::for_each;

#include <cstdlib>
using std::strtod;

#include <iostream>
using std::cout;
using std::endl;

#include <stdexcept>
using std::domain_error;

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <Dilator.h>
#include <Marker.h>
#include <PartialList.h>
#include <PartialUtils.h>
#include <SdifFile.h>

using namespace Loris;

//  function prototypes
void parseArguments( int nargs, char * args[] );
void printUsage( const char * programName );

//  global state
double FreqScale = 1.;
double AmpScale = 1.;
string Outname = "dilated.sdif";
vector< double > markers, times;

int main( int argc, char * argv[] )
{   
    if ( argc < 3 )
    {
        printUsage( argv[0] );
        return 1;
    }
    
    //  get the name of the file to synthesize
    string fname = argv[1];
    
    //  parse the other arguments
    parseArguments( argc - 2, argv + 2 );
    
    //  import and prepare the Partials
    SdifFile fin( fname );
    PartialList & partials = fin.partials();
    for ( int i = 0; i < fin.markers().size(); ++i )
    {
        markers.push_back( fin.markers()[ i ].time() );
    }
    
    if ( 0 == times.size() )
    {
        cout << "You must specify the times (in seconds) to be used for dilation." << endl;
        printUsage( argv[0] );
        return 1;
    }

    if ( ! fin.markers().empty() )
    {
        cout << "Features marked in " << fname << " before dilating:\n";
        //  print out the markers:
        std::vector< Marker >::iterator it;
        for ( it = fin.markers().begin(); it != fin.markers().end(); ++it )
        {
            cout << it->time() << "\t\"" << it->name() << "\"\n";
        }
    }
    
    if ( times.size() == markers.size() )
    {
        cout << "Dilating partials using " << times.size() 
             << " marked features." << endl;
        Dilator dilator( markers.begin(), markers.end(), times.begin() );
        dilator.dilate( partials.begin(), partials.end() );
        dilator.dilate( fin.markers().begin(), fin.markers().end() );
    }
    else if ( 1 == times.size() )
    {
        double dur = 
            PartialUtils::timeSpan( partials.begin(), partials.end() ).second;
        cout << "Scaling duration from " << dur << " to " << times.front() 
             << " seconds" << endl;
        Dilator dilator( &dur, (&dur) + 1, times.begin() );
        dilator.dilate( partials.begin(), partials.end() );
        dilator.dilate( fin.markers().begin(), fin.markers().end() );
    }
    else
    {
       cout << "Specified time points need to correspond to Markers "
            << "in " << fname << "." << endl;
        printUsage( argv[0] );
        return 1;
    }

    if ( ! fin.markers().empty() )
    {
        cout << "Features marked in " << fname << " after dilating:\n";
        //  print out the markers:
        std::vector< Marker >::iterator it;
        for ( it = fin.markers().begin(); it != fin.markers().end(); ++it )
        {
            cout << it->time() << "\t\"" << it->name() << "\"\n";
        }
    }

    if ( 1. != FreqScale )
    {
       cout << "Scaling partial frequencies by " << FreqScale << endl;
       PartialUtils::scaleFrequency( partials.begin(), partials.end(), FreqScale );
    }
    if ( 1. != AmpScale )
    {
       cout << "Scaling partial amplitudes by " << AmpScale << endl;
       PartialUtils::scaleAmplitude( partials.begin(), partials.end(), AmpScale );
    }
    //  render the Partials
    cout << "Exporting " << partials.size() << " dilated partials to " << Outname << endl;
    SdifFile fout( partials.begin(), partials.end() );
    fout.markers() = fin.markers();
    
    //  export the samples 
    fout.write( Outname );  
    
    cout << "* Done." << endl;

    return 0;
}

static double getFloatArg( const char * arg )
{   
    char * endptr;
    double x = strtod( arg, &endptr );
    if ( endptr == arg )    
    {
        cout << "Error processing argument: " << arg << endl;
        throw domain_error( "bad argument" );
    }   
    return x;
}

void parseArguments( int nargs, char * args[] )
{
    while ( nargs > 0 )
    {
        string arg  = *args;
        if ( arg[0] == '-' )
        {
            ++args;
            --nargs;
            //  process an option
            if  ( arg == "-freq" )
            {
                FreqScale = getFloatArg( *args );
                ++args;
                --nargs;
            }
            else if ( arg == "-amp" )
            {
                AmpScale = getFloatArg( *args );
                ++args;
                --nargs;
            }
            else if ( arg == "-o" )
            {
                Outname = *args;
                ++args;
                --nargs;
            }
            else
            {
                cout << "Unrecognized argument: " << arg << endl;
                cout << "Ignoring the rest." << endl;
                return;
            }
        }
        else
        {
            //  all the remaining command line args 
            //  should be times
            while( nargs > 0 )
            {
                times.push_back( getFloatArg( *args ) );
                ++args;
                --nargs;
            }
            
        }
    }
}

void printUsage( const char * programName )
{
    cout << "usage: " << programName << " filename.sdif [options] times" << endl;
    cout << "options:" << endl;
    cout << "-freq <frequency scale factor>" << endl;
    cout << "-amp <amplitude scale factor>" << endl;
    cout << "-o <output SDIF file name, default is dilated.sdif>" << endl;
    cout << "\nTimes (any non-zero number) are used for dilation." << endl;
    cout << "The specified times must all correspond to " << endl;
    cout << "Markers in the SDIF file. If only a single time is" << endl;       
    cout << "specified, and the SDIF file has no Markers or more" << endl;
    cout << "than one, the specified time is used as the overall " << endl;
    cout << "duration of the uniformly-dilated partials." << endl;
}

