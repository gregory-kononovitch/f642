;
; file    : hufacl-2.s (cp of hufdcl-1.s)
; project : f645 (t508.f640/2)
;
; Created on: Sep 23, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;
;%define		symb	cl
;%define		off		r8d
;%define		bits	r9
;;;;;;;
				;
				shl 		bits, 1
				jc			.al.case1x
				; 0x
				sub			off, 2
				shl			bits, 1
				jc			.al.case01
				; 00
				mov			symb, 1
				jmp			.donehacc
.al.case01		; 01
				mov			symb, 2
				jmp			.donehacc
				;

				; 1x
.al.case1x:
				jno			.al.case11x
				shl			bits, 2
				jc			.al.case101x
				; 100
				sub			off, 3
				mov			symb, 3
				jmp			.donehacc

.al.case101x:
				jno			.al.case1011
				; 1010
				shl			bits, 1
				sub			off, 4
				mov			sym, 0
				jmp			.donehacc

.al.case1011:
				; 1011
				shl			bits, 1
				sub			off, 4
				mov			sym, 4
				jmp			.donehacc

; ------------------------------------------------

.al.case11x:
				jno			.al.case111x
				; 110
				shl			bits, 1
				sub			off, 3
				mov			symb, 3
				jmp			.donehacc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.al.case111x:
				shl			bits, 2
				jc			.al.case4x
				; 1110
				sub			off, 4
				mov			symb, 4
				jmp			.donehacc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.al.case4x:
				shl			bits, 1
				jc			.al.case5x
				; 11110
				sub			off, 5
				mov			symb, 5
				jmp			.donehacc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.al.case5x:
				jno			.al.case6x
				shl			bits, 1
				; 111110
				sub			off, 6
				mov			symb, 6
				jmp			.donehacc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.al.case6x:
				shl			bits, 2
				jc			.al.case7x
				; 1111110
				sub			off, 7
				mov			symb, 7
				jmp			.donehacc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.al.case7x:
				jno			.al.case8x
				shl			bits, 1
				; 11111110
				sub			off, 8
				mov			symb, 8
				jmp			.donehacc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.al.case8x:
				shl			bits, 2
				jc			.al.case9x
				; 111111110
				sub			off, 9
				mov			symb, 9
				jmp			.donehacc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.al.case9x:
				jno			.al.case10x
				shl			bits, 1
				; 111111110
				sub			off, 10
				mov			symb, 10
				jmp			.donehacc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.al.case10x:
				shl			bits, 2
				jc			.al.caseerr
				; 111111110
				sub			off, 11
				mov			symb, 11
				jmp			.donehacc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.al.caseerr:
				sub			off, 11
				jmp			.herr
				;
				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

