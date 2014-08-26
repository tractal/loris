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
 *	lorisPartialList.i
 *
 *	SWIG interface file describing the PartialList, Partial, and
 *  Breakpoint classes, and iterators on Partials and PartialLists.
 *	Include this file in loris.i to include the PartialList class
 *	interface in the scripting module. 
 *
 * Kelly Fitz, 17 Nov 2000
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */
 
%newobject SwigPartialIterator::next;
	// but not SwigPListIterator::next
%newobject Partial::__iter__;
%newobject PartialList::__iter__;
%newobject Partial::iterator;
%newobject PartialList::iterator;

%newobject *::findAfter;
%newobject *::findNearest;
	

/* ***************** inserted C++ code ***************** */
%{
#include <Partial.h>
#include <PartialList.h>
#include <Notifier.h>

using Loris::debugger;
using Loris::Partial;
using Loris::PartialList;
using Loris::PartialListIterator;
using Loris::Breakpoint;


/*	new iterator definitions

	These are much better than the old things, more like the 
	iterators in Python 2.2 and later, very much simpler.
*/
struct SwigPListIterator
{
	PartialList & subject;
	PartialList::iterator it;

	SwigPListIterator( PartialList & l ) : subject( l ), it ( l.begin() ) {}
	SwigPListIterator( PartialList & l, PartialList::iterator i ) : subject( l ), it ( i ) {}
	~SwigPListIterator( void ) {}
	
	bool atEnd( void ) { return it == subject.end(); }
	bool hasNext( void ) { return !atEnd(); }

	Partial * next( void )
	{
		if ( atEnd() )
		{
			throw_exception("end of PartialList");
			return 0;
		}
		Partial * ret = &(*it);
		++it;
		return ret;
	}
};

typedef Partial::iterator BreakpointPosition;

struct SwigPartialIterator
{
	Partial & subject;
	Partial::iterator it;

	SwigPartialIterator( Partial & p ) : subject( p ), it ( p.begin() ) {}
	SwigPartialIterator( Partial & p, Partial::iterator i ) : subject( p ), it ( i ) {}
	~SwigPartialIterator( void ) {}
	
	bool atEnd( void ) { return it == subject.end(); }
	bool hasNext( void ) { return !atEnd(); }

	BreakpointPosition * next( void )
	{
		if ( atEnd() )
		{
			throw_exception("end of Partial");
			return 0;
		}
		BreakpointPosition * ret = new BreakpointPosition(it);
		++it;
		return ret;
	}
};

%}
/* ***************** end of inserted C++ code ***************** */

/* *********** exception handling for new iterators *********** */
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

%exception PartialList::erase
{
    char * err;
    clear_exception();
    $action
    if ((err = check_exception()))
    {
        SWIG_exception( SWIG_ValueError, err );
    }
}

/* ******** end of exception handling for new iterators ******** */

/* ***************** new PartialList iterator ****************** */

%feature("docstring",
"An iterator over a PartialList. Access Partials
in a PartialList by invoking next until atEnd 
returns true.") SwigPListIterator;

%rename (PartialListIterator) SwigPListIterator;
%nodefault SwigPListIterator;
class SwigPListIterator
{
public:
	~SwigPListIterator( void );

%feature("docstring",
"Return true if there are no more Partials in the PartialList.") atEnd;

	bool atEnd( void );

%feature("docstring",
"Return the next Partial in the PartialList that has not yet
been returned by this iterator.") next;

	Partial * next( void );

#ifdef SIWGPYTHON
    %extend
    {
%feature("docstring",
"Return this iterator.") __iter__;

        SwigPListIterator * __iter__( void )
        {
            return self;
        }

%feature("docstring",
"Return this iterator.") iterator;

        SwigPListIterator * iterator( void )
        {
            return self;
        }
    }
#endif
};

/* ************** end of new PartialList iterator ************** */

/* ******************** new Partial iterator ******************* */
%feature("docstring",
"An iterator over a Partial. Access Breakpoints
in a Partial by invoking next until atEnd 
returns true.") SwigPListIterator;

%rename (PartialIterator) SwigPartialIterator;
%nodefault SwigPartialIterator;
class SwigPartialIterator
{
public:
	~SwigPartialIterator( void );

%feature("docstring",
"Return true if there are no more Breakpoints in the Partial.") atEnd;

	bool atEnd( void );

%feature("docstring",
"Return the next Breakpoint in the Partial that has not yet
been returned by this iterator.") next;

	BreakpointPosition * next( void );
	
#ifdef SIWGPYTHON
    %extend
    {
 %feature("docstring",
"Return this iterator.") __iter__;

       SwigPartialIterator * __iter__( void )
        {
            return self;
        }

%feature("docstring",
"Return this iterator.") iterator;

        SwigPartialIterator * iterator( void )
        {
            return self;
        }
    }
#endif
};

/* **************** end of new Partial iterator **************** */

/* ************************ PartialList ************************ */

%feature("docstring",
"A PartialList represents a collection of Bandwidth-Enhanced 
Partials, each having a trio of synchronous, non-uniformly-
sampled breakpoint envelopes representing the time-varying 
frequency, amplitude, and noisiness of a single bandwidth-
enhanced sinusoid.

For more information about Bandwidth-Enhanced Partials and the  
Reassigned Bandwidth-Enhanced Additive Sound Model, refer to
the Loris website: www.cerlsoundgroup.org/Loris/") PartialList;

class PartialList
{
public:

%feature("docstring",
"Construct a new empty PartialList, or a PartialList
that is a copy of another (containing identical copies
of the Partials in another).") PartialList;

	PartialList( void );
	PartialList( const PartialList & rhs );
	
%feature("docstring",
"Delete this PartialList.") ~PartialList;
	
	~PartialList( void );
	
%feature("docstring",
"Remove all the Partials from this PartialList.") clear;

	void clear( void );	

%feature("docstring",
"Return the number of Partials in this PartialList.") size;

	unsigned long size( void );

	%extend
	{

%feature("docstring",
"Return an iterator on the Partials in this PartialList.") iterator;

		SwigPListIterator * iterator( void )
		{
			return new SwigPListIterator(*self);
		}

		#ifdef SWIGPYTHON
%feature("docstring",
"Return an iterator on the Partials in this PartialList.") __iter__;

		SwigPListIterator * __iter__( void )
		{
			return new SwigPListIterator(*self);
		}

%feature("docstring",
"Return the number of Partials in this PartialList.") __len__;

		unsigned long __len__( void ) 
		{
			return self->size();
		}
		#endif	
		

%feature("docstring",
"Append a copy of a Partial, or copies of all the Partials in
another PartialList, to this PartialList.") append;

		void append( Partial * partial )
		{
			self->insert( self->end(), *partial );
		}
		void append( PartialList * other )
		{
			self->insert( self->end(), other->begin(), other->end() );
		}
	
		//  Implement remove using a linear search to find
		//  the Partial that should be removed -- slow and
		//  gross, but the only straightforward way to make
		//  erase play nice with the new iterator paradigm
		//  (especially in Python). Raise an exception if
		//  the specified Partial is not in the list.
		//
		//  This is consistent with the behavior of remove
		//	in Python lists.
		
%feature("docstring",
"Remove the specified Partial from this PartialList. An
exception is raised if the specified Partial is not a member
of this PartialList. The Partial itself must be a member, not
merely identical to a Partial in this PartialList.") remove;

		void remove( Partial * partial )
		{
			PartialList::iterator it = self->begin();
			while ( it != self->end() )
			{
				if ( &(*it) == partial )	// compare addresses
				{
					self->erase( it );
					return;
				}
				++it;
			}
			throw_exception( "PartialList.erase(p): p not in PartialList" );
		}
		 

%feature("docstring",
"Return the first Partial this PartialList, or 0 if this
PartialList is empty.") first;

		Partial * first( void )
		{
			if ( self->empty() )
			{
				return 0;
			}
			else
			{
				return &( self->front() );
			}
		}

%feature("docstring",
"Return the last Partial this PartialList, or 0 if this
PartialList is empty.") last;

		Partial * last( void )
		{
			if ( self->empty() )
			{
				return 0;
			}
			else
			{
				return &( self->back() );
			}
		}
		
%feature("docstring",
"Construct a new a PartialList that is a copy of 
another (containing identical copies of the Partials 
in another). 

This member is deprecated, use the normal copy constructor:
   plist_copy = PartialList( plist )
") copy;

		PartialList * copy( void )
		{
			return new PartialList( *self );
		}

	}	//	end of added methods

};

/* ********************* end of PartialList ********************* */

/* ************************** Partial *************************** */

%feature("docstring",
"A Partial represents a single component in the
reassigned bandwidth-enhanced additive model. A Partial
consists of a chain of Breakpoints describing the
time-varying frequency, amplitude, and bandwidth (or
noisiness) envelopes of the component, and a 4-byte
label. The Breakpoints are non-uniformly distributed in
time. For more information about Reassigned
Bandwidth-Enhanced Analysis and the Reassigned
Bandwidth-Enhanced Additive Sound Model, refer to the
Loris website: 
    www.cerlsoundgroup.org/Loris/
") Partial;

class Partial
{
public:

%feature("docstring",
"Construct a new empty Partial, having no Breakpoints,
or a Partial that is a copy of another (containing
identical of the Breakpoints in another).") Partial;

	Partial( void );
	Partial( const Partial & );

%feature("docstring",
"Delete this Partial.") ~Partial;

	~Partial( void );
	
%feature("docstring",
"Return the label (an integer) for this Partial. The
default label is 0.") label;

	int label( void );
	
%feature("docstring",
"Return the starting phase (in radians) for this
Partial. An exception is raised if there are no
Breakpoints in this Partial.") initialPhase;

	double initialPhase( void );
	
%feature("docstring",
"Return the time (in seconds) of the first Breakpoint in
this Partial. An exception is raised if there are no
Breakpoints in this Partial.") startTime;
	
	double startTime( void );

%feature("docstring",
"Return the time (in seconds) of the last Breakpoint in
this Partial. An exception is raised if there are no
Breakpoints in this Partial.") endTime;

	double endTime( void );

%feature("docstring",
"Return the difference in time (in seconds) between the 
first and last Breakpoints in this Partial.") duration;

	double duration( void );
	
%feature("docstring",
"Return the number of Breakpoints in this Partial.") numBreakpoints;
	
	long numBreakpoints( void );
	
%feature("docstring",
"Set the label (an integer) for this Partial. Unlabeled
Partials have the default label of 0.") setLabel;

	void setLabel( int l );
		
%feature("docstring",
"Return the interpolated frequency (in Hz) of this
Partial at the specified time in seconds. The frequency
at times earlier than the first Breakpoint is the
frequency of the first Breakpoint. The frequency at
times later than the last Breakpoint is the frequency of
the last Breakpoint. An exception is raised if there are
no Breakpoints in this Partial.") frequencyAt;
	
	double frequencyAt( double time );

%feature("docstring",
"Return the interpolated amplitude of this Partial at
the specified time in seconds. The amplitude at times
earlier than the first Breakpoint and at times later
than the last Breakpoint is zero. An exception is raised
if there are no Breakpoints in this Partial.")
amplitudeAt;

	double amplitudeAt( double time );

%feature("docstring",
"Return the interpolated bandwidth (between 0 and 1) of
this Partial at the specified time in seconds. The
bandwidth at times earlier than the first Breakpoint and
at times later than the last Breakpoint is zero. An
exception is raised if there are no Breakpoints in this
Partial.") bandwidthAt;
	
	double bandwidthAt( double time );

%feature("docstring",
"Return the interpolated phase (in radians) of this
Partial at the specified time in seconds. The phase at
times earlier than the first Breakpoint is extrapolated
from phase of the first Breakpoint assuming constant
frequency. The phase at times later than the last
Breakpoint is the extrapolated from the phase of the
last Breakpoint assuming constant frequency. An
exception is raised if there are no Breakpoints in this
Partial.") phaseAt;

	double phaseAt( double time );

	%extend
	{

%feature("docstring",
"Return an iterator on the BreakpointPositions in this
Partial. Optionally, specify the initial BreakpointPosition
for the new iterator.") iterator;

		SwigPartialIterator * iterator( void )
		{
			return new SwigPartialIterator(*self);
		}
		
		SwigPartialIterator * iterator( BreakpointPosition * startHere )
		{
			return new SwigPartialIterator(*self, *startHere );
		}

%feature("docstring",
"Return an iterator on the BreakpointPositions in this 
Partial.") __iter__;

		#ifdef SWIGPYTHON
		SwigPartialIterator * __iter__( void )
		{
			return new SwigPartialIterator(*self);
		}
		#endif	

%feature("docstring",
"Remove the specified Breakpoint from this Partial. An
exception is raised if the specified Breakpoint is not a
member of this Partial. The Breakpoint itself must be a
member, not merely identical to a Breakpoint in this
Partial.") remove;

		void remove( BreakpointPosition * pos )
		{
			if ( *pos != self->end() )
			{
				*pos = self->erase( *pos );
			}
		}

%feature("docstring",
"Return the first Breakpoint this Partial, or 0 if this
Partial is empty.") first;

        Breakpoint * first( void )
        {
            if ( self->numBreakpoints() == 0 )
            {
                return 0;
            }
            else
            {
                return &( self->first() );
            }
        }

%feature("docstring",
"Return the last Breakpoint this Partial, or 0 if this
Partial is empty.") last;

        Breakpoint * last( void )
        {
            if ( self->numBreakpoints() == 0 )
            {
                return 0;
            }
            else
            {
                return &( self->last() );
            }
        }

%feature("docstring",
"Insert a copy of the Breakpoint bp into this Partial at
the specified time in seconds. Return nothing.") insert;

		void insert( double time, const Breakpoint & bp )
		{
			// return new SwigPartialIterator(*self, self->insert( time, bp ));
			self->insert( time, bp );
		}
		
%feature("docstring",
"Return a BreakpointPosition positioned at
the first Breakpoint in this Partial that is later than
the specified time. The iterator might be at its end
(return no more Breakpoints) if there are no Breakpoints
in this Partial later than the specified time.")
findAfter;

		BreakpointPosition * findAfter( double time )
		{
			BreakpointPosition p = self->findAfter( time );
			if ( p != self->end() )
			{
				return new BreakpointPosition( p );
			}
			else
			{
				return NULL;
			}
		}
	
%feature("docstring",
"Return a BreakpointPosition positioned at
the Breakpoint in this Partial that is nearest to the
specified time.") findNearest;

		BreakpointPosition * findNearest( double time )
		{
			return new BreakpointPosition( self->findNearest( time ) );
		}
	}		
};

/* *********************** end of Partial *********************** */

/* ************************* Breakpoint ************************* */

%feature("docstring",
"A Breakpoint represents a single breakpoint in the
time-varying frequency, amplitude, and bandwidth
envelope of a Reassigned Bandwidth-Enhanced Partial.

Instantaneous phase is also stored, but is only used at
the onset of a partial, or when it makes a transition
from zero to nonzero amplitude.

A Partial represents a Reassigned Bandwidth-Enhanced
model component. For more information about
Bandwidth-Enhanced Partials and the Reassigned
Bandwidth-Enhanced Additive Sound Model, refer to the
Loris website:
    www.cerlsoundgroup.org/Loris/
") Breakpoint;

class Breakpoint
{
public:	

%feature("docstring",
"Construct a new Breakkpoint having the specified
frequency (in Hz), amplitude (absolute), bandwidth
(between 0 and 1), and phase (in radians, default 0),
or a Breakpoint that is a copy of another (having
identical parameters another).") Breakpoints;

	Breakpoint( double f, double a, double b, double p = 0. );
	Breakpoint( const Breakpoint & rhs );

%feature("docstring",
"Delete this Breakpoint.") ~Breakpoint;

	~Breakpoint( void );


%feature("docstring",
"Return the frequency (in Hz) of this Breakpoint.") frequency;

	double frequency( void );
	 
%feature("docstring",
"Return the amplitude (absolute) of this Breakpoint.") amplitude;

	double amplitude( void );
	 
%feature("docstring",
"Return the bandwidth, or noisiness (0 to 1) of 
this Breakpoint.") bandwidth;

	double bandwidth( void );
	 
%feature("docstring",
"Return the phase (in radians) of this Breakpoint.") phase;

	double phase( void );
	 	
%feature("docstring",
"Set the frequency (in Hz) of this Breakpoint.") setFrequency;

	void setFrequency( double x );

%feature("docstring",
"Set the amplitude (absolute) of this Breakpoint.") setAmplitude;
	 
	void setAmplitude( double x );

%feature("docstring",
"Set the bandwidth, or noisiness (0 to 1) of 
this Breakpoint.") setBandwidth;
	 
	void setBandwidth( double x );

%feature("docstring",
"Set the phase (in radians) of this Breakpoint.") setPhase;
	 
	void setPhase( double x );
	 	
};	//	end of SWIG interface class Breakpoint


%feature("docstring",
"A BreakpointPosition represents the position of a 
Breakpoint within a Partial." ) BreakpointPosition;

%nodefault BreakpointPosition;
class BreakpointPosition
{
public:
	~BreakpointPosition( void );

	%extend
	{
%feature("docstring",
"Return the time (in seconds) of the Breakpoint at this
BreakpointPosition.") time;

		double time( void ) 
		{ 
			return self->time(); 
		}

%feature("docstring",
"Return the Breakpoint (not a copy!) at this
BreakpointPosition.") breakpoint;

		Breakpoint * breakpoint( void ) 
		{ 
			return &(self->breakpoint());
		}
		
		//	duplicate the Breakpoint interface:
		//	(not sure yet whether this is the right way)
		//
		
%feature("docstring",
"Return the frequency (in Hz) of the Breakpoint at this
BreakpointPosition.") frequency;

		double frequency( void ) { return self->breakpoint().frequency(); }

%feature("docstring",
"Return the amplitude (absolute) of the Breakpoint at this
BreakpointPosition.") amplitude;
		
		double amplitude( void ) { return self->breakpoint().amplitude(); }

%feature("docstring",
"Return the bandwidth, or noisiness (0 to 1) of the
Breakpoint at this BreakpointPosition.") bandwidth;
		
		double bandwidth( void ) { return self->breakpoint().bandwidth(); }

%feature("docstring",
"Return the phase (in radians) of the Breakpoint at this
BreakpointPosition.") phase;
		
		double phase( void ) { return self->breakpoint().phase(); }

%feature("docstring",
"Set the frequency (in Hz) of the Breakpoint at this
BreakpointPosition.") setFrequency;
			
		void setFrequency( double x ) { self->breakpoint().setFrequency( x ); }

%feature("docstring",
"Set the amplitude (absolute) of the Breakpoint at this
BreakpointPosition.") setAmplitude;
		
		void setAmplitude( double x ) { self->breakpoint().setAmplitude( x ); }

%feature("docstring",
"Set the bandwidth, or noisiness (0 to 1) of the
Breakpoint at this BreakpointPosition.") setBandwidth;
		
		void setBandwidth( double x ) { self->breakpoint().setBandwidth( x ); }

%feature("docstring",
"Set the phase (in radians) of the Breakpoint at this
BreakpointPosition.") setPhase;
		
		void setPhase( double x ) { self->breakpoint().setPhase( x ); }		
	}
};

