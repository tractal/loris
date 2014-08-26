<CsoundSynthesizer>
; tryit.csd - a Csound structured data file 
; for testing the Loris unit generators
;
; To try this out, copy flute and clarinet
; SDIF files from the test directory into
; this directory, naming them flute.sdif
; and clarinet.sdif.
;
<CsOptions>
-W -d -o tryit.wav
</CsOptions>

<CsInstruments>
; originally tryit.orc
sr = 44100
kr = 4410
ksmps = 10
nchnls = 1

; 
; Play the partials in clarinet.sdif 
; from 0 to 3 sec with 1 ms fadetime 
; and no frequency , amplitude, or 
; bandwidth modification.
;
instr 1
    ktime    linseg      0, p3, 3.0    ; linear time function from 0 to 3 seconds
             lorisread   ktime, "clarinet.sdif", 1, 1, 1, 1, .001
    asig     lorisplay   1, 1, 1, 1
             out         asig
endin


; 
; Play the partials in clarinet.sdif 
; from 0 to 3 sec with 1 ms fadetime 
; adding tuning and vibrato, increasing the 
; "breathiness" (noisiness) and overall
; amplitude, and adding a highpass filter.
;
instr 2
    ktime    linseg      0, p3, 3.0    ; linear time function from 0 to 3 seconds
    
    ; compute frequency scale for tuning
    ; (original pitch was G#4)
    ifscale  =           cpspch(p4)/cpspch(8.08)

    ; make a vibrato envelope
    kvenv    linseg      0, p3/6, 0, p3/6, .02, p3/3, .02, p3/6, 0, p3/6, 0
    kvib     oscil       kvenv, 4, 1   ; table 1, sinusoid

    kbwenv   linseg      1, p3/6, 1, p3/6, 2, 2*p3/3, 2
             lorisread   ktime, "clarinet.sdif", 1, 1, 1, 1, .001
    a1       lorisplay   1, ifscale+kvib, 2, kbwenv  
    a2       atone       a1, 1000      ; highpass filter, cutoff 1000 Hz
             out         a2
endin


instr 3
	idur = p3		; duration of instrument play
	ipitch = cpspch(p4) ; pitch to play
	
	ifscale = ipitch/415	; the original fundamental was about 415 Hz
	ifadetime = 0.001 	; a fadetime of 1 ms (same as Loris) will be used
	
	; make a time index signal
	ktime linseg	0.15, idur/4, 1.8, idur/4, .8, idur/2, 2.3
	
	; make a vibrato envelope
	kvenv linseg	0, idur/6, 0, idur/2, .02, idur/3, 0
	kvib oscil		kvenv, 4, 1		; table 1, sinusoid

             lorisread   ktime, "clarinet.sdif", 1, 1, 1, 1, ifadetime
	a1 lorisplay 1, ifscale+kvib, 2, 1  

	a2 atone a1, 1000  
	out a2
endin

</CsInstruments>
<CsScore>
; originally tryit.sco
; make sinusoid in table 1
f 1 0 4096 10 1

; play instr 1
;     strt   dur
i 1    0      3
i 1    +      1
i 1    +      6
s

; play instr 2
;     strt   dur   ptch
i 2     1     3    8.08
i 2     3.5   1    8.04
i 2     4     6    8.00
i 2     4     6    8.07

e
</CsScore>

</CsoundSynthesizer>
