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
; long brodga650(brodge650 *brodge, bgra650 *img)
;
%define		res		0x20
;
%define		fix		0x20; res + 0x70
%define		o_x0	0x30; fix - 0
%define		o_y0	0x40; fix - 16
%define		o_p		0x50; fix - 32
%define		o_m		0x60; fix - 48
%define		o_r		0x70; fix - 64
%define		o_g		0x80; 64
%define		o_b		0x90; fix - 96
;
%define		o_var	0xA0; fix + 64
%define		o_dist	0xA0
%define		o_fx	0xB0
;
%define		stack_size	0xB0

;
%define		px		xmm8
%define		py		xmm9
%define		px0		xmm10
%define		py0		xmm11
%define		pp		xmm12
%define		pre		xmm13
%define		pgr		xmm14
%define		pbl		xmm15

brodga650:
			begin stack_size
			;
			mov				r10d, dword [rdi + 8]		; w
			mov				eax, r10d
			shr				r10, 2						; w4
			mov				dword [rbp - res], r10d		; w4
			mov				r11d, dword [rdi + 12]		; h
			mov				dword [rbp - res + 4], r11d	;
			imul			eax, r11d					; size
			shl				eax, 2						; s4
			mov				dword [rbp - res + 8], eax	;
			;
			mov				rsi, [rdi + 16]				; osc
			mov				ecx, dword [rdi + 24]		; nb
			mov				rdx, [rdi]					; img
			mov				qword [rbp - 8], rdx		; img
			; loop
			sub				ecx, 1
			xor				rax, rax
.loop
			mov				rdx, qword [rbp - 8]		; imr
			mov				r14, rdx					; img
			add				r14, qword[rbp -res - 12]	; + s4
			mov				r15, r14					; imb
			add				r15, qword[rbp -res - 12]	; + s4
			;
			lea				rdi, [rsi + 8*rcx]			; osc
			push			rcx
			; x0
			mov				eax, dword [rdi]			; x0
			mov				dword [rbp - o_x0], eax
			mov				dword [rbp - o_x0 + 4], eax
			mov				dword [rbp - o_x0 + 8], eax
			mov				dword [rbp - o_x0 + 12], eax
			movaps			px0, oword [rbp - o_x0]
			; y0
			mov				eax, dword [rdi + 4]		; y0
			mov				dword [rbp - o_y0], eax
			mov				dword [rbp - o_y0 + 4], eax
			mov				dword [rbp - o_y0 + 8], eax
			mov				dword [rbp - o_y0 + 12], eax
			movaps			py0, oword [rbp - o_y0]
			; p
			mov				eax, dword [rdi + 32]		; p
			mov				dword [rbp - o_p], eax
			mov				dword [rbp - o_p + 4], eax
			mov				dword [rbp - o_p + 8], eax
			mov				dword [rbp - o_p + 12], eax
			movaps			pp, oword [rbp - o_p]
			; m
			mov				eax, dword [rdi + 16]		; m
			mov				dword [rbp - o_m], eax
			mov				dword [rbp - o_m + 4], eax
			mov				dword [rbp - o_m + 8], eax
			mov				dword [rbp - o_m + 12], eax
			movaps			xmm4, oword [rbp - o_m]
			; red
			mov				eax, dword [rdi + 20]		; r
			mov				dword [rbp - o_r], eax
			mov				dword [rbp - o_r + 4], eax
			mov				dword [rbp - o_r + 8], eax
			mov				dword [rbp - o_r + 12], eax
			movaps			pre, oword [rbp - o_r]
			mulps			pre, xmm4
			movaps			oword [rbp - o_r], pre
			; green
			mov				eax, dword [rdi + 24]		; g
			mov				dword [rbp - o_g], eax
			mov				dword [rbp - o_g + 4], eax
			mov				dword [rbp - o_g + 8], eax
			mov				dword [rbp - o_g + 12], eax
			movaps			pgr, oword [rbp - o_g]
			mulps			pgr, xmm4
			movaps			oword [rbp - o_g], pgr
			; blue
			mov				eax, dword [rdi + 28]		; b
			mov				dword [rbp - o_b], eax
			mov				dword [rbp - o_b + 4], eax
			mov				dword [rbp - o_b + 8], eax
			mov				dword [rbp - o_b + 12], eax
			movaps			pbl, oword [rbp - o_b]
			mulps			pbl, xmm4
			movaps			oword [rbp - o_b], pbl

			jmp				osc1						; process
.ret		; osc return

			pop				rcx
			sub				ecx, 1
			cmp				ecx, 0
			jge				.loop

			;
			return

;;;;;;;;;;;;;;;;;;;;;;;;;;;
%define		imr		rdx
%define		img		r14
%define		imb		r15
%define		osc		rdi
%define		w4		r12d
%define		h1 		r13d
;
osc1:
			;
			;
			movdqa			py, [ZEROp]
			mov				h1, dword [rbp - res + 4]	; h
.loopy
			movdqa			px, [FLUSH]
			mov				w4, dword [rbp - res]		; w4
			;
.loopx
			; dist
			movaps			xmm4, px					; xi
			subps			xmm4, px0					; xi - x0
			mulps			xmm4, xmm4					; ^2
			movaps			xmm5, py					; yi
			subps			xmm5, py0					; yi - y0
			mulps			xmm5, xmm5					; ^2
			addps			xmm4, xmm5					; +
			sqrtps			xmm4, xmm4					; sqrt
			mulps			xmm4, pp
			movaps			oword [rbp - o_dist], xmm4	; dist
			; fx
			cvttps2dq		xmm5, xmm4
			cvtdq2ps		xmm5, xmm5
			subps			xmm4, xmm5					;
			movaps			oword [rbp - o_fx], xmm4	; frac()
			; red
			mulps			xmm4, oword [rbp - o_r]
			addps			xmm4, oword [imr]
			movdqa			oword [imr], xmm4
			; green
			movdqa			xmm4, oword [rbp - o_fx]
			mulps			xmm4, oword [rbp - o_g]
			addps			xmm4, oword [img]
			movdqa			oword [img], xmm4
			; blue
			movdqa			xmm4, oword [rbp - o_fx]
			mulps			xmm4, oword [rbp - o_b]
			addps			xmm4, oword [imb]
			movdqa			oword [imb], xmm4


.cootx		;
			add				imr, 16
			add				img, 16
			add				imb, 16
			addps			px, [FOURp]
			sub				w4, 1
			jnz				.loopx


.cooty		;
			addps			py, [ONEp]
			sub				h1, 1
			jnz				.loopy
			jz				brodga650.ret
