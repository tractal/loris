//  test_Aiff.cc
//

#include "AiffData.h"
#include "AiffFile.h"
#include "Marker.h"

#include "Analyzer.h"
#include "BreakpointEnvelope.h"
#include "Channelizer.h"
#include "Dilator.h"
#include "Distiller.h"
#include "Exception.h"
#include "FrequencyReference.h"
#include "Partial.h"
#include "PartialList.h"
#include "PartialUtils.h"

// #include "SpcFile.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

using std::cout;
using std::endl;

using namespace Loris;

//  -------------------------------------------------------
//  make_twoMarkers
//
//  twoMarkers.aiff is the clarinet sample, 103886 samples, 
//  with markers at 2s (Marker 1) and 1s (Marker 2).
//
static std::string make_twoMarkers( const std::string & in_fname,
                             const std::string & fname = "twoMarkers.ctest.aiff" )
{
    cout << "Reading " << in_fname << " and adding two markers to it.\n";
    AiffFile f( in_fname );
    f.markers().push_back( Marker( 2, "Marker 1" ) );
    f.markers().push_back( Marker( 1, "Marker2" ) );
    if ( (f.samples().size() % 2) == 0 )
        f.samples().push_back(0);
    f.write( fname, 24 );
    cout << "Done." << endl;
    return fname;
    
}

int main( int argc, char * argv[] )
{
    std::string in_fname;
    std::string path("");   
    if ( argc < 2 )
    {
        if ( std::getenv("srcdir") ) 
        {
            path = std::getenv("srcdir");
            path = path + "/";
        }

        in_fname = path + "clarinet.aiff";
        std::cout << "I need a filename, using " << in_fname << std::endl;
    }
    else
    {
        in_fname = argv[1];
    }
    
    try
    {       
        std::string fname = make_twoMarkers( in_fname );
        AiffFile f( fname );

        cout << "Found " << f.samples().size() << " samples." << endl;
        
        for ( int i = 0; i < 10; ++i )
            cout << f.samples()[i] << ", ";
        cout << "..." << endl;
        
        cout << "Sample rate is: " << f.sampleRate() << "." << endl;
        cout << "MIDI note number is: " << f.midiNoteNumber() << "." << endl;
        
        cout << "There are " << f.markers().size() << " markers." << endl;
        for ( AiffFile::markers_type::iterator it = f.markers().begin(); 
              it != f.markers().end(); 
              ++it )
        {
            Marker & m = *it;
            cout << m.name() << " at time " << m.time() << endl;
        }
        
        std::vector< Byte > bytes;
        const int BPS = 16;
        convertSamplesToBytes( f.samples(), bytes, BPS );
        std::vector< double > dbls;
        convertBytesToSamples( bytes, dbls, BPS );
        cout << "Made " << dbls.size() <<   " identical samples?" << endl;

        // compare:
        cout << "compare:" << endl;
        for ( int i = 0; i < 10; ++i )
            cout << f.samples()[i] << "\t" << dbls[i] << "\n";
        cout << "..." << endl;
        

        // analyze clarinet, don't do this if it isn't the clarinet!
        cout << "analyzing clarinet 4G#" << endl;
        Analyzer a(415*.8, 415*1.6);
        a.analyze( f.samples(), f.sampleRate() );
        PartialList & clar = a.partials();
        
        FrequencyReference clarRef( clar.begin(), clar.end(), 0, 1000, 20 );
        Channelizer ch( clarRef.envelope() , 1 );
        ch.channelize( clar.begin(), clar.end() );
        
        Distiller still;
        still.distill( clar );

        cout << "analyzed, found " << clar.size() << " partials" << endl;
        
        Partial & fund = *(std::find_if( clar.begin(), clar.end(), PartialUtils::isLabelEqual(1) ));
        cout << "fundamental appears to be about " << fund.frequencyAt(1) << " Hz." << endl;
        
        Partial & second = *(std::find_if( clar.begin(), clar.end(), PartialUtils::isLabelEqual(2) ));
        cout << "second harmonic appears to be about " << second.frequencyAt(1) << " Hz." << endl;
        cout << "(they should be around 415 and 830)" << endl;
        
        //  add a Partial
        Partial p;
        p.insert( 0.5, Breakpoint( 100, 0.1, 0 ) );
        p.insert( 1.5, Breakpoint( 500, 0.1, 0 ) );
        p.setLabel( clar.size() * 2 );
        f.addPartial( p, 0.2 );
        
        f.markers().push_back( Marker( 0.5, "hey!" ) );
        f.setMidiNoteNumber( 48 );
        f.write( std::string("new") + fname, BPS );
        
        /*
        double endtime = 0.75 * 
            PartialUtils::timeSpan( clar.begin(), clar.end() ).second;
        cout << "cropping to " << endtime << " seconds." << endl;
        PartialUtils::crop( clar.begin(), clar.end(), 0, endtime );
        
        cout << "exporting " << clar.size() 
             << " cropped sinusoidal spc partials" << endl;
        SpcFile spc( clar.begin(), clar.end(), 68 );
        spc.addPartial( p );
        spc.markers() = f.markers();
        std::sort( spc.markers().begin(), spc.markers().end(), Marker::sortByTime() );
        spc.write( "spcSines.ctest.spc", false );
        spc.write( "spcEnhanced.ctest.spc", true );
        
        //  import them again
        SpcFile reload( "spcEnhanced.ctest.spc" );
        cout << "reloaded " << reload.partials().size() << " spc partials." 
             << " (should be next PO2 after " << p.label() << ")" << endl;

        std::pair< double, double > span = 
            PartialUtils::timeSpan( clar.begin(), clar.end() );
        cout << "time span is " << span.first << " to "
             << span.second << " seconds." << endl;
        */
        
	AiffFile reload( std::string("new") + fname );
        cout << "There are " << reload.markers().size() << " markers." << endl;
        for ( AiffFile::markers_type::iterator it = reload.markers().begin(); 
              it != reload.markers().end(); 
              ++it )
        {
            Marker & m = *it;
            cout << m.name() << " at time " << m.time() << endl;
        }
        
        //  finally, make sure that we can read spc files 
        //  generated by Kyma, too.
        // SpcFile duh(path + "fromKyma.spc");
    }
    catch( Loris::Exception & ex )
    {
        std::cout << ex.what() << std::endl;
        return 1;
    }
    return 0;
}

