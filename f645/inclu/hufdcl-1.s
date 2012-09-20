;
; file    : hufdcl-1.asi (->s)
; project : f645 (t508.f640/2)
;
; Created on: Sep 18, 2012
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
				jc			.o.case1x
				; 0x
				jo			.o.case01x
				shl			bits, 1
				; 00
				sub			off, 2
				mov			symb, 0
				jmp			.donehdcl
				;
				; 01x
.o.case01x:
				shl			bits, 2
				jc			.o.case011
				; 010
				sub			off, 3
				mov			symb, 1
				jmp			.donehdcl

.o.case011:
				; 011
				sub			off, 3
				mov			symb, 2
				jmp			.donehdcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;
.o.case1x:
				jno			.o.case11x
				; 10x
				shl			bits, 2
				jc			.o.case101
				; 100
				sub			off, 3
				mov			symb, 3
				jmp			.donehdcl

.o.case101:		; 101
				sub			off, 3
				mov			symb, 4
				jmp			.donehdcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.o.case11x:
				shl			bits, 2
				jc			.o.case111x
				; 110
				sub			off, 3
				mov			symb, 5
				jmp			.donehdcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.o.case111x:
				shl			bits, 1
				jc			.o.case4x
				; 1110
				sub			off, 4
				mov			symb, 6
				jmp			.donehdcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.o.case4x:
				jno			.o.case5x
				; 11110
				shl			bits, 1
				sub			off, 5
				mov			symb, 7
				jmp			.donehdcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.o.case5x:
				shl			bits, 2
				jc			.o.case6x
				; 111110
				sub			off, 6
				mov			symb, 8
				jmp			.donehdcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.o.case6x:
				shl			bits, 1
				jc			.o.case7x
				; 1111110
				sub			off, 7
				mov			symb, 9
				jmp			.donehdcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.o.case7x:
				jno			.o.case8x
				shl			bits, 1
				; 11111110
				sub			off, 8
				mov			symb, 10
				jmp			.donehdcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.o.case8x:
				shl			bits, 2
				jc			.caseerr
				; 111111110
				sub			off, 9
				mov			symb, 11
				jmp			.donehdcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.caseerr:
				sub			off, 9
				jmp			.herr
				;
				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

