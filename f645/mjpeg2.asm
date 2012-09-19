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
%define		off		r8d
%define		bits	r9
%define		data	r10
%define		eof		r11
				;
				;
				mov			data, rdi
				mov			eof, rdi
				add			eof, rsi
				movbe		bits, qword [data]
				mov			off, 64
				add			data, 8
				;

.loop:			;
				shl 		bits, 1
				jc			.case1x
				; 0x
				shl			bits, 1
				jc			.case01x
				; 00
				sub			off, 2
				mov			rdx, 0
				jmp			.donedcl
				;
				; 01x
.case01x:
				shl			bits, 1
				jc			.case011
				; 010
				sub			off, 3
				mov			rdx, 1
				jmp			.donedcl

.case011:
				; 011
				sub			off, 3
				mov			rdx, 2
				jmp			.donedcl
				;;;;;;;;;;;;;;;;;;;;;;;;;;;
.case1x:
				shl			bits, 1
				jc			.case011
				; 010










.donedcl		; svg
				mov			cl, byte [rdx]
;				mov			r11b, byte [rsi]
;				cmp			rcx, r11
;				jnz			.err

				mov			byte [rsi], cl
				add			rsi, 1
				;
				; cpt svg
				sub			r8, 1
				jmp			.end
				jz			.end
				;
				; rax
				sub			rax, 1
				jns			.reinit
				add			rdi, 8
				movbe		r9, qword [rdi]
				mov			rax, 63
				jmp			.reinit

.err			;
				mov			rax, rdi
				return

.end:			;
				mov			rax, rdi
				return

