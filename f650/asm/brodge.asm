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
			mov				dword [rbp - 16], r10d
			mov				r11d, dword [rdi + 12]		; h
			mov				rsi, [rdi + 16]				; osc
			mov				ecx, dword [rdi + 24]		; nb
			mov				rdx, [rdi]					; img
			; loop
			sub				ecx, 1
			xor				rax, rax
.loop
			lea				rdi, [rsi + 8*rcx]			; osc
			push			rcx
			; x0
			mov				eax, dword [rdi]			; x0
			mov				dword [rbp - 64], eax
			mov				dword [rbp - 60], eax
			mov				dword [rbp - 56], eax
			mov				dword [rbp - 52], eax
			movaps			xmm2, oword [rbp - 64]
			; y0
			mov				eax, dword [rdi + 4]		; y0
			mov				dword [rbp - 64], eax
			mov				dword [rbp - 60], eax
			mov				dword [rbp - 56], eax
			mov				dword [rbp - 52], eax
			movaps			xmm3, oword [rbp - 64]

			jmp				osc1						; process
.ret		; osc return

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
%define		px0		xmm2
%define		py0		xmm3
;
osc1:
			;
			;
			movdqa			xmm1, [ZEROp]
.loopy
			movdqa			xmm0, [FLUSH]
			mov				r8d, dword [rbp - 16]		; w4
			;
.loopx
			movaps			xmm4, xmm0					; xi
			subps			xmm4, xmm2					; xi - x0
			mulps			xmm4, xmm4					; ²
			movaps			xmm5, xmm1					; yi
			subps			xmm5, xmm3					; yi - y0
			mulps			xmm5, xmm5
			addps			xmm4, xmm5
			sqrtps			xmm4, xmm4

.cootx		;
			addps			xmm0, [FOURp]
			sub				r8, 1
			jnz				.loopx


.cooty		;
			addps			xmm1, [ONEp]
			sub				r11d, 1
			jnz				.loopy
			jz				brodga650.ret
