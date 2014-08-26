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
 *	loris.i
 *
 *  SWIG interface file for building scripting language modules
 *  implementing Loris functionality. This interface has been 
 *  completely rewritten (23 Jan 2003) to support new versions
 *  of SWIG (current is 1.3.17) and take advantage of new features
 *  and bug fixes. This interface wraps many functions in the 
 *  Loris procedural interface, but also provides some Loris C++
 *  class wrappers, to provide enhanced functionality in the 
 *  context of laguages that perform garbage collection.
 *
 *	Also, several interface (.i) files were collapsed into one
 *	(not sure I did myself any favors).
 *
 * Kelly Fitz, 8 Nov 2000
 * rewrite: Kelly Fitz, 23 Jan 2003
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

%include exception.i 
%include typemaps.i 
%include std_vector.i
%include std_list.i

// ----------------------------------------------------------------
//		docstring for the Loris module (Python)
//
%define DOCSTRING
"
Loris is an Open Source sound modeling and processing software package
based on the Reassigned Bandwidth-Enhanced Additive Sound Model. Loris
supports modified resynthesis and manipulations of the model data,
such as time- and frequency-scale modification and sound morphing.


Loris is developed by Kelly Fitz and Lippold Haken at the CERL Sound
Group, and is distributed under the GNU General Public License (GPL).
For more information, please visit

   http://www.cerlsoundgroup.org/Loris/
"
%enddef

%module(docstring=DOCSTRING) loris

// enable automatic docstring generation in Python module
%feature("autodoc","0");

// ----------------------------------------------------------------
//		include Loris headers needed to generate wrappers
//
%{
	#include "loris.h"
	
	#include "AiffFile.h"
	#include "Analyzer.h"
	#include "Channelizer.h"
	#include "Collator.h"
	#include "Distiller.h"
	#include "LorisExceptions.h"
	#include "LinearEnvelope.h"
	#include "Marker.h"
	#include "Partial.h"
	#include "SdifFile.h"
	#include "Sieve.h"
	#include "SpcFile.h"
	#include "Synthesizer.h"

	//	import the entire Loris namespace
	using namespace Loris;
	
	#include <stdexcept>
	#include <vector>
%}

// ----------------------------------------------------------------
//		Use the SWIG library to wrap std::vectors.
//
namespace std {
   %template(DoubleVector) vector< double >;
   %template(MarkerVector) vector< Marker >;
};

// ----------------------------------------------------------------
//		notification and exception handlers
//
//	Exception handling code for procedural interface calls.
//	Copied from the SWIG manual. Tastes great, less filling.

%{ 
	static char error_message[256];
	static int error_status = 0;
	
	void throw_exception( const char *msg ) 
	{
		strncpy(error_message,msg,256);
		error_status = 1;
	}
	
	void clear_exception( void ) 
	{
		error_status = 0;
	}
	
	char *check_exception( void ) 
	{
		if ( error_status ) 
		{
			return error_message;
		}
		else 
		{
			return NULL;
		}
	}
%}

// Procedural interface functions all use the exception
// and notification mechanism defined above.
//
%exception 
{
    char * err;
    clear_exception();
    $action
    if ( 0 != (err = check_exception()) )
    {
        SWIG_exception( SWIG_ValueError, err );
    }
}

//	Configure notification and debugging using a
//	in a SWIG initialization block. This code is
//	executed when the module is loaded by the 
//	host interpreter.
//
%{
	//	notification function for Loris debugging
	//	and notifications, installed in initialization
	//	block below:
	static void printf_notifier( const char * s )
	{
		printf("*\t%s\n", s);
	}	
%}

%init 
%{
	setNotifier( printf_notifier );
	setExceptionHandler( throw_exception );
%}

// ----------------------------------------------------------------
//		wrap procedural interface
//
//	Not all functions in the procedural interface are trivially
//	wrapped, some are wrapped to return newly-allocated objects,
//	which we wouldn't do in the procedural interface, but we
//	can do, because SWIG and the scripting langauges take care of 
//	the memory management ambiguities.
//

%feature("docstring", 
"Label Partials in a PartialList with the integer nearest to the
amplitude-weighted average ratio of their frequency envelope to a
reference frequency envelope. If a reference frequency is specified,
then the reference envelope is constant at that frequency.

The frequency spectrum is partitioned into non-overlapping channels
whose time-varying center frequencies track the reference frequency
envelope. The reference label indicates which channel's center
frequency is exactly equal to the reference envelope frequency, and
other channels' center frequencies are multiples of the reference
envelope frequency divided by the reference label. Each Partial in the
PartialList is labeled with the number of the channel that best fits
its frequency envelope. Partials are labeled, but otherwise
unmodified.

For finer control over channelization, including harmonic stretching
for stiff strings, and amplitude weighting for determining the
best-fitting channel, use the Channelizer class."
) channelize;

void channelize( PartialList * partials, 
                 LinearEnvelope * refFreqEnvelope, int refLabel );

%inline
%{

	void channelize( PartialList * partials, double refFreq )
    {
    	//	create a constant envelope at refFreq
    	LinearEnvelope env( refFreq );    	
		Channelizer::channelize( *partials, env, 1 );
    }
                 
%}
                

%feature("docstring",
"Collate unlabeled (zero-labeled) Partials into the smallest-possible 
number of Partials that does not combine any overlapping Partials.
Collated Partials assigned labels higher than any label in the original 
list, and appear at the end of the sequence, after all previously-labeled
Partials. Optionally specify the fade and gap times, else default values
are used.") collate_duh;

%rename( collate ) collate_duh;

%inline 
%{
    // there seems to be a collision with a symbol name
    // in localefwd.h (GNU) that is somehow getting
    // imported, and using statements do not solve
    // the problem as they should.
    void collate_duh( PartialList * partials, 
				  	  double fadeTime = Collator::DefaultFadeTimeMs/1000.0, 
				  	  double gapTime = Collator::DefaultSilentTimeMs/1000.0 )
    {
    	Collator c( fadeTime, gapTime );
        c.collate( *partials );
    }
%}

%feature("docstring",
"Return a newly-constructed LinearEnvelope using the legacy 
FrequencyReference class. The envelope will have approximately
the specified number of samples. The specified number of samples 
must be greater than 1. Uses the FundamentalEstimator 
(FundamentalFromPartials) class to construct an estimator of 
fundamental frequency, configured to emulate the behavior of
the FrequencyReference class in Loris 1.4-1.5.2. If numSamps
is unspecified, construct the reference envelope from fundamental 
estimates taken every five milliseconds.

For simple sounds, this frequency reference may be a 
good first approximation to a reference envelope for
channelization (see channelize()).") createFreqReference;

%newobject createFreqReference;
LinearEnvelope * 
createFreqReference( PartialList * partials, 
					 double minFreq, double maxFreq, long numSamps );

// why can't I use default arguments to do this?
// Because SWIG wants all default arguments to be
// match the C++ function declaration with it is
// processing C++ code. 
%inline 
%{
	LinearEnvelope * 
	createFreqReference( PartialList * partials, 
						 double minFreq, double maxFreq )
	{
		return createFreqReference( partials, minFreq, maxFreq, 0 );
	}
%}

%feature("docstring",
"Return a newly-constructed LinearEnvelope that estimates
the time-varying fundamental frequency of the sound
represented by the Partials in a PartialList. This uses
the FundamentalEstimator (FundamentalFromPartials) 
class to construct an estimator of fundamental frequency, 
and returns a LinearEnvelope that samples the estimator at the 
specified time interval (in seconds). Default values are used 
to configure the estimator. Only estimates in the specified 
frequency range will be considered valid, estimates outside this 
range will be ignored.

Clients are responsible for disposing of the newly-constructed 
LinearEnvelope.") createF0Estimate;
   
LinearEnvelope * 
createF0Estimate( PartialList * partials, double minFreq, double maxFreq, 
                  double interval );

%feature("docstring", 
"Dilate Partials in a PartialList according to the given initial
and target time points. Partial envelopes are stretched and
compressed so that temporal features at the initial time points
are aligned with the final time points. Time points are sorted, so
Partial envelopes are are only stretched and compressed, but
breakpoints are not reordered. Duplicate time points are allowed.
There must be the same number of initial and target time points.") dilate;


//	dilate needs a contract to guarantee that the
//	same number of initial and target points are
//	provided.
%contract dilate( PartialList * partials, 
                  const std::vector< double > & ivec, 
                  const std::vector< double > & tvec ) 
{
require:
	ivec->size() == tvec->size();
}

%inline
%{
	void dilate( PartialList * partials, 
		   	     const std::vector< double > & ivec, 
				 const std::vector< double > & tvec )
	{
		const double * initial = &( ivec.front() );
		const double * target = &( tvec.front() );
		int npts = ivec.size();
		dilate( partials, initial, target, npts );
	}
%}

%feature("docstring",
"Distill labeled (channelized) Partials in a PartialList into a 
PartialList containing at most one Partial per label. Unlabeled 
(zero-labeled) Partials are left unmodified at the end of the 
distilled Partials. Optionally specify the fade and gap times, 
defaults are 5ms and 1ms.
") wrap_distill;

%rename( distill ) wrap_distill;

%inline
%{
	void wrap_distill( PartialList * partials, 
				      double fadeTime = Distiller::DefaultFadeTimeMs/1000.0, 
				      double gapTime = Distiller::DefaultSilentTimeMs/1000.0 )
	{
		try
		{
			Distiller d( fadeTime, gapTime );
			d.distill( *partials );
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );
		}
	}
%}


%feature("docstring",
"Export audio samples stored in a vector to an AIFF file having the
specified number of channels and sample rate at the given file
path (or name). The floating point samples in the vector are
clamped to the range (-1.,1.) and converted to integers having
bitsPerSamp bits. The default values for the sample rate and
sample size, if unspecified, are 44100 Hz (CD quality) and 16 bits
per sample, respectively. If neither is specified, then the 
default synthesis parameters (see SynthesisParameters) are used.

If a PartialList is specified, the Partials are rendered at the
specified sample rate and then exported.

Only mono files can be exported, the last argument is ignored, 
and is included only for backward compatability") wrap_exportAiff;

%rename( exportAiff ) wrap_exportAiff;

// Need this junk, because SWIG changed the way it handles
// default arguments when writing C++ wrappers.
//
%inline 
%{
	void wrap_exportAiff( const char * path, const std::vector< double > & samples,
					      double samplerate = 44100, int bitsPerSamp = 16, 
					      int nchansignored = 1 )
	{
		exportAiff( path, &(samples.front()), samples.size(), 
					samplerate, bitsPerSamp );
	}
	
	void wrap_exportAiff( const char * path, PartialList * partials,
					      double samplerate = 44100, int bitsPerSamp = 16 )
	{
		try
		{
			std::vector< double > vec;
			
			Synthesizer synth( samplerate, vec );			
			synth.synthesize( partials->begin(), partials->end() );
		
			exportAiff( path, &(vec.front()), vec.size(), 
					    samplerate, bitsPerSamp );
					
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );
		}
	}
	
%}

%feature("docstring",
"Export Partials in a PartialList to a SDIF file at the specified
file path (or name). SDIF data is written in the Loris RBEP
format. For more information about SDIF, see the SDIF website at:
	www.ircam.fr/equipes/analyse-synthese/sdif/  ") exportSdif;

void exportSdif( const char * path, PartialList * partials );

%feature("docstring",
"Export Partials in a PartialList to a Spc file at the specified
file path (or name). The fractional MIDI pitch must be specified.
The optional enhanced parameter defaults to true (for
bandwidth-enhanced spc files), but an be specified false for
pure-sines spc files. The optional endApproachTime parameter is in
seconds; its default value is zero (and has no effect). A nonzero
endApproachTime indicates that the PartialList does not include a
release, but rather ends in a static spectrum corresponding to the
final breakpoint values of the partials. The endApproachTime
specifies how long before the end of the sound the amplitude,
frequency, and bandwidth values are to be modified to make a
gradual transition to the static spectrum.");

void exportSpc( const char * path, PartialList * partials, double midiPitch, 
				int enhanced, double endApproachTime );

// Need these two also, because SWIG changed the way it handles
// default arguments when writing C++ wrappers.
//
%inline %{
void exportSpc( const char * path, PartialList * partials, double midiPitch,
			    int enhanced )
{
	exportSpc( path, partials, midiPitch, enhanced, 0. );
}
%}
%inline %{
void exportSpc( const char * path, PartialList * partials, double midiPitch )
{
	exportSpc( path, partials, midiPitch, true, 0. );
}
%}

%feature("docstring",
"Apply a reference Partial to fix the frequencies of Breakpoints
whose amplitude is below threshold_dB. Threshold 0 harmonifies all
Partials. To apply only to quiet Partials, specify a lower 
threshold (like -90). The reference Partial is the first Partial
in the PartialList labeled refLabel (usually 1). The Envelope,
if specified, is a time-varying weighting on the harmonifing process. 
When 1, harmonic frequencies are used, when 0, breakpoint frequencies are 
unmodified. ") harmonify;


%{
#include "Harmonifier.h"
%}

%inline %{
	void harmonify( PartialList * partials, long refLabel,
                    const Envelope * env, double threshold_dB )
	{
		try
		{
			Harmonifier::harmonify( partials->begin(), partials->end(), 
									refLabel, *env, threshold_dB );
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );
		}
	}

    void harmonify( PartialList * partials, long refLabel, 
                    double threshold_dB )
    {
        LinearEnvelope e( 1 );
        try
		{
			Harmonifier::harmonify( partials->begin(), partials->end(), 
                                	refLabel, e, threshold_dB );
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );
		}
    }
%}                        

%feature("docstring",
"Import Partials from an SDIF file at the given file path (or
name), and return them in a PartialList. Loris can import
SDIF data stored in 1TRC format or in the Loris RBEP format.
For more information about SDIF, see the SDIF website at:
	www.ircam.fr/equipes/analyse-synthese/sdif/");
	
%newobject importSdif;
%inline %{
	PartialList * importSdif( const char * path )
	{
		PartialList * dst = createPartialList();
		importSdif( path, dst );

		// check for exception:
		if (check_exception())
		{
			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}
%}


%feature("docstring",
"Import Partials from an Spc file at the given file path (or
name), and return them in a PartialList.");

%newobject importSpc;
%inline %{
	PartialList * importSpc( const char * path )
	{
		PartialList * dst = createPartialList();
		importSpc( path, dst );

		// check for exception:
		if (check_exception())
		{
			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}
%}


%{
#include "Morpher.h"

//	Assign these flags the same default values used by the Morpher class.
static bool DoLogAmplitudeMorphing = true;
static bool DoLogFrequencyMorphing = false;

%}

%feature("docstring",
"Enable or disable log-domain amplitude and bandwidth morphing.") enableLogAmpMorphing;

%feature("docstring",
"Enable or disable log-domain frequency morphing.") enableLogFreqMorphing;


%inline %{

	void enableLogAmpMorphing( bool enableFlag )
	{
		DoLogAmplitudeMorphing = enableFlag;
	}
	
	void enableLogFreqMorphing( bool enableFlag )
	{
		DoLogFrequencyMorphing = enableFlag;
	}
	

%}


%feature("docstring",
"Morph labeled Partials in two PartialLists according to the
given frequency, amplitude, and bandwidth (noisiness) morphing
envelopes, and return the morphed Partials in a PartialList.

Optionally specify the labels of the Partials to be used as 
reference Partial for the two morph sources. The reference 
partial is used to compute frequencies for very low-amplitude 
Partials whose frequency estimates are not considered reliable. 
The reference Partial is considered to have good frequency 
estimates throughout. A reference label of 0 indicates that 
no reference Partial should be used for the corresponding
morph source.

Loris morphs Partials by interpolating frequency, amplitude,
and bandwidth envelopes of corresponding Partials in the
source PartialLists. For more information about the Loris
morphing algorithm, see the Loris website:
	www.cerlsoundgroup.org/Loris/") morph;

%newobject morph;
%inline %{
	PartialList * morph( const PartialList * src0, const PartialList * src1, 
                         const Envelope * ffreq, 
                         const Envelope * famp, 
                         const Envelope * fbw )
	{
		PartialList * dst = createPartialList();
		
		notifier << "morphing " << src0->size() << " Partials with " <<
					src1->size() << " Partials" << endl;
		try
		{			
			//	make a Morpher object and do it:
			Morpher m( *ffreq, *famp, *fbw );
			m.enableLogAmpMorphing( DoLogAmplitudeMorphing );
			m.enableLogFreqMorphing( DoLogFrequencyMorphing );
			
			m.morph( src0->begin(), src0->end(), src1->begin(), src1->end() );
					
			//	splice the morphed Partials into dst:
			dst->splice( dst->end(), m.partials() );
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );

			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}
	
	PartialList * morph( const PartialList * src0, const PartialList * src1, 
                         double freqweight, 
                         double ampweight, 
                         double bwweight )
	{
		LinearEnvelope ffreq( freqweight ), famp( ampweight ), fbw( bwweight );
		
		PartialList * dst = createPartialList();
		
		notifier << "morphing " << src0->size() << " Partials with " <<
					src1->size() << " Partials" << endl;
		try
		{			
			//	make a Morpher object and do it:
			Morpher m( ffreq, famp, fbw );
			m.enableLogAmpMorphing( DoLogAmplitudeMorphing );
			m.enableLogFreqMorphing( DoLogFrequencyMorphing );
			
			m.morph( src0->begin(), src0->end(), src1->begin(), src1->end() );
					
			//	splice the morphed Partials into dst:
			dst->splice( dst->end(), m.partials() );
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );

			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}

	PartialList * morph( const PartialList * src0, const PartialList * src1,
	                     long src0RefLabel, long src1RefLabel,
                         const Envelope * ffreq, 
                         const Envelope * famp, 
                         const Envelope * fbw )
	{
		PartialList * dst = createPartialList();
		
		notifier << "morphing " << src0->size() << " Partials with " <<
					src1->size() << " Partials" << endl;
		try
		{			
			//	make a Morpher object and do it:
			Morpher m( *ffreq, *famp, *fbw );
			m.enableLogAmpMorphing( DoLogAmplitudeMorphing );
			m.enableLogFreqMorphing( DoLogFrequencyMorphing );
			

			if ( src0RefLabel != 0 )
			{
			   notifier << "using Partial labeled " << src0RefLabel;
			   notifier << " as reference Partial for first morph source" << endl;
			   m.setSourceReferencePartial( *src0, src0RefLabel );
			}
			else
			{
			   notifier << "using no reference Partial for first morph source" << endl;
			}

			if ( src1RefLabel != 0 )
			{
			   notifier << "using Partial labeled " << src1RefLabel;
			   notifier << " as reference Partial for second morph source" << endl;
			   m.setTargetReferencePartial( *src1, src1RefLabel );
			}
			else
			{
			   notifier << "using no reference Partial for second morph source" << endl;
			}			
			
			
			m.morph( src0->begin(), src0->end(), src1->begin(), src1->end() );
					
			//	splice the morphed Partials into dst:
			dst->splice( dst->end(), m.partials() );
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );

			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}
	
	PartialList * morph( const PartialList * src0, const PartialList * src1, 
	                     long src0RefLabel, long src1RefLabel,
                         double freqweight, 
                         double ampweight, 
                         double bwweight )
	{
		LinearEnvelope ffreq( freqweight ), famp( ampweight ), fbw( bwweight );
		
		PartialList * dst = createPartialList();
		
		notifier << "morphing " << src0->size() << " Partials with " <<
					src1->size() << " Partials" << endl;
		try
		{			
			//	make a Morpher object and do it:
			Morpher m( ffreq, famp, fbw );
			m.enableLogAmpMorphing( DoLogAmplitudeMorphing );
			m.enableLogFreqMorphing( DoLogFrequencyMorphing );
			

			if ( src0RefLabel != 0 )
			{
			   notifier << "using Partial labeled " << src0RefLabel;
			   notifier << " as reference Partial for first morph source" << endl;
			   m.setSourceReferencePartial( *src0, src0RefLabel );
			}
			else
			{
			   notifier << "using no reference Partial for first morph source" << endl;
			}

			if ( src1RefLabel != 0 )
			{
			   notifier << "using Partial labeled " << src1RefLabel;
			   notifier << " as reference Partial for second morph source" << endl;
			   m.setTargetReferencePartial( *src1, src1RefLabel );
			}
			else
			{
			   notifier << "using no reference Partial for second morph source" << endl;
			}			
			
			
			m.morph( src0->begin(), src0->end(), src1->begin(), src1->end() );
					
			//	splice the morphed Partials into dst:
			dst->splice( dst->end(), m.partials() );
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );

			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}
%}

%feature("docstring",
"Set the shaping parameter for the amplitude morphing
function. 

DEPRECATED - DO NOT USE
") morpher_setAmplitudeShape;

%rename( setAmplitudeMorphShape ) morpher_setAmplitudeShape;

void morpher_setAmplitudeShape( double shape );





const double LORIS_DEFAULT_AMPMORPHSHAPE;    
const double LORIS_LINEAR_AMPMORPHSHAPE;


%feature("docstring",
"Trim Partials by removing Breakpoints outside a specified time span.
Insert a Breakpoint at the boundary when cropping occurs.
") crop;

void crop( PartialList * partials, double t1, double t2 );

%feature("docstring",
"Copy Partials in the source PartialList having the specified label
into a new PartialList. The source PartialList is unmodified.
") copyLabeled;

%newobject copyLabeled;
%inline %{
	PartialList * copyLabeled( PartialList * partials, long label )
	{
		PartialList * dst = createPartialList();
		copyLabeled( partials, label, dst );
		
		// check for exception:
		if ( check_exception() )
		{
			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}
%}

%feature("docstring",
"Extract Partials in the source PartialList having the specified
label and return them in a new PartialList.") extractLabeled;

%newobject extractLabeled;
%inline %{
	PartialList * extractLabeled( PartialList * partials, long label )
	{
		PartialList * dst = createPartialList();
		extractLabeled( partials, label, dst );
		
		// check for exception:
		if ( check_exception() )
		{
			destroyPartialList( dst );
			dst = NULL;
		}
		return dst;
	}
%}

%feature("docstring",
"Remove from a PartialList all Partials having the specified label.") removeLabeled;

void removeLabeled( PartialList * partials, long label );

%feature("docstring",
"Resample all Partials in a PartialList using the specified
sampling interval, so that the Breakpoints in the Partial
envelopes will all lie on a common temporal grid. 

The Breakpoint times in resampled Partials will comprise a
contiguous sequence of ALL integer multiples of the sampling interval
(a lot of data, but useful for some third-party tools, like the CNMAT
sinusoids~ external for Max/MSP). 

If a timing envelope is specified, then that envelope represents
a warping of the time axis that is applied during resampling. The
Breakpoint times in resampled Partials will a comprise contiguous 
sequence of all integer multiples of the sampling interval between 
the first and last breakpoints in the timing envelope, and each 
Breakpoint will represent the parameters of the original Partial 
at the time that is the value of the timing envelope at that instant.
This can be used to achieve effects similar to dilation (see dilate),
but can also be used to achieve time-reveral and scrubbing effects.

If phase correct resampling is selected, Partial frequencies are
altered slightly to match, as nearly as possible, the Breakpoint
phases after resampling. Phases are updated so that the Partial
frequencies and phases are consistent after resampling. The
default is phase correct resampling, unless a timing envelope
is specified, in which case it is better to explcitly match
phases at known critical points.

See also quantize, which was previously described as sparse
resampling.")  wrap_resample;

%rename( resample ) wrap_resample;

%{
#include "Resampler.h"
%}


%inline %{

    void wrap_resample( PartialList * partials, double interval, 
				   		bool denseResampling = true,
				     	bool phaseCorrect = true )
	{
			
		try
		{		
			Resampler r( interval );
			r.setPhaseCorrect( phaseCorrect );
			if ( denseResampling )
			{
                r.resample( partials->begin(), partials->end() );
            }
            else
            {
                r.quantize( partials->begin(), partials->end() );
            }
            
            PartialList::iterator it = partials->begin();
            while ( it != partials->end() )
            {
            	if ( 0 == it->numBreakpoints() )
            	{
            		it = partials->erase( it );
            	}
            	else
            	{
            		++it;
            	}
            }
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );
		}
	}

	void wrap_resample( PartialList * partials, LinearEnvelope * timing, 
	                    double interval,
				     	bool phaseCorrect = false )
	{			
		try
		{		
			Resampler r( interval );
			r.setPhaseCorrect( phaseCorrect );
			r.resample( partials->begin(), partials->end(), *timing );

            PartialList::iterator it = partials->begin();
            while ( it != partials->end() )
            {
            	if ( 0 == it->numBreakpoints() )
            	{
            		it = partials->erase( it );
            	}
            	else
            	{
            		++it;
            	}
            }
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );
		}
	}
%}


%feature("docstring",
"Quantize the Breakpoint times in the specified Partials.
Each Breakpoint in the Partials is replaced by a Breakpoint
constructed by resampling the Partial at the nearest
integer multiple of the of the resampling interval.

In previous versions of Loris, this was called sparse resampling.
") quantize;
	
%inline %{

	void quantize( PartialList * partials, double interval )
	{
		try
		{		
			Resampler r( interval );
			r.setPhaseCorrect( true );
			r.quantize( partials->begin(), partials->end() );

            PartialList::iterator it = partials->begin();
            while ( it != partials->end() )
            {
            	if ( 0 == it->numBreakpoints() )
            	{
            		it = partials->erase( it );
            	}
            	else
            	{
            		++it;
            	}
            }
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );
		}
	}

%}

%{
#include "PartialUtils.h"
%}
 
%feature("docstring",
"Bad old name for scaleAmplitude.") scaleAmp;

%feature("docstring",
"Scale the amplitude of a Partial, or all Partials in a PartialList, according 
to an envelope representing a time-varying amplitude scale value.")
scaleAmplitude;
				 
%inline 
%{	

	void scaleAmp( PartialList * partials, Envelope * ampEnv )
	{
		PartialUtils::scaleAmplitude( partials->begin(), partials->end(), *ampEnv );
	}
	
	void scaleAmplitude( PartialList * partials, Envelope * ampEnv )
	{
		PartialUtils::scaleAmplitude( partials->begin(), partials->end(), *ampEnv );
	}

    void scaleAmplitude( Partial * p, Envelope * ampEnv )
	{
		PartialUtils::scaleAmplitude( *p, *ampEnv );
	}

    void scaleAmp( PartialList * partials, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::scaleAmplitude( partials->begin(), partials->end(), e );
	}
	
	void scaleAmplitude( PartialList * partials, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::scaleAmplitude( partials->begin(), partials->end(), e );
	}
	
	void scaleAmplitude( Partial * p, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::scaleAmplitude( *p, e );
	}
%}

%feature("docstring",
"Scale the bandwidth of a Partial, or all Partials in a PartialList, according 
to an envelope representing a time-varying bandwidth scale value.");

%inline %{	

	void scaleBandwidth( PartialList * partials, Envelope * bwEnv )
	{
		PartialUtils::scaleBandwidth( partials->begin(), partials->end(), *bwEnv );
	}
				 

	void scaleBandwidth( PartialList * partials, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::scaleBandwidth( partials->begin(), partials->end(), e );
	}
	
	void scaleBandwidth( Partial * p, Envelope * bwEnv )
	{
		PartialUtils::scaleBandwidth( *p, *bwEnv );
	}
				 

	void scaleBandwidth( Partial * p, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::scaleBandwidth( *p, e );
	}	
%}

%feature("docstring",
"Scale the frequency of a Partial, or all Partials in a PartialList, according 
to an envelope representing a time-varying frequency scale value.");
				 
%inline %{	

	void scaleFrequency( PartialList * partials, Envelope * freqEnv )
	{
		PartialUtils::scaleFrequency( partials->begin(), partials->end(), *freqEnv );
	}

	void scaleFrequency( PartialList * partials, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::scaleFrequency( partials->begin(), partials->end(), e );
	}
	
	void scaleFrequency( Partial * p, Envelope * freqEnv )
	{
		PartialUtils::scaleFrequency( *p, *freqEnv );
	}

	void scaleFrequency( Partial * p, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::scaleFrequency( *p, e );
	}
	
%}

%feature("docstring",
"Scale the relative noise content of a Partial, or all Partials in a PartialList,
according to an envelope representing a (time-varying) noise energy 
scale value.");


%inline 
%{	

	void scaleNoiseRatio( PartialList * partials, Envelope * noiseEnv )
	{
		PartialUtils::scaleNoiseRatio( partials->begin(), partials->end(), *noiseEnv );
	}

	void scaleNoiseRatio( PartialList * partials, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::scaleNoiseRatio( partials->begin(), partials->end(), e );
	}
	
	void scaleNoiseRatio( Partial * p, Envelope * noiseEnv )
	{
		PartialUtils::scaleNoiseRatio( *p, *noiseEnv );
	}

	void scaleNoiseRatio( Partial * p, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::scaleNoiseRatio( *p, e );
	}
%}

%feature("docstring",
"Set the bandwidth of a Partial, or all Partials in a PartialList, according 
to an envelope representing a time-varying bandwidth value.");

%inline 
%{	

	void setBandwidth( PartialList * partials, Envelope * bwEnv )
	{
		PartialUtils::setBandwidth( partials->begin(), partials->end(), *bwEnv );
	}
	
	void setBandwidth( PartialList * partials, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::setBandwidth( partials->begin(), partials->end(), e );
	}
	
	void setBandwidth( Partial * p, Envelope * bwEnv )
	{
		PartialUtils::setBandwidth( *p, *bwEnv );
	}
	
	void setBandwidth( Partial * p, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::setBandwidth( *p, e );
	}
%}

%feature("docstring",
"Scale the amplitudes of a set of Partials by applying 
a spectral suface constructed from another set.
If frequency and time stretch factors are specified,
then the spectral surface is stretched by those 
factors before the surface is used to shape the 
Partial amplitudes.");

void shapeSpectrum( PartialList * partials, PartialList * surface,
                    double stretchFreq, double stretchTime );

%inline %{	
	void shapeSpectrum( PartialList * partials, PartialList * surface )
	{
		shapeSpectrum( partials, surface, 1.0, 1.0 );
	}
%}	          

%feature("docstring",
"Shift the pitch of all Partials in a PartialList according to 
the given pitch envelope. The pitch envelope is assumed to have 
units of cents (1/100 of a halfstep).");


%inline 
%{	
	void shiftPitch( PartialList * partials, Envelope * pitchEnv )
	{
		PartialUtils::shiftPitch( partials->begin(), partials->end(), *pitchEnv );	
	}
	
	void shiftPitch( PartialList * partials, double val )
	{
		LinearEnvelope e( val );
		PartialUtils::shiftPitch( partials->begin(), partials->end(), e );
	}
	
%}

%feature("docstring",
"Shift the time of all the Breakpoints in a Partial by a constant
amount (in seconds).");

void shiftTime( PartialList * partials, double offset );

%feature("docstring",
"Eliminate overlapping Partials having the same label
(except zero). If any two partials with same label
overlap in time, keep only the longer of the two.
Set the label of the shorter duration partial to zero.
Optionally specify the fade time, default is 1ms.") fake_sift;

//void sift( PartialList * partials );

%rename( sift ) fake_sift;

%inline
%{
	void fake_sift( PartialList * partials, 
				    double fadeTime = Sieve::DefaultFadeTimeMs/1000.0 )
	{
		Sieve s( fadeTime );
		s.sift( *partials );
	}
%}


%feature("docstring",
"Sort the Partials in a PartialList in order of increasing label.
The sort is stable; Partials having the same label are not
reordered.");

void sortByLabel( PartialList * partials );

%feature("docstring",
"Return the average amplitude over all Breakpoints in this Partial.
Return zero if the Partial has no Breakpoints.") avgAmplitude;

double avgAmplitude( const Partial * p );

%feature("docstring",
"Return the average frequency over all Breakpoints in this Partial.
Return zero if the Partial has no Breakpoints.") avgFrequency;

double avgFrequency( const Partial * p );

%feature("docstring",
"Return the maximum amplitude achieved by a Partial.") peakAmplitude;

double peakAmplitude( const Partial * p );

%feature("docstring",
"Return the minimum start time and maximum end time
of all Partials in this PartialList.") timeSpan;

%apply double * OUTPUT { double * tmin_out, double * tmax_out };
void timeSpan( PartialList * partials, double * tmin_out, double * tmax_out );

%feature("docstring",
"Return the average frequency over all Breakpoints in this Partial, 
weighted by the Breakpoint amplitudes. Return zero if the Partial 
has no Breakpoints.") weightedAvgFrequency;

double weightedAvgFrequency( const Partial * p );

%feature("docstring",
"Recompute phases of all Breakpoints later than the specified 
time so that the synthesized phases of those later Breakpoints 
matches the stored phase, as long as the synthesized phase at 
the specified time matches the stored (not recomputed) phase.

Phase fixing is only applied to non-null (nonzero-amplitude) 
Breakpoints, because null Breakpoints are interpreted as phase 
reset points in Loris. If a null is encountered, its phase is 
corrected from its non-Null successor, if it has one, otherwise 
it is unmodified.") fixPhaseAfter;

void fixPhaseAfter( PartialList * partials, double time );

%feature("docstring",
"Recompute phases of all Breakpoints in a Partial
so that the synthesized phases match the stored phases, 
and the synthesized phase at (nearest) the specified
time matches the stored (not recomputed) phase.

Backward phase-fixing stops if a null (zero-amplitude) 
Breakpoint is encountered, because nulls are interpreted as 
phase reset points in Loris. If a null is encountered, the 
remainder of the Partial (the front part) is fixed in the 
forward direction, beginning at the start of the Partial. 
Forward phase fixing is only applied to non-null 
(nonzero-amplitude) Breakpoints. If a null is encountered, 
its phase is corrected from its non-Null successor, if 
it has one, otherwise it is unmodified.") fixPhaseAt;

void fixPhaseAt( PartialList * partials, double time );

%feature("docstring",
"Recompute phases of all Breakpoints earlier than the specified 
time so that the synthesized phases of those earlier Breakpoints 
matches the stored phase, and the synthesized phase at the 
specified time matches the stored (not recomputed) phase.

Backward phase-fixing stops if a null (zero-amplitude) Breakpoint
is encountered, because nulls are interpreted as phase reset 
points in Loris. If a null is encountered, the remainder of the 
Partial (the front part) is fixed in the forward direction, 
beginning at the start of the Partial.") fixPhaseBefore;

void fixPhaseBefore( PartialList * partials, double time );

%feature("docstring",
"Fix the phase travel between two times by adjusting the
frequency and phase of Breakpoints between those two times.

This algorithm assumes that there is nothing interesting 
about the phases of the intervening Breakpoints, and modifies 
their frequencies as little as possible to achieve the correct 
amount of phase travel such that the frequencies and phases at 
the specified times match the stored values. The phases of all 
the Breakpoints between the specified times are recomputed.") fixPhaseBetween;

void fixPhaseBetween( PartialList * partials, double tbeg, double tend );

%feature("docstring",
"Recompute phases of all Breakpoints later than the specified 
time so that the synthesized phases of those later Breakpoints 
matches the stored phase, as long as the synthesized phase at 
the specified time matches the stored (not recomputed) phase. 
Breakpoints later than tend are unmodified.

Phase fixing is only applied to non-null (nonzero-amplitude) 
Breakpoints, because null Breakpoints are interpreted as phase 
reset points in Loris. If a null is encountered, its phase is 
corrected from its non-Null successor, if it has one, otherwise 
it is unmodified.") fixPhaseForward;

void fixPhaseForward( PartialList * partials, double tbeg, double tend );

/*
%feature("docstring",
"Adjust frequencies of the Breakpoints in the 
 specified Partial such that the rendered Partial 
achieves (or matches as nearly as possible, within 
the constraint of the maximum allowable frequency
alteration) the analyzed phases. 

partial The Partial whose frequencies,
and possibly phases (if the frequencies
cannot be sufficiently altered to match
the phases), will be recomputed.

maxFixPct The maximum allowable frequency 
alteration, default is 0.2%.") fixFrequency;

void fixFrequency( Partial & partial, double maxFixPct = 0.2 );
*/

%feature("docstring",
"Return a string describing the Loris version number.");

%inline %{
	const char * version( void )
	{
		static const char * vstr = LORIS_VERSION_STR;
		return vstr;
	}
%}
 
// ----------------------------------------------------------------
//		wrap Loris classes

//	Wrap all calls into the Loris library with exception
//	handlers to prevent exceptions from leaking out of the
//	C++ code, wherein they can be handled, and into the
//	interpreter, where they will surely cause an immediate
//	halt. Only std::exceptions and Loris::Exceptions (and 
//	subclasses) can be thrown.
//
//	Don't use procedural interface calls here, because this 
//	exception handler doesn't check for exceptions raised in
//	the procedural interface!
//

//	These should probably not all report UnknownError, could
//	make an effort to raise the right kind of (SWIG) exception.
//
%exception {
	try
	{	
		$action
	}
	catch( Loris::Exception & ex ) 
	{
		//	catch Loris::Exceptions:
		std::string s("Loris exception: " );
		s.append( ex.what() );
		SWIG_exception( SWIG_UnknownError, (char *) s.c_str() );
	}
	catch( std::exception & ex ) 
	{
		//	catch std::exceptions:
		std::string s("std C++ exception: " );
		s.append( ex.what() );
		SWIG_exception( SWIG_UnknownError, (char *) s.c_str() );
	}
}

// ---------------------------------------------------------------------------
//	class Marker
//
%feature("docstring",
"Class Marker represents a labeled time point in a set of Partials
or a vector of samples. Collections of Markers (see the MarkerContainer
definition below) are held by the File I/O classes in Loris (AiffFile,
SdifFile, and SpcFile) to identify temporal features in imported
and exported data.") Marker;

class Marker
{
public:
//	-- construction --
	
%feature("docstring",
"Initialize a Marker with the specified time (in seconds) and name,
or copy the time and name from another Marker. If unspecified, time 
is zero and the label is empty.") Marker;

	Marker( void );
	 
	Marker( double t, const char * s );	 

	Marker( const Marker & other );
	
%feature("docstring",
"Destroy this Marker.") Marker::~Marker;
	~Marker( void );

		 
//	-- access --

%feature("docstring",
"Return the name of this Marker.");

	%extend 
	{
		const char * name( void ) { return self->name().c_str(); }
	}
	 
%feature("docstring",
"Return the time (in seconds) associated with this Marker.");

	double time( void );
	 
//	-- mutation --

%feature("docstring",
"Set the name of the Marker.");

	void setName( const char * s );
	 
%feature("docstring",
"Set the time (in seconds) associated with this Marker.");

	void setTime( double t );

	
};	//	end of class Marker

// ---------------------------------------------------------------------------
//	class AiffFile
//	
%feature("docstring",
"An AiffFile represents a sample file (on disk) in the Audio Interchange
File Format. The file is read from disk and the samples stored in memory
upon construction of an AiffFile instance. The samples are accessed by 
the samples() method, which converts them to double precision floats and
returns them in a vector.") AiffFile;

%newobject AiffFile::samples;

%feature("docstring",
"");

class AiffFile
{
public:
%feature("docstring",
"An AiffFile instance can be initialized in any of the following ways:

Initialize a new AiffFile from a vector of samples and sample rate.

Initialize a new AiffFile from two vectors of samples, for left and right
channels, and sample rate.

Initialize a new AiffFile using data read from a named file.

Initialize an instance of AiffFile having the specified sample 
rate, accumulating samples rendered at that sample rate from
all Partials in the specified PartialList with the (optionally) 
specified Partial fade time (see Synthesizer.h for an explanation 
of fade time). 
") AiffFile;

	AiffFile( const char * filename );
	AiffFile( const std::vector< double > & vec, double samplerate );
    AiffFile( const std::vector< double > & vec_left,
              const std::vector< double > & vec_right, 
              double samplerate );    

	%extend 
	{

		AiffFile( PartialList * l, double sampleRate = 44100, double fadeTime = .001 ) 
		{
			return new AiffFile( l->begin(), l->end(), sampleRate, fadeTime );
		}
	}
	
%feature("docstring",
"Destroy this AiffFile.") ~AiffFile;

	~AiffFile( void );
	
%feature("docstring",
"Return the sample rate in Hz for this AiffFile.") sampleRate;

	double sampleRate( void ) const;
	
%feature("docstring",
"Return the MIDI note number for this AiffFile. The defaul
note number is 60, corresponding to middle C.") midiNoteNumber;
	
	double midiNoteNumber( void ) const;

	//	this has been renamed
%feature("docstring",
"Return the number of sample frames (equal to the number of samples
in a single channel file) stored by this AiffFile.") numFrames;

	%rename( sampleFrames ) numFrames;
	unsigned long numFrames( void ) const;
		
%feature("docstring",
"Render the specified Partial using the (optionally) specified
Partial fade time, and accumulate the resulting samples into
the sample vector for this AiffFile.") addPartial;

	void addPartial( const Partial & p, double fadeTime = .001 /* 1 ms */ );

%feature("docstring",
"Set the fractional MIDI note number assigned to this AiffFile. 
If the sound has no definable pitch, use note number 60.0 
(the default).") setMidiNoteNumber;

	void setMidiNoteNumber( double nn );
		 
%feature("docstring",
"Export the sample data represented by this AiffFile to
the file having the specified filename or path. Export
signed integer samples of the specified size, in bits
(8, 16, 24, or 32).") write;

void write( const char * filename, unsigned int bps = 16 );
	
	%extend 
	{
	
%feature("docstring",
"Return a copy of the samples (as floating point numbers
on the range -1,1) stored in this AiffFile.") samples; 

		std::vector< double > samples( void )
		{
			return self->samples();
		}

		 
%feature("docstring",
"The number of channels is always 1. 
Loris only deals in mono AiffFiles") channels;

		int channels( void ) { return 1; }

%feature("docstring",
"Render all Partials on the specified half-open (STL-style) range
with the (optionally) specified Partial fade time (see Synthesizer.h
for an examplanation of fade time), and accumulate the resulting 
samples.") addPartials;

		void addPartials( PartialList * l, double fadeTime = 0.001/* 1ms */ )
		{
			self->addPartials( l->begin(), l->end(), fadeTime );
		}
		
%feature("docstring",
"Return the (possibly empty) collection of Markers for 
this AiffFile.") markers;

		std::vector< Marker > markers( void )
		{
			return self->markers();
		}

%feature("docstring",
"Append a collection of Markers for to the existing
set of Markers for this AiffFile.") addMarkers;

		void addMarkers( const std::vector< Marker > & markers )
		{
			self->markers().insert( self->markers().end(),
			                        markers.begin(), markers.end() );
		}
	
%feature("docstring",
"Specify a new (possibly empty) collection of Markers for
this AiffFile.") setMarkers;

		void setMarkers( const std::vector< Marker > & markers )
		{
			self->markers().assign( markers.begin(), markers.end() );
		}
	}
};

// ---------------------------------------------------------------------------
//	class Analyzer
//	

%feature("docstring",
"An Analyzer represents a configuration of parameters for
performing Reassigned Bandwidth-Enhanced Additive Analysis
of sampled waveforms. This analysis process yields a collection 
of Partials, each having a trio of synchronous, non-uniformly-
sampled breakpoint envelopes representing the time-varying 
frequency, amplitude, and noisiness of a single bandwidth-
enhanced sinusoid. 

For more information about Reassigned Bandwidth-Enhanced 
Analysis and the Reassigned Bandwidth-Enhanced Additive Sound 
Model, refer to the Loris website: 

	http://www.cerlsoundgroup.org/Loris/
") Analyzer;


%newobject Analyzer::analyze;
			
class Analyzer
{
public:
%feature("docstring",
"Construct and return a new Analyzer configured with the given	
frequency resolution (minimum instantaneous frequency	
difference between Partials) and analysis window main 
lobe width (between zeros). All other Analyzer parameters 	
are computed from the specified resolution and window
width. If the window width is not specified, 
then it is set to twice the resolution. If an envelope is
provided for the frequency resolution, then it describes
the time-varying analysis resolution in Hz. 

An Analyzer configuration can also be copied from another
instance.") Analyzer;
	 
	Analyzer( double resolutionHz );
	Analyzer( double resolutionHz, double windowWidthHz );
	Analyzer( const Envelope & resolutionEnv, double windowWidthHz );	
	Analyzer( const Analyzer & another );

%feature("docstring",
"Destroy this Analyzer.") ~Analyzer;

	~Analyzer( void );
		
	%extend 
	{
%feature("docstring",
"Analyze a vector of (mono) samples at the given sample rate 	  	
(in Hz) and return the resulting Partials in a PartialList.
If specified, use a frequency envelope as a fundamental reference for
Partial formation.") analyze;

		PartialList * analyze( const std::vector< double > & vec, double srate )
		{
			PartialList * partials = new PartialList();
			if ( ! vec.empty() )
			{
				self->analyze( vec, srate );
			}
			partials->splice( partials->end(), self->partials() );
			return partials;
		}
		 
		PartialList * analyze( const std::vector< double > & vec, double srate, 
                               Envelope * env )
		{
			PartialList * partials = new PartialList();
			if ( ! vec.empty() )
			{
				self->analyze( vec, srate, *env );
			}
			partials->splice( partials->end(), self->partials() );
			return partials;
		}
	}
	
%feature("docstring",
"Return the amplitude floor (lowest detected spectral amplitude),              
in (negative) dB, for this Analyzer.");

	double ampFloor( void ) const;


%feature("docstring",
"Return the crop time (maximum temporal displacement of a time-
frequency data point from the time-domain center of the analysis
window, beyond which data points are considered \"unreliable\")
for this Analyzer.");

 	double cropTime( void ) const;

%feature("docstring",
"Return the maximum allowable frequency difference between                     
consecutive Breakpoints in a Partial envelope for this Analyzer.");

 	double freqDrift( void ) const;

%feature("docstring",
"Return the frequency floor (minimum instantaneous Partial                  
frequency), in Hz, for this Analyzer.");

 	double freqFloor( void ) const;

%feature("docstring",
"Return the frequency resolution (minimum instantaneous frequency          
difference between Partials) for this Analyzer at the specified
time in seconds. If no time is specified, then the initial resolution
(at 0 seconds) is returned.");

	double freqResolution( double time = 0.0 ) const;

%feature("docstring",
"Return the hop time (which corresponds approximately to the 
average density of Partial envelope Breakpoint data) for this 
Analyzer.");

	double hopTime( void ) const;

%feature("docstring",
"Return the sidelobe attenutation level for the Kaiser analysis window in
positive dB. Higher numbers (e.g. 90) give very good sidelobe 
rejection but cause the window to be longer in time. Smaller 
numbers raise the level of the sidelobes, increasing the likelihood
of frequency-domain interference, but allow the window to be shorter
in time.");

  	double sidelobeLevel( void ) const;

%feature("docstring",
"Return the frequency-domain main lobe width (measured between 
zero-crossings) of the analysis window used by this Analyzer.");

	double windowWidth( void ) const;

%feature("docstring",
"Return true if the phases and frequencies of the constructed
 partials should be modified to be consistent at the end of the
 analysis, and false otherwise. (Default is true.)");
 
    bool phaseCorrect( void ) const;
    
	
%feature("docstring",
"Set the amplitude floor (lowest detected spectral amplitude), in              
(negative) dB, for this Analyzer.");

	void setAmpFloor( double x );
	
%feature("docstring",
"Deprecated, use storeResidueBandwidth instead.");

 	void setBwRegionWidth( double x );
	
%feature("docstring",
"Set the crop time (maximum temporal displacement of a time-
frequency data point from the time-domain center of the analysis
window, beyond which data points are considered \"unreliable\")
for this Analyzer.");

 	void setCropTime( double x );
	
%feature("docstring",
"Set the maximum allowable frequency difference between                     
consecutive Breakpoints in a Partial envelope for this Analyzer.");

	void setFreqDrift( double x );
	
%feature("docstring",
"Set the amplitude floor (minimum instantaneous Partial                  
frequency), in Hz, for this Analyzer.");

	void setFreqFloor( double x );
	
%feature("docstring",
"Set the frequency resolution (minimum instantaneous frequency          
difference between Partials) for this Analyzer. (Does not cause     
other parameters to be recomputed.)");

	void setFreqResolution( double x );
	void setFreqResolution( const Envelope & e );  
	
%feature("docstring",
"Set the hop time (which corresponds approximately to the average
density of Partial envelope Breakpoint data) for this Analyzer.");

 	void setHopTime( double x );
	
%feature("docstring",
"Set the sidelobe attenutation level for the Kaiser analysis window in
positive dB. Larger numbers (e.g. 90) give very good sidelobe 
rejection but cause the window to be longer in time. Smaller 
numbers raise the level of the sidelobes, increasing the likelihood
of frequency-domain interference, but allow the window to be shorter
in time.");

	void setSidelobeLevel( double x );
	
%feature("docstring",
"Set the frequency-domain main lobe width (measured between 
zero-crossings) of the analysis window used by this Analyzer.");

	void setWindowWidth( double x );
	
%feature("docstring",
"Indicate whether the phases and frequencies of the constructed
 partials should be modified to be consistent at the end of the
 analysis. (Default is true.)");

    void setPhaseCorrect( bool TF = true );
    
    
%feature("docstring",
"Construct Partial bandwidth envelopes during analysis
by associating residual energy in the spectrum (after
peak extraction) with the selected spectral peaks that
are used to construct Partials. 

regionWidth is the width (in Hz) of the bandwidth 
association regions used by this process, must be positive.
If unspecified, a default value is used.");

	void storeResidueBandwidth( double regionWidth = Analyzer::Default_ResidueBandwidth_RegionWidth );
	
%feature("docstring",
"Construct Partial bandwidth envelopes during analysis
by storing the mixed derivative of short-time phase, 
scaled and shifted so that a value of 0 corresponds
to a pure sinusoid, and a value of 1 corresponds to a
bandwidth-enhanced sinusoid with maximal energy spread
(minimum sinusoidal convergence).

tolerance is the amount of range over which the 
mixed derivative indicator should be allowed to drift away 
from a pure sinusoid before saturating. This range is mapped
to bandwidth values on the range [0,1]. Must be positive and 
not greater than 1. If unspecified, a default value is used.");
	
%feature("compactdefaultargs") storeConvergenceBandwidth;
	
	void storeConvergenceBandwidth( double tolerance = 0.01 * Analyzer::Default_ConvergenceBandwidth_TolerancePct );

%feature("docstring",
"Disable bandwidth envelope construction. Bandwidth 
will be zero for all Breakpoints in all Partials.");
	
	void storeNoBandwidth( void );

%feature("docstring",
"Return true if this Analyzer is configured to compute
bandwidth envelopes using the spectral residue after
peaks have been identified, and false otherwise.");

	bool bandwidthIsResidue( void ) const;

%feature("docstring",
"Return true if this Analyzer is configured to compute
bandwidth envelopes using the mixed derivative convergence
indicator, and false otherwise.");

	bool bandwidthIsConvergence( void ) const;

%feature("docstring",
"Return the width (in Hz) of the Bandwidth Association regions
used by this Analyzer, only if the spectral residue method is
used to compute bandwidth envelopes. Return zero if the mixed
derivative method is used, or if no bandwidth is computed.");

	double bwRegionWidth( void ) const;

%feature("docstring",
"Return the mixed derivative convergence tolerance
only if the convergence indicator is used to compute
bandwidth envelopes. Return zero if the spectral residue
method is used or if no bandwidth is computed.");

	double bwConvergenceTolerance( void ) const;


%feature("docstring",
"Return the fundamental frequency estimate envelope constructed
during the most recent analysis performed by this Analyzer.
Will be empty unless buildFundamentalEnv was invoked to enable the
construction of this envelope during analysis.") fundamentalEnv;

    LinearEnvelope fundamentalEnv( void ) const;
        
%feature("docstring",
"Configure the fundamental frequency estimator.

fmin is the lower bound on the fundamental frequency estimate.
fmax is the upper bound on the fundamental frequency estimate
threshDb is the lower bound on the amplitude of a spectral peak
that will constribute to the fundamental frequency estimate (very
low amplitude peaks tend to have less reliable frequency estimates).
Default is -60 dB.
threshHz is the upper bound on the frequency of a spectral
peak that will constribute to the fundamental frequency estimate.
Default is 8 kHz.

The fundamental frequency estimate can be accessed by
fundamentalEnv() after the analysis is complete.
") buildFundamentalEnv;


    void buildFundamentalEnv( double fmin, double fmax, 
                              double threshDb = -60, double threshHz = 8000 );

%feature("docstring",
"Return the overall amplitude estimate envelope constructed
during the most recent analysis performed by this Analyzer.") ampEnv;

    LinearEnvelope ampEnv( void ) const;
    
%feature("docstring",
"Legacy support, do not use. The amplitude and frequency envelopes
are always estimated.") buildAmpEnv;

    void buildAmpEnv( bool TF = true );

    void buildFundamentalEnv( bool TF = true );

};	//	end of class Analyzer






// ---------------------------------------------------------------------------
//	class SdifFile
//
%feature("docstring",
"Class SdifFile represents reassigned bandwidth-enhanced Partial 
data in a SDIF-format data file. Construction of an SdifFile 
from a stream or filename automatically imports the Partial
data.") SdifFile;


%newobject SdifFile::partials;

class SdifFile
{
public:
 %feature("docstring",
"Initialize an instance of SdifFile by importing Partial data from
the file having the specified filename or path, 
or initialize an instance of SdifFile storing the Partials in
the specified PartialList. If no PartialList is specified,
construct an empty SdifFile.") SdifFile;

	SdifFile( const char * filename );
	SdifFile( void );
	 
%feature("docstring",
"Destroy this SdifFile.") ~SdifFile;

	 ~SdifFile( void );
		
%feature("docstring",
"Export the Partials represented by this SdifFile to
the file having the specified filename or path.") write; 

	void write( const char * path );

%feature("docstring",
"Export the envelope Partials represented by this SdifFile to
the file having the specified filename or path in the 1TRC
format, resampled, and without phase or bandwidth information.") write1TRC;

	void write1TRC( const char * path );
	
	%extend 
	{
		SdifFile( PartialList * l ) 
		{
			return new SdifFile( l->begin(), l->end() );
		}
	
%feature("docstring",
"Return a copy of the Partials represented by this SdifFile.") partials;

		PartialList * partials( void )
		{
			PartialList * plist = new PartialList( self->partials() );
			return plist;
		}
		 
%feature("docstring",
"Add all the Partials in a PartialList to this SdifFile.") addPartials;

		void addPartials( PartialList * l )
		{
			self->addPartials( l->begin(), l->end() );
		}
		 
		//	add members to access Markers
		// 	now much improved to take advantage of 
		// 	SWIG support for std::vector.
%feature("docstring",
"Return the (possibly empty) collection of Markers for 
this SdifFile.") markers;

		std::vector< Marker > markers( void )
		{
			return self->markers();
		}

%feature("docstring",
"Append a collection of Markers for to the existing
set of Markers for this SdifFile.") addMarkers;

		void addMarkers( const std::vector< Marker > & markers )
		{
			self->markers().insert( self->markers().end(),
			                        markers.begin(), markers.end() );
		}
	
%feature("docstring",
"Specify a new (possibly empty) collection of Markers for
this SdifFile.") setMarkers;

		void setMarkers( const std::vector< Marker > & markers )
		{
			self->markers().assign( markers.begin(), markers.end() );
		}	
	}	
		 
};	//	end of class SdifFile

// ---------------------------------------------------------------------------
//	class SpcFile
//
%feature("docstring",
"Class SpcFile represents a collection of reassigned bandwidth-enhanced
Partial data in a SPC-format envelope stream data file, used by the
real-time bandwidth-enhanced additive synthesizer implemented on the
Symbolic Sound Kyma Sound Design Workstation. Class SpcFile manages 
file I/O and conversion between Partials and envelope parameter streams.") SpcFile;

%newobject SpcFile::partials;

class SpcFile
{
public:
%feature("docstring",
"Construct and return a new SpcFile by importing envelope parameter 
streams from the file having the specified filename or path, 
or initialize an instance of SpcFile having the specified fractional
MIDI note number. If a PartialList is specified, add those
Partials to the file. Otherwise, the new SpcFile contains 
no Partials (or envelope parameter streams).
The default MIDI note number is 60 (middle C).") SpcFile;

	SpcFile( const char * filename );
	SpcFile( double midiNoteNum = 60 );

%feature("docstring",
"Destroy this SpcFile.") ~SpcFile;
   
	~SpcFile( void );
	
%feature("docstring",
"Return the sample rate for this SpcFile in Hz.") sampleRate;

	double sampleRate( void ) const;

%feature("docstring",
"Return the MIDI note number for this SpcFile.
Note number 60 corresponds to middle C.") sampleRate;

	double midiNoteNumber( void ) const;

%feature("docstring",
"Add the specified Partial to the enevelope parameter streams
represented by this SpcFile. If a label is specified, use that
label, instead of the Partial's label, for the Partial added to
the SpcFile.

A SpcFile can contain only one Partial having any given (non-zero) 
label, so an added Partial will replace a Partial having the 
same label, if such a Partial exists.

This may throw an InvalidArgument exception if an attempt is made
to add unlabeled Partials, or Partials labeled higher than the
allowable maximum.   
") addPartial;

	void addPartial( const Partial & p );
	void addPartial( const Partial & p, int label );

%feature("docstring",
"Set the fractional MIDI note number assigned to this SpcFile. 
If the sound has no definable pitch, use note number 60.0 (the default).") setMidiNoteNumber;

	void setMidiNoteNumber( double nn );
	 
%feature("docstring",
"Set the sampling freqency in Hz for the spc data in this
SpcFile. This is the rate at which Kyma must be running to ensure
proper playback of bandwidth-enhanced Spc data.
The default sample rate is 44100 Hz.") setSampleRate;

	void setSampleRate( double rate );
			 
%feature("docstring",
"Export the envelope parameter streams represented by this SpcFile to
the file having the specified filename or path. Export phase-correct 
bandwidth-enhanced envelope parameter streams if enhanced is true 
(the default), or pure sinsoidal streams otherwise.

A nonzero endApproachTime indicates that the Partials do not include a
release or decay, but rather end in a static spectrum corresponding to the
final Breakpoint values of the partials. The endApproachTime specifies how
long before the end of the sound the amplitude, frequency, and bandwidth
values are to be modified to make a gradual transition to the static spectrum.

If the endApproachTime is not specified, it is assumed to be zero, 
corresponding to Partials that decay or release normally.") write;

	void write( const char * filename, bool enhanced = true,
				double endApproachTime = 0 );

	
	%extend 
	{
		SpcFile( PartialList * l, double midiNoteNum = 60 ) 
		{
			return new SpcFile( l->begin(), l->end(), midiNoteNum );
		}
	
%feature("docstring",
"Return a copy of the Partials represented by this SdifFile.") partials;

		PartialList * partials( void )
		{
			PartialList * plist = new PartialList( self->partials().begin(), self->partials().end() );
			return plist;
		}

%feature("docstring",
"Add all the Partials in a PartialList to this SpcFile.
			
A SpcFile can contain only one Partial having any given (non-zero) 
label, so an added Partial will replace a Partial having the 
same label, if such a Partial exists.

This may throw an InvalidArgument exception if an attempt is made
to add unlabeled Partials, or Partials labeled higher than the
allowable maximum.") addPartials;

		void addPartials( PartialList * l )
		{
			self->addPartials( l->begin(), l->end() );
		}
		 
%feature("docstring",
"Return the (possibly empty) collection of Markers for 
this SpcFile.") markers;

		std::vector< Marker > markers( void )
		{
			return self->markers();
		}

%feature("docstring",
"Append a collection of Markers for to the existing
set of Markers for this SpcFile.") addMarkers;

		void addMarkers( const std::vector< Marker > & markers )
		{
			self->markers().insert( self->markers().end(),
			                        markers.begin(), markers.end() );
		}
	
%feature("docstring",
"Specify a new (possibly empty) collection of Markers for
this SpcFile.") setMarkers;

		void setMarkers( const std::vector< Marker > & markers )
		{
			self->markers().assign( markers.begin(), markers.end() );
		}
	}
	
};	//	end of class SpcFile


// ----------------------------------------------------------------
//	Include auxiliary SWIG interface files.

%include lorisPartialList.i

%include lorisFundamental.i

%include lorisEnvelope.i

%include lorisSynthesizer.i

%include lorisChannelizer.i


