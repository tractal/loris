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
 *  lorisSynthesizer.i
 *
 *  SWIG interface file describing functions associated with Loris bandwidth-
 *  enhanced sinusoidal synthesis. Include this file in loris.i to include 
 *  these classes in the scripting module. 
 *
 * Kelly Fitz, 4 Nov 2009
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 

/* ******************** inserted C++ code ******************** */
%{

#include <LorisExceptions.h>
#include <Synthesizer.h>

using Loris::Synthesizer;


%}
/* ***************** end of inserted C++ code ***************** */

/* *************** synthesis parameters struct **************** */

%feature("docstring",
"Access to Loris bandwidth-enhanced sinusoidal synthesis parameters. 
Parameters assigned here are the default parameters used in Loris 
synthesis. Some functions allow these parameters to be overridden
(see e.g. loris.synthesize).") 
SynthesisParameters;


%feature("docstring",
"Return the Loris Synthesizer's Partial fade time, in seconds.") 
SynthesisParameters::fadeTime;

%feature("docstring",
"Return the sampling rate (in Hz) for the Loris Synthesizer.") 
SynthesisParameters::sampleRate;

%feature("docstring",
"Set the Loris Synthesizer's fade time to the specified value 
(in seconds, must be non-negative).

	t is the new Partial fade time in seconds.
") 
SynthesisParameters::setFadeTime;

%feature("docstring",
"Set the Loris Synthesizer's sample rate to the specified value 
(in Hz, must be positive).

	rate is the new synthesis sample rate.
") 
SynthesisParameters::setSampleRate;

%feature("docstring",
"Return the numerator coefficients in the filter used by the Loris 
Synthesizer in bandwidth-enhanced sinusoidal synthesis.") 
SynthesisParameters::filterCoefsNumerator;

%feature("docstring",
"Return the denominator coefficients in the filter used by the Loris 
Synthesizer in bandwidth-enhanced sinusoidal synthesis.") 
SynthesisParameters::filterCoefsDenominator;

%feature("docstring",
"Set the coefficients in the filter used by the Loris 
Synthesizer in bandwidth-enhanced sinusoidal synthesis.

    num is the new numerator coefficients
    den is the new denominator coefficients
    
The zeroeth denominator coefficient must be non-zero. If it is not
equal to 1.0, all the other coefficients are scaled by its inverse.
") 
SynthesisParameters::setFilterCoefs;


//  This class does not exist, really, it is just a collection of 
//  accessors and mutators for the global default Synthesizer 
//  parameters.

%nodefault SynthesisParameters;

%inline
%{
    class SynthesisParameters
    {
    public:
    
        //  -- default fade time access and mutation --
        
        static double fadeTime( void ) 
        {
            return Synthesizer::DefaultParameters().fadeTime;
        }
    
    
        static void setFadeTime( double t )
        {
            Synthesizer::Parameters params = 
                Synthesizer::DefaultParameters();
            params.fadeTime = t;
            Synthesizer::SetDefaultParameters( params );
        }
    
    
        //  -- default sample rate access and mutation --
        
        static double sampleRate( void ) 
        {
            return Synthesizer::DefaultParameters().sampleRate;
        }
    
    
        static void setSampleRate( double rate )    
        {
            Synthesizer::Parameters params = 
                Synthesizer::DefaultParameters();
            params.sampleRate = rate;
            Synthesizer::SetDefaultParameters( params );        
        }
    
        //  -- filter access and mutation --
        
        static std::vector< double > filterCoefsNumerator( void ) 
        {
            return Synthesizer::DefaultParameters().filter.numerator();
        }
        
        static std::vector< double > filterCoefsDenominator( void )
        {
            return Synthesizer::DefaultParameters().filter.denominator();
        }                
        
        static void setFilterCoefs( std::vector< double > num, std::vector< double > den )
        {
            if ( 0. == den[0] )
            {
                Throw( InvalidArgument, 
                       "Zeroeth feedback coefficient must be non-zero." );
            }
        
            Synthesizer::Parameters params = 
                Synthesizer::DefaultParameters();
            params.filter.numerator() = num;
            params.filter.denominator() = den;
            Synthesizer::SetDefaultParameters( params );        
        }
    
    };

%}




/* ******************** synthesis functions ******************** */


%feature("docstring",
"Synthesize Partials in a PartialList at the given sample rate, and
return the (floating point) samples in a vector. The vector is
sized to hold as many samples as are needed for the complete
synthesis of all the Partials in the PartialList. 

If the sample rate is unspecified, the sample rate in the default 
SynthesisParameters is used. (See loris.SynthesisParameters.)") 
synthesize;


%newobject synthesize;
%inline 
%{
	std::vector<double> synthesize( const PartialList * partials, double srate )
	{
		std::vector<double> dst;
		try
		{
			Synthesizer synth( srate, dst );
			synth.synthesize( partials->begin(), partials->end() );
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );
		}
		return dst;
	}
	
	std::vector<double> synthesize( const PartialList * partials )
	{
		std::vector<double> dst;
		try
		{
			Synthesizer synth( dst );
			synth.synthesize( partials->begin(), partials->end() );
		}
		catch ( std::exception & ex )
		{
			throw_exception( ex.what() );
		}
		return dst;
	}
%}


