#! python
#
#   This is the Loris C++ Class Library, implementing analysis, 
#   manipulation, and synthesis of digitized sounds using the Reassigned 
#   Bandwidth-Enhanced Additive Sound Model.
#   
#   Loris is Copyright (c) 1999-2010 by Kelly Fitz and Lippold Haken
#  
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#  
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY, without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#  
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#  
#  
#   morphtest.py
#
#   Very simple Loris instrument tone morphing demonstration using a
#   dynamically-loaded Python module.
#
#   Kelly Fitz, 13 Dec 2000
#   loris@cerlsoundgroup.org
#  
#   http://www.cerlsoundgroup.org/Loris/
#
"""
--- Welcome to the very simple Loris morphing test ---

Generates a simple linear morph between a 
clarinet and a flute using the Loris
extension module for Python.
"""
print __doc__

import loris, os, time

print ' Using Loris version', loris.version()

path = os.getcwd()
print '(in %s)' % path
try:    
    path = os.environ['srcdir']
except:
    path = os.path.join(os.pardir, 'test')
print '(looking for sources in %s)' % path

#
#   analyze clarinet tone
#
print 'analyzing clarinet 4G# (%s)' % time.ctime(time.time())
a = loris.Analyzer( 390 )
a.setFreqDrift( 30 )
a.setAmpFloor( -80 )

cf = loris.AiffFile( os.path.join(path, 'clarinet.aiff') )
v = cf.samples()
samplerate = cf.sampleRate()

clar = a.analyze( v, samplerate )

print 'checking SDIF export/import'
loris.exportSdif( 'clarinet.pytest.sdif', clar )
clar = loris.importSdif( 'clarinet.pytest.sdif' )

try:
    print 'making a bogus attempt at writing an Spc file --'
    print 'this will fail because the Partials are unchannelized'
    loris.exportSpc( 'bad_spc_file.pytest.spc', clar, 90 )
except:
    import sys
    print 'caught:', sys.exc_type
    print 'error is:' , sys.exc_value
    print 'OK, moving on!'

loris.channelize( clar, 415 )
loris.distill( clar )

# just for fun, print out the average 
# frequency of the first partial in the
# clarinet analysis:
f = 0
n = 0
import sys
if float(sys.version[:3]) >= 2.2:
    p = clar.first()
    for pos in p:
        f = f + pos.frequency()
        n = n + 1
else:
    p = clar.iterator().next()
    it = p.iterator()
    while not it.atEnd():
        f = f + it.next().frequency()
        n = n + 1
        
print "avg frequency of first distilled clarinet partial is", f/n

print 'shifting pitch of clarinet'
loris.shiftPitch( clar, loris.LinearEnvelope( -600 ) )

# check clarinet synthesis:
loris.exportAiff( 'clarOK.pytest.aiff', loris.synthesize( clar, samplerate ), samplerate, 16 )

#
#   analyze flute tone
#
print 'analyzing flute 4D (%s)' % time.ctime(time.time())
a = loris.Analyzer( 270 )       # reconfigure Analyzer
a.setFreqDrift( 30 )
v = loris.AiffFile( os.path.join(path, 'flute.aiff') ).samples()
flut = a.analyze( v, samplerate )

# loris.channelize( flut, loris.createFreqReference( flut, 291*.8, 291*1.2, 50 ), 1 )
refenv = a.fundamentalEnv()
loris.channelize( flut, refenv, 1 )
loris.distill( flut )

# check flute synthesis:
loris.exportAiff( 'flutOK.pytest.aiff', loris.synthesize( flut, samplerate ), samplerate, 16 )

# just for fun, print out the average 
# frequency of the first partial in the
# flute analysis:
fund = loris.copyLabeled( flut, 1 );
print "avg frequency of first distilled flute partial is", loris.weightedAvgFrequency( fund.first() )

#
#   perform temporal dilation
#
flute_times = [0.4, 1.]
clar_times = [0.2, 1.]
tgt_times = [0.3, 1.2]

print 'dilating sounds to match', tgt_times, '(%s)' % time.ctime(time.time())
print 'flute times:', flute_times
loris.dilate( flut, flute_times, tgt_times )
print 'clarinet times:', clar_times
loris.dilate( clar, clar_times, tgt_times )

#
#   perform morph
#
print 'morphing flute and clarinet (%s)' % time.ctime(time.time())
mf = loris.LinearEnvelope()
mf.insertBreakpoint( 0.6, 0 )
mf.insertBreakpoint( 2, 1 )
m = loris.morph( clar, flut, mf, mf, mf )
loris.exportAiff( 'morph.pytest.aiff', 
                  loris.synthesize( m, samplerate ), 
                  samplerate, 16 )
                  
#
# 	sanity check on the morph
#
# check the number and labeling of partials
# (this will fail for very old versions of Python without sets I suppose)

clarlabels = set()
for p in clar:
	clarlabels.add( p.label() )
flutlabels = set()
for p in flut:
	flutlabels.add( p.label() )
morphlabels = set()
for p in m:
	morphlabels.add( p.label() )
print 'found', len(morphlabels), 'labeled partials in the morph'

# use set algebra to check that all labels in the sources
# appear in the morph, and no others do
print 'checking Partial labels in morph...'
x = ( clarlabels | flutlabels ) ^ morphlabels
if len(x) > 0:
	raise 'ERROR in morphtest: morph does not have the right partial labels'
else:
	print 'all partial labels present in the sources are found in the morph'
	print 'and all the labels in the morph are in (at least) one of the sources'
		

[clbeg, clend] = loris.timeSpan( clar )
[flbeg, flend] = loris.timeSpan( flut )
[mbeg, mend] = loris.timeSpan( m )

# huge kludge!
# no way to determine the breakpoint gap used by the Morpher!
# Fix!
DefaultBreakpointGap = 1E-4
mbeg = mbeg + DefaultBreakpointGap
mend = mend - DefaultBreakpointGap

if not ( mbeg == clbeg ):
	raise 'ERROR in morphtest: morph begin (%f) not equal to clar begin (%f)'%(mbeg, cbeg)
if not ( mend == flend ):
	raise 'ERROR in morphtest: morph end (%f) not equal to flute end (%f)'%(mend,flend)
	
print 'morphed partials pass sanity tests'

print 'done (%s)' % time.ctime(time.time())
