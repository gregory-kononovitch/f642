;
; file    : hufdcl-2.s (cp of hufdcl-1.s)
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
				jc			.c.case1x
				; 0x
				sub			off, 2
				shl			bits, 1
				jc			.c.case01
				; 00
				mov			symb, 0
				jmp			.donehdcc
.c.case01		; 01
				mov			symb, 1
				jmp			.donehdcc
				;

				; 1x
.c.case1x:
				shl			bits, 1
				jc			.c.case11x
				; 10
				sub			off, 2
				mov			symb, 2
				jmp			.donehdcc

.c.case11x:
				jno			.c.case111x
				; 110
				shl			bits, 1
				sub			off, 3
				mov			symb, 3
				jmp			.donehdcc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.c.case111x:
				shl			bits, 2
				jc			.c.case4x
				; 1110
				sub			off, 4
				mov			symb, 4
				jmp			.donehdcc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.c.case4x:
				shl			bits, 1
				jc			.c.case5x
				; 11110
				sub			off, 5
				mov			symb, 5
				jmp			.donehdcc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.c.case5x:
				jno			.c.case6x
				shl			bits, 1
				; 111110
				sub			off, 6
				mov			symb, 6
				jmp			.donehdcc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.c.case6x:
				shl			bits, 2
				jc			.c.case7x
				; 1111110
				sub			off, 7
				mov			symb, 7
				jmp			.donehdcc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.c.case7x:
				jno			.c.case8x
				shl			bits, 1
				; 11111110
				sub			off, 8
				mov			symb, 8
				jmp			.donehdcc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.c.case8x:
				shl			bits, 2
				jc			.c.case9x
				; 111111110
				sub			off, 9
				mov			symb, 9
				jmp			.donehdcc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.c.case9x:
				jno			.c.case10x
				shl			bits, 1
				; 111111110
				sub			off, 10
				mov			symb, 10
				jmp			.donehdcc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.c.case10x:
				shl			bits, 2
				jc			.c.caseerr
				; 111111110
				sub			off, 11
				mov			symb, 11
				jmp			.donehdcc
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.c.caseerr:
				sub			off, 11
				jmp			.herr
				;
				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

