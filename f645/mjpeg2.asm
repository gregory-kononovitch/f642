;
; file    : mjpeg2.asm
; project : f645 (t508.f640/2)
;
; Created on: Sep 19, 2012
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
;%include "inclu/mjpeg_tables.s"

ALIGN 16



SECTION .text		ALIGN=16


;;;;;;;;;;;;;;;
; STACK
%define		VAR			0x0100
%define		TREEL		0x17D0
%define		TREEC		0x2EA0
%define		QUANY		0x2FA0
%define		QUANC		0x30A0
; Local variables (RBP - VAR + )
%define		PTREEL		0x00
%define		PTREEC		0x08
%define		PQUANL		0x10
%define		PQUANC		0x18
;
%define		PTEST		0x80

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
;;;; rdx (symb) < 256     r8 (off) >= 9     .herr
;;;;      -> value in dl
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

hdcl:			;

%include "inclu/hufdcl-1.s"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
				;
				;
.herr			;
				mov			rax, data
				return

.donedcl		;
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


.end:			;
				;mov			rax, data
				mov		rax, rdx
				return

