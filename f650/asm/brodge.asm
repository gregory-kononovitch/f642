;
; file    : brodge.asm
; project : t508.f640/2
;
; Created on: Sep 6, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;

%include "include1.txt"

global 	brodga650:			function

SECTION .data
ZEROp		dd		0.0, 0.0, 0.0, 0.0
ONEp		dd		1.0, 1.0, 1.0, 1.0
FOURp		dd		4.0, 4.0, 4.0, 4.0
FLUSH		dd		0.0, 1.0, 2.0, 3.0


;;;;;;;;;;;;;;;;;;;;;;;;;;
; long brodga650(brodge650 *brodge)
brodga650:
			begin 64
			;
			mov				r10d, dword [rdi + 8]		; w
			shr				r10, 2						; w4
			mov				r10d, dword [rdi + 12]		; h
			mov				rsi, [rdi + 16]				; osc
			mov				ecx, dword [rdi + 24]		; nb
			mov				rdx, [rdi]					; img
			; loop
			sub				ecx, 1
.loop
			mov				rdi, [rsi + 8*rcx]			; osc
			push			rcx

			jmp				osc1						; process

			pop				rcx
			sub				ecx, 1
			cmp				ecx, 0
			jge				.loop

			;
			return

;;;;;;;;;;;;;;;;;;;;;;;;;;;
%define		img		rdx
%define		osc		rdi
%define		w4		r10d
%define		h 		r11d
;
osc1:
			;
			movdqa	xmm0, [FLUSH]
			;
.loopy



.cooty		;
			sub				r11d, 1
			jnz				.loopy

			ret
