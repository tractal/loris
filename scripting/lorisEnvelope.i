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
 *	lorisEnvelope.i
 *
 *	SWIG interface file describing the LinearEnvelope, LinearEnvelopeIterator, 
 *  and LinearEnvelopePosition classes. Include this file in loris.i to include 
 *  these class interfaces in the scripting module. 
 *
 * Kelly Fitz, 15 Sep 2009
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
 
/* ******************** inserted C++ code ********************* */
%{

typedef LinearEnvelope::iterator LinearEnvelopePosition;
 
struct SwigLinEnvIterator
{
	LinearEnvelope & subject;
	LinearEnvelopePosition it;

	SwigLinEnvIterator( LinearEnvelope & l ) : subject( l ), it ( l.begin() ) {}
	
	SwigLinEnvIterator( LinearEnvelope & l, LinearEnvelope::iterator i ) : 
		subject( l ), it ( i ) {}
		
	~SwigLinEnvIterator( void ) {}
	
	bool atEnd( void ) { return it == subject.end(); }
	bool hasNext( void ) { return !atEnd(); }

	LinearEnvelopePosition * next( void )
	{
		if ( atEnd() )
		{
			throw_exception("end of LinearEnvelope");
			return 0;
		}
		LinearEnvelopePosition * ret = new LinearEnvelopePosition(it);
		++it;
		return ret;
	}
};

%}

/* ***************** end of inserted C++ code ***************** */



/* ************ exception handling for the iterator *********** */

/*	Exception handling code copied from the SWIG manual. 
	Tastes great, less filling.
	Defined in loris.i.
*/

%include exception.i
%exception next
{
    char * err;
    clear_exception();
    $action
    if ((err = check_exception()))
    {
#if defined(SWIGPYTHON)
		%#ifndef NO_PYTHON_EXC_STOPITER
		PyErr_SetString( PyExc_StopIteration, err );
		return NULL;
		%#else
		SWIG_exception( SWIG_ValueError, err );
		%#endif
#else
        SWIG_exception( SWIG_ValueError, err );
#endif
    }
}

/* ******* end of exception handling for new iterators ******** */

/* ************** SWIG definitions for C++ classes ************ */


// ---------------------------------------------------------------------------
//	class Envelope
//
//	Is this good for anything?
		
%feature("docstring",
"Envelope is an abstract base class for all time-varying envelopes
and parameters, representing a single-valued funtion of one variable
(usually time).") Envelope;

class Envelope
{
public:

%extend
{
	Envelope * Envelope( const Envelope * other )
	{
		return other->clone();
	}
}
};

// ---------------------------------------------------------------------------
//	class LinearEnvelope
//

%newobject LinearEnvelope::__iter__;
%newobject LinearEnvelope::iterator;


%feature("docstring",
"A LinearEnvelope represents a linear segment breakpoint 
function with infinite extension at each end (that is, the 
values past either end of the breakpoint function have the 
values at the nearest end).") LinearEnvelope;


class LinearEnvelope : public Envelope
{
public:
%feature("docstring",
"Construct and return a new LinearEnvelope, empty,
or having a single breakpoint at time 0 with the 
specified value.

An LinearEnvelope can also be copied from another
instance.") LinearEnvelope;

	LinearEnvelope( void );
	LinearEnvelope( const LinearEnvelope & );
	LinearEnvelope( double initialValue );
   
   
%feature("docstring",
"Destroy this LinearEnvelope.") ~LinearEnvelope;
   
	~LinearEnvelope( void );
	
%feature("docstring",
"Insert a new breakpoint into the envelope at the specified
time and value.") insertBreakpoint;

	void insertBreakpoint( double time, double value );

%feature("docstring",
"Insert a new breakpoint into the envelope at the specified
time and value.") insert;

	void insert( double time, double value );

%feature("docstring",
"Return the (linearly-interpolated) value of the envelope
at the specified time.") valueAt; 

	double valueAt( double x ) const;		
	
%feature("docstring",
"Return the number of breakpoints in this LinearEnvelope.") size; 

	unsigned int size( void ) const;	
		

%feature("docstring",
"Add a constant value to this LinearEnvelope and return a reference
to self.") operator+=;

    LinearEnvelope & operator+=( double offset );

%feature("docstring",
"Subtract a constant value from this LinearEnvelope and return a reference
to self.") operator-=;

    LinearEnvelope & operator-=( double offset );
        
%feature("docstring",
"Scale this LinearEnvelope by a constant value and return a reference
to self.") operator*=;

     LinearEnvelope & operator*=( double scale );

%feature("docstring",
"Divide this LinearEnvelope by a constant value and return a reference
to self.") operator*=;

     LinearEnvelope & operator/=( double div );


//  --- add members for binary arithmetic operators ---
//
//	These have to be extensions, because in C++, 
//	they are non-members.

%extend 
{
%feature("docstring",
"Add a constant value or a LinearEnvelope to this LinearEnvelope 
and return a new LinearEnvelope.") operator+;

    LinearEnvelope operator+( double offset )
    {
        LinearEnvelope env = *self; 
        env += offset;
        return env;
    }
    
    LinearEnvelope operator+( LinearEnvelope & rhs )
    {
        LinearEnvelope env = *self + rhs; 
        return env;
    }    

%feature("docstring",
"Subtract a constant value or a LinearEnvelope from this LinearEnvelope 
and return a new LinearEnvelope.") operator+;

    LinearEnvelope operator-( double offset )
    {
        LinearEnvelope env = *self; 
        env -= offset;
        return env;
    }


    LinearEnvelope operator-( LinearEnvelope & rhs )
    {
        LinearEnvelope env = *self - rhs; 
        return env;
    }    

%feature("docstring",
"Scale a LinearEnvelope by a constant value and return a new 
LinearEnvelope.") operator*;

    LinearEnvelope operator*( double scale )
    {
        LinearEnvelope env = *self; 
        env *= scale;
        return env;
    }

%feature("docstring",
"Divide a LinearEnvelope by a constant value and return a new 
LinearEnvelope.") operator/;

    LinearEnvelope operator/( double div )
    {
        LinearEnvelope env = *self; 
        env /= div;
        return env;
    }

//  --- add members iterators ---

%feature("docstring",
"Return an iterator on the positions in this
LinearEnvelope. Optionally, specify the initial position
for the new iterator.") iterator;

	SwigLinEnvIterator * iterator( void )
	{
		return new SwigLinEnvIterator(*self);
	}
	
	SwigLinEnvIterator * iterator( LinearEnvelope::iterator * startHere )
	{
		return new SwigLinEnvIterator(*self, *startHere );
	}

%feature("docstring",
"Return an iterator on the positions in this 
LinearEnvelope.") __iter__;

	#ifdef SWIGPYTHON
	SwigLinEnvIterator * __iter__( void )
	{
		return new SwigLinEnvIterator(*self);
	}
	#endif	
		

}   //  end of extend
     	 
};	//	end of class LinearEnvelope


// ---------------------------------------------------------------------------
//	class LinearEnvelopePosition
//


%feature("docstring",
"A LinearEnvelopePosition represents a breakpoint, a value
associated with a particular time in a LinearEnvelope.") LinearEnvelopePosition;


%nodefault LinearEnvelopePosition;
class LinearEnvelopePosition
{
public:
	~LinearEnvelopePosition( void );

	%extend
	{
%feature("docstring",
"Return the time (in seconds) of the corresponding to this
LinearEnvelopePosition.") time;
	
		double time( void ) const 
		{
			return (*self)->first;
		}
		
%feature("docstring",
"Return the value (arbitrary units) associated with this
LinearEnvelopePosition.") value;

		double value( void ) const
		{
			return (*self)->second;
		}
		
%feature("docstring",
"Assign the value (arbitrary units) associated with this
LinearEnvelopePosition.") setValue;

		void setValue( double x )
		{
			(*self)->second = x;
		}
	}
	
};	//	end of class LinearEnvelopePosition


// ---------------------------------------------------------------------------
//	class LinearEnvelopeIterator
//


%rename (LinearEnvelopeIterator) SwigLinEnvIterator;

%nodefault SwigLinEnvIterator;

%newobject SwigLinEnvIterator::next;


%feature("docstring",
"An iterator over a LinearEnvelope. Access time-value pairs
in a LinearEnvelope by invoking next until atEnd 
returns true.") SwigLinEnvIterator;

class SwigLinEnvIterator
{
public:
	~SwigLinEnvIterator( void );

%feature("docstring",
"Return true if there are no more time-value pairs in the LinearEnvelope.") atEnd;

	bool atEnd( void );

%feature("docstring",
"Return the next time-value pair in the LinearEnvelope that has not yet
been returned by this iterator.") next;

	LinearEnvelopePosition * next( void );

#ifdef SIWGPYTHON
    %extend
    {
%feature("docstring",
"Return this iterator.") __iter__;

        SwigLinEnvIterator * __iter__( void )
        {
            return self;
        }

%feature("docstring",
"Return this iterator.") iterator;

        SwigLinEnvIterator * iterator( void )
        {
            return self;
        }
    }
#endif

};	//	end of class LinearEnvelopeIterator

// ---------------------------------------------------------------------------
//	class BreakpointEnvelope - deprecated
//

%feature("docstring",
"BreakpointEnvelope is deprecated, use LinearEnvelope instead.") BreakpointEnvelope;

%inline 
%{

	LinearEnvelope * BreakpointEnvelope( void ) 
	{
		return new LinearEnvelope();
	}
	LinearEnvelope * BreakpointEnvelope( const LinearEnvelope *rhs )
	{
		return new LinearEnvelope( *rhs );
	}
	LinearEnvelope * BreakpointEnvelope( double initialValue )
	{
		return new LinearEnvelope( initialValue );
	}

%}


%feature("docstring",
"BreakpointEnvelopeWithValue is deprecated, use LinearEnvelope instead.") 
BreakpointEnvelopeWithValue;

%inline 
%{
	LinearEnvelope * 
	BreakpointEnvelopeWithValue( double initialValue )
	{
		return new LinearEnvelope( initialValue );
	}
%}

/* ********** end of SWIG definitions for C++ classes ********* */

