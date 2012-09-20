;
; file    : mjpeg2.asm
; project : f645 (t508.f640/2)
;
; Created on: Sep 18, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;

default rel

%macro  begin 1
	push    rbp
	mov     rbp, rsp
	sub     rsp, %1
%endmacro

%macro  return 0
	mov     rsp, rbp
    pop     rbp
    ret
%endmacro


global 	hu2fman645:			function


SECTION .data		ALIGN=16
;%include "mjpeg_tables.ash"

ALIGN 16



SECTION .text		ALIGN=16



%macro	shift1 0
		sub			off, 1
		jns			.loop
		movbe		r9, qword [data]
		add			data, 8
		mov			off, 63
		jmp			.loop
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern long hu2fman645(void *dseg, int size, void *dest);
hu2fman645:
%define		symb	dl
%define		off		r8d
%define		bits	r9
%define		data	r10
%define		eof		r11
				;
				begin	64
				;
				mov			data, rdi
				mov			eof, rdi
				add			eof, rsi
				mov			dword [rbp - 8], esi
				movbe		bits, qword [data]
				mov			off, 64
				add			data, 8
				;
				xor			rcx, rcx
				mov			rsi, rdx
				xor			rdx, rdx
				xor			rdi, rdi
				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; rdx (symb) < 256     r8 (off) >= 9
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
hdcl:			;
;%include "inclu/hufdcl-1.asi"

				;
				shl 		bits, 1
				jc			.case1x
				; 0x
				jo			.case01x
				shl			bits, 1
				; 00
				sub			off, 2
				xor			symb, symb
				jmp			.donedcl
				;
				; 01x
.case01x:
				shl			bits, 2
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
				jno			.case11x
				; 10x
				shl			bits, 2
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
				shl			bits, 2
				jc			.case111x
				; 110
				sub			off, 3
				mov			symb, 5
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case111x:
				shl			bits, 1
				jc			.case4x
				; 1110
				sub			off, 4
				mov			symb, 6
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case4x:
				jno			.case5x
				; 11110
				shl			bits, 1
				sub			off, 5
				mov			symb, 7
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case5x:
				shl			bits, 2
				jc			.case6x
				; 111110
				sub			off, 6
				mov			symb, 8
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case6x:
				shl			bits, 1
				jc			.case7x
				; 1111110
				sub			off, 7
				mov			symb, 9
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case7x:
				jno			.case8x
				shl			bits, 1
				; 11111110
				sub			off, 8
				mov			symb, 10
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.case8x:
				shl			bits, 2
				jc			.err
				; 111111110
				sub			off, 9
				mov			symb, 11
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.donedcl		; cmp
;				mov			dil, byte [rsi]
;				cmp			di, dx
;				jnz			.err
				; svg
				mov			byte [rsi], dl
				add			rsi, 1
				sub			dword [rbp - 8], 1
				jz			.end
;				return
				;
				;
				test		off, 32
				jnz			hdcl
				;
				movbe		eax, dword [data]
				add			data, 4
				mov			ecx, 32
				sub			ecx, off
				shl			rax, cl
				or			bits, rax
				add			off, 32
				jmp			hdcl

.err			;
				mov			rax, data
				return

.end:			;
				;mov			rax, data
				mov		rax, rdx
				return

