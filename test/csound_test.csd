<CsoundSynthesizer>
; csound_test.csd - a Csound structured data file 
; for testing the Loris unit generators
;
;
<CsOptions>
-A -d -o csound_opcode_test.aiff
</CsOptions>

<CsInstruments>
sr = 44100
kr = 441
ksmps = 100
nchnls = 1

;
; Morph the partials in clarinet.sdif into the
; partials in flute.sdif. 
;
instr 1
    ionset   =           .25
    idecay   =           .5
    itmorph  =           p3 - (ionset + idecay)
    ipshift  =           cpspch(8.02)/cpspch(8.08)
   
    ktcl     linseg      0, ionset, .2, itmorph, 2.0, idecay, 2.1    ; clarinet time function, morph from .2 to 2.0 seconds
    ktfl     linseg      0, ionset, .5, itmorph, 2.1, idecay, 2.3    ; flute time function, morph from .5 to 2.1 seconds
   
    kmurph   linseg      0, ionset, 0, itmorph, 1, idecay, 1
    
             lorisread   ktcl, "clarinet.ctest.sdif", 1, ipshift, 2, 1, .001
             lorisread   ktfl, "flute.ctest.sdif", 2, 1, 1, 1, .001
             lorismorph  1, 2, 3, kmurph, kmurph, kmurph
    asig     lorisplay   3, 1, 1, 1
             out         asig
endin



</CsInstruments>   

<CsScore>
; play instr 1
;     strt   dur
i 1    0      2
e
</CsScore>   

</CsoundSynthesizer>
