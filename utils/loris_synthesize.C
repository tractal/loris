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

#include <cmath>
using std::log;

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

#include <AiffFile.h>
#include <Dilator.h>
#include <Marker.h>
#include <PartialList.h>
#include <PartialUtils.h>
#include <SdifFile.h>
#include <SpcFile.h>

using namespace Loris;

//  function prototypes
void parseArguments( int nargs, char * args[] );
void printUsage( const char * programName );

//  global state 
//  (Geez, this is lousy programming. I'd flunk my students for doing this.)
double Rate = 44100;
double FreqScale = 1.;
double AmpScale = 1.;
double BwScale = 1.;
string Outname = "synth.aiff";
vector< double > marker_times, cmdline_times;

int main( int argc, char * argv[] )
{   
    if ( argc < 2 )
    {
        printUsage( argv[0] );
        return 1;
    }
    
    //  get the filename and its suffix
    string filename( argv[1] );
    string suffix = filename.substr( filename.rfind('.')+1 );

    // ----------- read Partials and Markers ---------------    
            
    PartialList partials;
    std::vector< Marker > markers;
    double midiNN = 0;
    
    if ( suffix == "sdif" )
    {
        try
        {
            SdifFile f( filename );
            cout << "SDIF partials file \"" << filename << "\":" << endl;
            std::pair< double, double > span = 
                PartialUtils::timeSpan( f.partials().begin(), f.partials().end() );
            cout << f.partials().size() << " partials spanning " << span.first;
            cout << " to " << span.second << " seconds.\n";
            partials.insert( partials.begin(), f.partials().begin(), f.partials().end() );
            markers.insert( markers.begin(), f.markers().begin(), f.markers().end() );
        }
        catch( Exception & ex )
        {
            cout << "Error reading markers from file: " << filename << "\n";
            cout << ex.what() << "\n";
            return 1;
        }
    }
    else if ( suffix == "spc" )
    {
        try
        {
            SpcFile f( filename );
            cout << "Spc partials file \"" << filename << "\":" << endl;
            std::pair< double, double > span = 
                PartialUtils::timeSpan( f.partials().begin(), f.partials().end() );
            cout << f.partials().size() << " partials spanning " << span.first;
            cout << " to " << span.second << " seconds.\n";
            cout << "MIDI note number " << f.midiNoteNumber() << endl;
            midiNN = f.midiNoteNumber();
            partials.insert( partials.begin(), f.partials().begin(), f.partials().end() );
            markers.insert( markers.begin(), f.markers().begin(), f.markers().end() );
        }
        catch( Exception & ex )
        {
            cout << "Error reading markers from file: " << filename << "\n";
            cout << ex.what() << "\n";
            return 1;
        }
    }
    else
    {
        cout << "Error -- unrecognized suffix: " << suffix << "\n";
        return 1;
    }   
    
    //  parse the other arguments
    parseArguments( argc - 2, argv + 2 );
    
    // ----------- dilate ---------------   

    for ( int i = 0; i < markers.size(); ++i )
    {
        marker_times.push_back( markers[ i ].time() );
    }
    
    if ( 0 < cmdline_times.size() )
    {
        if ( ! markers.empty() )
        {
            cout << "Features marked in " << filename << " before dilating:\n";
            //  print out the marker_times:
            std::vector< Marker >::iterator it;
            for ( it = markers.begin(); it != markers.end(); ++it )
            {
                cout << it->time() << "\t\"" << it->name() << "\"\n";
            }
        }

        if ( cmdline_times.size() == marker_times.size() )
        {
            cout << "Dilating partials using " << cmdline_times.size() 
                 << " marked features." << endl;
            Dilator dilator( marker_times.begin(), marker_times.end(), cmdline_times.begin() );
            dilator.dilate( partials.begin(), partials.end() );
            dilator.dilate( markers.begin(), markers.end() );
        }
        else if ( 1 == cmdline_times.size() )
        {
            double dur = 
                PartialUtils::timeSpan( partials.begin(), partials.end() ).second;
            cout << "Scaling duration from " << dur << " to " << cmdline_times.front() 
                 << " seconds" << endl;
            Dilator dilator( &dur, (&dur) + 1, cmdline_times.begin() );
            dilator.dilate( partials.begin(), partials.end() );
            dilator.dilate( markers.begin(), markers.end() );
        }
        else
        {
           cout << "Specified time points need to correspond to Markers "
                << "in " << filename << ", ignoring." << endl;
            printUsage( argv[0] );
            return 1;
        }

        if ( ! markers.empty() )
        {
            cout << "Features marked in " << filename << " after dilating:\n";
            //  print out the marker_times:
            std::vector< Marker >::iterator it;
            for ( it = markers.begin(); it != markers.end(); ++it )
            {
                cout << it->time() << "\t\"" << it->name() << "\"\n";
            }
        }
    }
    
    if ( 1. != FreqScale )
    {
       cout << "Scaling partial frequencies by " << FreqScale << endl;
       PartialUtils::scaleFrequency( partials.begin(), partials.end(), FreqScale );
       
       if ( 0 != midiNN )
       {
          double newNN = midiNN + 12 * ( log( FreqScale ) / log( 2.0 ) );
          cout << "Adjusting Midi Note Number from " << midiNN 
               << " to " << newNN << endl;
          midiNN = newNN;
       }
    }
    
    if ( 1. != AmpScale )
    {
       cout << "Scaling partial amplitudes by " << AmpScale << endl;
       PartialUtils::scaleAmplitude( partials.begin(), partials.end(), AmpScale );
    }
    
    if ( 1. != BwScale )
    {
       cout << "Scaling partial bandwidths by " << BwScale << endl;
       PartialUtils::scaleBandwidth( partials.begin(), partials.end(), BwScale );
    }
    
    //  render the Partials
    cout << "Rendering " << partials.size() << " partials at "
         << Rate << " Hz." << endl;
    AiffFile fout( partials.begin(), partials.end(), Rate );
    fout.markers() = markers;
    if ( 0 != midiNN )
    {
       fout.setMidiNoteNumber( midiNN );
    }
    
    //  export the samples 
    cout << "Exporting to " << Outname << endl;
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
            if ( arg == "-rate" )
            {
                Rate = getFloatArg( *args );
                ++args;
                --nargs;
            }
            else if ( arg == "-freq" )
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
            else if ( arg == "-bw" )
            {
                BwScale = getFloatArg( *args );
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
            //  should be cmdline_times
            while( nargs > 0 )
            {
                cmdline_times.push_back( getFloatArg( *args ) );
                ++args;
                --nargs;
            }
            
        }
    }
}

void printUsage( const char * programName )
{
    cout << "usage: " << programName << " filename.sdif [options] [cmdline_times]" << endl;
    cout << "options:" << endl;
    cout << "-rate <sample rate in Hz>" << endl;
    cout << "-freq <frequency scale factor>" << endl;
    cout << "-amp <amplitude scale factor>" << endl;
    cout << "-bw <bandwidth scale factor>" << endl;
    cout << "-o <output AIFF file name, default is synth.aiff>" << endl;
    cout << "\nOptional cmdline_times (any number) are used for dilation." << endl;
    cout << "If cmdline_times are specified, they must all correspond to " << endl;
    cout << "Markers in the SDIF file. If only a single time is" << endl;       
    cout << "specified, and the SDIF file has no Markers or more" << endl;
    cout << "than one, the specified time is used as the overall duration" << endl;
    cout << "of the uniformly-dilated synthesis." << endl;
}

