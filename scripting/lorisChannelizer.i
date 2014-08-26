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
 *  lorisChannelizer.i
 *
 *  SWIG interface file describing the Channelizer class, include this file 
 *  in loris.i to include Channelizer in the scripting module. 
 *
 * Kelly Fitz, 13 Oct 2009
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
 /* ***************** inserted C++ code ***************** */
%{

#include <Channelizer.h>
using Loris::Channelizer;

%}
/* ***************** end of inserted C++ code ***************** */

%import lorisEnvelope.i
%import lorisPartialList.i

// ----------------------------------------------------------------
//		experiment: wrap Channelizer class
//

%feature("docstring",
"Class Channelizer represents an algorithm for automatic labeling of
a sequence of Partials. Partials must be labeled in
preparation for morphing (see Morpher) to establish correspondences
between Partials in the morph source and target sounds. 

Channelized partials are labeled according to their adherence to a
harmonic frequency structure with a time-varying fundamental
frequency. The frequency spectrum is partitioned into
non-overlapping channels having time-varying center frequencies that
are harmonic (integer) multiples of a specified reference frequency
envelope, and each channel is identified by a unique label equal to
its harmonic number. Each Partial is assigned the label
corresponding to the channel containing the greatest portion of its
(the Partial's) energy. 

A reference frequency Envelope for channelization and the channel
number to which it corresponds (1 for an Envelope that tracks the
Partial at the fundamental frequency) must be specified. The
reference Envelope can be constructed explcitly, point by point
(using, for example, the BreakpointEnvelope class), or constructed
automatically using the FrequencyReference class. 

The Channelizer can be configured with a stretch factor, to accomodate
detuned harmonics, as in the case of piano tones. The static member
computeStretchFactor can compute the apppropriate stretch factor, given
a pair of partials. This computation is based on formulae given in 
'Understanding the complex nature of the piano tone' by Martin Keane
at the Acoustics Research Centre at the University of Aukland (Feb 2004).
The stretching factor must be non-negative (and is zero for perfectly
tunes harmonics). Even in the case of stretched harmonics, the
reference frequency envelope is assumed to track the frequency of
one of the partials, and the center frequency of the corresponding
channel, even though it may represent a stretched harmonic." ) Channelizer;

class Channelizer
{
public:
//	-- construction --
	%extend
	{
%feature("docstring",
"Construct a new Channelizer using the specified reference
Envelope to represent the a numbered channel. If the sound
being channelized is known to have detuned harmonics, a 
stretching factor can be specified (defaults to 0 for no 
stretching). The stretching factor can be computed using
the static member computeStretchFactor.

    refChanFreq is an Envelope representing the center frequency
    of a channel.
    
    refChanLabel is the corresponding channel number (i.e. 1
    if refChanFreq is the lowest-frequency channel, and all 
    other channels are harmonics of refChanFreq, or 2 if  
    refChanFreq tracks the second harmonic, etc.).
    
    stretchFactor is a stretching factor to account for detuned 
    harmonics, default is 0. 
" ) Channelizer;
	
		Channelizer( const LinearEnvelope * refChanFreq, int refChanLabel, double stretchFactor = 0 )
		{
			return new Channelizer( *refChanFreq, refChanLabel, stretchFactor );
		}
	}
	
%feature("docstring",
"
Construct a new Channelizer having a constant reference frequency.
The specified frequency is the center frequency of the lowest-frequency
channel (for a harmonic sound, the channel containing the fundamental 
Partial.

    refFreq is the reference frequency (in Hz) corresponding
    to the first frequency channel.
  
    stretchFactor is a stretching factor to account for detuned 
    harmonics, default is 0. 
" ) Channelizer;

	Channelizer( double refFreq, double stretchFactor = 0 );
	 
%feature("docstring",
"Destroy this Channelizer" ) ~Channelizer;

	~Channelizer( void );
	 
//	-- channelizing --

%feature("docstring",
"Label a Partial with the number of the frequency channel containing
the greatest portion of its (the Partial's) energy.

    partial is the Partial to label.
" ) channelize;

	void channelize( Partial & partial ) const;

	%extend 
	{
%feature("docstring",
"Assign each Partial in the specified half-open (STL-style) range
the label corresponding to the frequency channel containing the
greatest portion of its (the Partial's) energy.

    partials is the list of Partials to channelize
" ) channelize;

		void channelize( PartialList * partials ) const
		{
			self->channelize( partials->begin(), partials->end() );
		}
	}
		 
%feature("docstring",
"Compute the center frequency of one a channel at the specified
time. For non-stretched harmonics, this is simply the value
of the reference envelope scaled by the ratio of the specified
channel number to the reference channel number. For stretched
harmonics, the channel center frequency is computed using the
stretch factor. See Martin Keane, 'Understanding
the complex nature of the piano tone', 2004, for a discussion
and the source of the mode frequency stretching algorithms 
implemented here.

    time is the time (in seconds) at which to evalute 
    the reference envelope

    channel is the frequency channel (or harmonic, or vibrational     
    mode) number whose frequency is to be determined
    
    Returns the center frequency in Hz of the specified frequency channel
    at the specified time
" ) channelFrequencyAt;

    double channelFrequencyAt( double time, int channel ) const;

%feature("docstring",
"
Compute the (fractional) channel number estimate for a Partial having a
given frequency at a specified time. For ordinary harmonics, this
is simply the ratio of the specified frequency to the reference
frequency at the specified time. For stretched harmonics (as in 
a piano), the stretching factor is used to compute the frequency
of the corresponding modes of a massy string. See Martin Keane, 
'Understanding the complex nature of the piano tone', 2004, for 
the source of the mode frequency stretching algorithms 
implemented here.

    time is the time (in seconds) at which to evalute 
    the reference envelope
    
    frequency is the frequency (in Hz) for wihch the channel
    number is to be determined

    returns the channel number corresponding to the specified
    frequency and time
" ) computeChannelNumber;

    int computeChannelNumber( double time, double frequency ) const;
    
%feature("docstring",
"Compute the (fractional) channel number estimate for a Partial having a
given frequency at a specified time. For ordinary harmonics, this
is simply the ratio of the specified frequency to the reference
frequency at the specified time. For stretched harmonics (as in 
a piano), the stretching factor is used to compute the frequency
of the corresponding modes of a massy string. See Martin Keane, 
'Understanding the complex nature of the piano tone', 2004, for 
the source of the mode frequency stretching algorithms 
implemented here.

The fractional channel number is used internally to determine
a best estimate for the channel number (label) for a Partial
having time-varying frequency. 

    time is the time (in seconds) at which to evalute 
    the reference envelope

    frequency is the frequency (in Hz) for wihch the channel
    number is to be determined

    returns the fractional channel number corresponding to the specified
    frequency and time
" ) computeFractionalChannelNumber;

    double computeFractionalChannelNumber( double time, double frequency ) const;
    
    
%feature("docstring",
"Compute the reference frequency at the specified time. For non-stretched 
harmonics, this is simply the ratio of the reference envelope evaluated 
at that time to the reference channel number, and is the center frequecy
for the lowest channel. For stretched harmonics, the reference frequency 
is NOT equal to the center frequency of any of the channels, and is also
a function of the stretch factor. 

    time is the time (in seconds) at which to evalute 
    the reference envelope
" ) referenceFrequencyAt;

    double referenceFrequencyAt( double time ) const;

//	-- access/mutation --
		 
%feature("docstring",
"Return the exponent applied to amplitude before weighting
the instantaneous estimate of the frequency channel number
for a Partial. zero (default) for no weighting, 1 for linear
amplitude weighting, 2 for power weighting, etc.
Amplitude weighting is a bad idea for many sounds, particularly
those with transients, for which it may emphasize the part of
the Partial having the least reliable frequency estimate.
" ) amplitudeWeighting;

    double amplitudeWeighting( void ) const;

%feature("docstring",
"Set the exponent applied to amplitude before weighting
the instantaneous estimate of the frequency channel number
for a Partial. zero (default) for no weighting, 1 for linear
amplitude weighting, 2 for power weighting, etc.
Amplitude weighting is a bad idea for many sounds, particularly
those with transients, for which it may emphasize the part of
the Partial having the least reliable frequency estimate.
" ) setAmplitudeWeighting;

    void setAmplitudeWeighting( double expon );

%feature("docstring",
"Return the stretching factor used to account for detuned
harmonics, as in a piano tone. Normally set to 0 for 
in-tune harmonics.

The stretching factor is a small positive number for 
heavy vibrating strings (as in pianos) for which the
mass of the string significantly affects the frequency
of the vibrating modes. See Martin Keane, 'Understanding
the complex nature of the piano tone', 2004, for a discussion
and the source of the mode frequency stretching algorithms 
implemented here.
" ) stretchFactor;

    double stretchFactor( void ) const;
        
%feature("docstring",
"Set the stretching factor used to account for detuned
harmonics, as in a piano tone. Normally set to 0 for 
in-tune harmonics. The stretching factor for massy 
vibrating strings (like pianos) can be computed from 
the physical characteristics of the string, or using 
computeStretchFactor(). 

The stretching factor is a small positive number for 
heavy vibrating strings (as in pianos) for which the
mass of the string significantly affects the frequency
of the vibrating modes. See Martin Keane, 'Understanding
the complex nature of the piano tone', 2004, for a discussion
and the source of the mode frequency stretching algorithms 
implemented here.
" ) setStretchFactor;

    void setStretchFactor( double stretch );    
    
    
// -- static members --

	 
%feature("docstring",
"the stretching factor used to account for (consistently) 
detuned harmonics, as in a piano tone, from a pair of 
mode (harmonic) frequencies and numbers.

The stretching factor is a small positive number for 
heavy vibrating strings (as in pianos) for which the
mass of the string significantly affects the frequency
of the vibrating modes. See Martin Keane, 'Understanding
the complex nature of the piano tone', 2004, for a discussion
and the source of the mode frequency stretching algorithms 
implemented here.

The stretching factor is computed using computeStretchFactor,
but only a valid stretch factor will ever be assigned. If an
invalid (negative) stretching factor is computed for the
specified frequencies and mode numbers, the stretch factor
will be set to zero.

    fm is the frequency of the Mth stretched harmonic

    m is the harmonic number of the harmonic whose frequnecy is fm

    fn is the frequency of the Nth stretched harmonic

    n is the harmonic number of the harmonic whose frequnecy is fn
    
    returns    the stretching factor, usually a very small positive
    floating point number, or 0 for pefectly tuned harmonics
    (that is, if fn = n*f1).
    
    Use as argument to Channelizer.setStretchFactor()

" ) computeStretchFactor;

    static double computeStretchFactor( double fm, int m, double fn, int n );

    
};	//	end of class Channelizer

