;
; file    : faxpb.asm
; project : f640
;
; Created on: Aug 27, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;

; First try to trace a line in a int[] ~ rgb image
;
;
;

default rel

global faxpb: function


SECTION .text  align=16

; axpb(double x, double a, double b)
faxpb:
		mulsd		xmm0, xmm1
		addsd		xmm0, xmm2
        ret
;faxpb  ENDP
