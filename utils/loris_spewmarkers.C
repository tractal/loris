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
 * loris_spewmarkers.C
 *
 * main() function for a utility program to read the
 * markers stored in a AIFF, SPC, or SDIF file and
 * print them to standard output.
 *
 * Kelly Fitz, 12 May 2004
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
#include <iostream>
#include <string>
#include <vector>

#include "AiffFile.h"
#include "Exception.h"
#include "Marker.h"
#include "SdifFile.h"
#include "SpcFile.h"

using std::cout;
using std::string;
using std::vector;
using namespace Loris;

int main( int argc, char* argv[] )
{
    //  check for a single argument
    if ( argc != 2 )
    {
        cout << "usage:\t" << argv[0] << " filename\n\n";
        return 1;
    }   
    
    //  get the filename and its suffix
    string filename( argv[1] );
    string suffix = filename.substr( filename.rfind('.')+1 );
    
    //cout << "filename is " << filename << "\n";
    //cout << "suffix is " << suffix << "\n";
    
    std::vector< Marker > markers;
    if ( suffix == "aiff" || suffix == "aif" )
    {
        try
        {
            AiffFile f( filename );
            cout << "AIFF samples file \"" << filename << "\":" << endl;
            cout << f.numFrames() << " mono samples at " << f.sampleRate() << " Hz\n";
            cout << "(Duration " << f.numFrames() / f.sampleRate() << " seconds)\n";
            cout << "MIDI note number " << f.midiNoteNumber() << endl;
            markers.insert( markers.begin(), f.markers().begin(), f.markers().end() );
        }
        catch( Exception & ex )
        {
            cout << "Error reading markers from file: " << filename << "\n";
            cout << ex.what() << "\n";
            return 1;
        }
    }
    else if ( suffix == "sdif" )
    {
        try
        {
            SdifFile f( filename );
            cout << "SDIF partials file \"" << filename << "\":" << endl;
            std::pair< double, double > span = 
                PartialUtils::timeSpan( f.partials().begin(), f.partials().end() );
            cout << f.partials().size() << " partials spanning " << span.first;
            cout << " to " << span.second << " seconds.\n";
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
    
    if ( markers.empty() )
    {
        cout << "No markers found in " << filename << "\n";
    }
    else
    {
        cout << "Features marked in " << filename << ":\n";
        //  print out the markers:
        std::vector< Marker >::iterator it;
        for ( it = markers.begin(); it != markers.end(); ++it )
        {
            cout << it->time() << "\t\"" << it->name() << "\"\n";
        }
    }
    
    cout << "* Done." << endl;
    return 0;
}

