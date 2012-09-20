;
; file    : hufdcl-1.asi
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
%define		symb	dl
%define		off		r8d
%define		bits	r9
;;;;;;;
				;
				shl 		bits, 1
				jc			.case1x
				; 0x
				shl			bits, 1
				jc			.case01x
				; 00
				sub			off, 2
				xor			symb, symb
				jmp			.donedcl
				;
				; 01x
.case01x:
				shl			bits, 1
				jc			.case011
				; 010
				sub			off, 3
				mov			symb, 1
				jmp			.donedcl

.case011:
				; 011
				sub			off, 3
				mov			symb, 2
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;
.case1x:
				shl			bits, 1
				jc			.case11x
				; 10x
				shl			bits, 1
				jc			.case101
				; 100
				sub			off, 3
				mov			symb, 3
				jmp			.donedcl

.case101:		; 101
				sub			off, 3
				mov			symb, 4
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case11x:
				shl			bits, 1
				jc			.case111x
				; 110
				sub			off, 3
				mov			symb, 5
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case111x:
				shl			bits, 1
				jc			.case4x
				; 110
				sub			off, 4
				mov			symb, 6
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case4x:
				shl			bits, 1
				jc			.case5x
				; 110
				sub			off, 5
				mov			symb, 7
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case5x:
				shl			bits, 1
				jc			.case6x
				; 110
				sub			off, 6
				mov			symb, 8
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case6x:
				shl			bits, 1
				jc			.case7x
				; 110
				sub			off, 7
				mov			symb, 9
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case7x:
				shl			bits, 1
				jc			.case8x
				; 110
				sub			off, 8
				mov			symb, 10
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case8x:
				shl			bits, 1
				jc			.err
				; 110
				sub			off, 9
				mov			symb, 11
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
