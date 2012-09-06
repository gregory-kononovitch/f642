;
; file    : brodge.asm
; project : t508.f640/2
;
; Created on: Sep 6, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;

default rel

;%include "include1.txt"

%macro  begin 1
	push    rbp
	mov     rbp, rsp
	sub     rsp, %1
	push	r12
	push	r13
	push	r14
	push	r15
%endmacro

%macro  return 0
	pop		r15
	pop		r14
	pop		r13
	pop		r12
	mov     rsp, rbp
    pop     rbp
    ret
%endmacro


global 	brodga650:			function

SECTION .data
ZEROp		dd		0.0, 0.0, 0.0, 0.0
ONEp		dd		1.0, 1.0, 1.0, 1.0
FOURp		dd		4.0, 4.0, 4.0, 4.0
FLUSH		dd		0.0, 1.0, 2.0, 3.0
ALPHAp		dd		0xff000000, 0xff000000, 0xff000000, 0xff000000
WHITEp		dd		255.9, 255.9, 255.9, 255.9


;;;;;;;;;;;;;;;;;;;;;;;;;;
; long brodga650(brodge650 *brodge, bgra650 *img)
;
%define		res		0x20
;
%define		o_x0	0x30; fix - 0
%define		o_y0	0x40; fix - 16
%define		o_p		0x50; fix - 32
%define		o_m		0x60; fix - 48
%define		o_r		0x70; fix - 64
%define		o_g		0x80; 64
%define		o_b		0x90; fix - 96
;
%define		o_dist	0xA0
%define		o_fx	0xB0
%define		o_mi	0xC0
;
%define		stack_size	0xC0

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
%define		w4		r12d
%define		h1 		r13d
			;
			mov				w4, dword [rdi + 8]			; w
			mov				eax, w4
			shr				w4, 2						; w4
			mov				dword [rbp - res], w4		; w4
			mov				h1, dword [rdi + 12]		; h
			mov				dword [rbp - res + 4], h1	;
			imul			eax, h1						; size
			shl				eax, 2						; s4
			mov				dword [rbp - res + 8], eax	; s4

			;
			xor				rax, rax
			mov				dword [rbp - res + 12], eax	; sum mi
			;
			mov				qword [rbp - 16], rsi		; save bgra
			;
			mov				rsi, [rdi + 16]				; osc
			mov				ecx, dword [rdi + 24]		; nb
			mov				rdx, [rdi]					; img
			mov				qword [rbp - 8], rdx		; img
			; loop
			sub				ecx, 1
			xor				rax, rax

;
%define		imr		rdx
%define		img		r14
%define		imb		r15
%define		osc		rdi
;
.loopo:
			; images
			mov				imr, qword [rbp - 8]		; imr
			mov				img, imr					; img
			add				r14d, dword[rbp - res + 8]	; + s4
			mov				imb, img					; imb
			add				r15d, dword[rbp - res + 8]	; + s4

			; Osci
			mov				osc, rcx					; osci
			shl				osc, 3						; *8
			add				osc, rsi					; +osc
			mov				osc, [osc]

			; x0
			mov				eax, dword [osc]			; x0
			mov				dword [rbp - o_x0], eax
			mov				dword [rbp - o_x0 + 4], eax
			mov				dword [rbp - o_x0 + 8], eax
			mov				dword [rbp - o_x0 + 12], eax
			movaps			px0, oword [rbp - o_x0]

			; y0
			mov				eax, dword [osc + 4]		; y0
			mov				dword [rbp - o_y0], eax
			mov				dword [rbp - o_y0 + 4], eax
			mov				dword [rbp - o_y0 + 8], eax
			mov				dword [rbp - o_y0 + 12], eax
			movaps			py0, oword [rbp - o_y0]

			; p
			mov				eax, dword [osc + 32]		; p
			mov				dword [rbp - o_p], eax
			mov				dword [rbp - o_p + 4], eax
			mov				dword [rbp - o_p + 8], eax
			mov				dword [rbp - o_p + 12], eax
			movaps			pp, oword [rbp - o_p]

			; m
			mov				eax, dword [osc + 16]		; m
			mov				dword [rbp - o_m], eax
			mov				dword [rbp - o_m + 4], eax
			mov				dword [rbp - o_m + 8], eax
			mov				dword [rbp - o_m + 12], eax
			movaps			xmm4, oword [rbp - o_m]
			movss			xmm5, xmm4
			addss			xmm5, dword [rbp - res + 12]
			;movss			dword [rbp - res + 12], xmm5	; sum mi

			; red
			mov				eax, dword [osc + 20]		; r
			mov				dword [rbp - o_r], eax
			mov				dword [rbp - o_r + 4], eax
			mov				dword [rbp - o_r + 8], eax
			mov				dword [rbp - o_r + 12], eax
			movaps			pre, oword [rbp - o_r]
			mulps			pre, xmm4
			movaps			oword [rbp - o_r], pre

			; green
			mov				eax, dword [osc + 24]		; g
			mov				dword [rbp - o_g], eax
			mov				dword [rbp - o_g + 4], eax
			mov				dword [rbp - o_g + 8], eax
			mov				dword [rbp - o_g + 12], eax
			movaps			pgr, oword [rbp - o_g]
			mulps			pgr, xmm4
			movaps			oword [rbp - o_g], pgr

			; blue
			mov				eax, dword [osc + 28]		; b
			mov				dword [rbp - o_b], eax
			mov				dword [rbp - o_b + 4], eax
			mov				dword [rbp - o_b + 8], eax
			mov				dword [rbp - o_b + 12], eax
			movaps			pbl, oword [rbp - o_b]
			mulps			pbl, xmm4
			movaps			oword [rbp - o_b], pbl

			jmp				osc1						; process
.ret:		; osc return

			sub				ecx, 1
			cmp				ecx, 0
			jge				.loopo


return
;------------------------------------------------------------------------
rgb:		; make rgb
			mov				rsi, qword [rbp - 16]		; rest image
			; images
			mov				rdx, qword [rbp - 8]		; imr
			mov				r14, rdx					; img
			add				r14d, dword[rbp - res + 8]	; + s4
			mov				r15, r14					; imb
			add				r15d, dword[rbp - res + 8]	; + s4
			;
			; sum mi
			mov				eax, dword [rbp - res + 12]	; sum mi
			mov				dword [rbp - o_mi], eax
			mov				dword [rbp - o_mi + 4], eax
			mov				dword [rbp - o_mi + 8], eax
			mov				dword [rbp - o_mi + 12], eax
			movaps			xmm5, oword [rbp - o_mi]
			movaps			xmm4, [WHITEp]
			divps			xmm4, xmm5
			movaps			oword [rbp - o_mi], xmm4	; p 256- / smi
			;
			;
			mov				r9d, dword [rbp - res + 4]	; h
.loopy:		;
			;
			mov				r8d, dword [rbp - res]		; w4
.loopx:		;
			movaps			xmm0, oword [rdx]			; red
			mulps			xmm0, xmm4					; 255 / smi
			cvttps2dq		xmm0, xmm0
			pslld			xmm0, 16
			por				xmm0, [ALPHAp]
			;
			movaps			xmm1, oword [r14]			; green
			mulps			xmm1, xmm4					; 255 / smi
			cvttps2dq		xmm1, xmm1
			pslld			xmm0, 8
			por				xmm0, xmm1
			;
			movaps			xmm1, oword [r15]			; blue
			mulps			xmm1, xmm4					; 255 / smi
			cvttps2dq		xmm1, xmm1
			por				xmm0, xmm1
			;
			;
			movdqa			oword [rsi], xmm0
			;
.cootx:
			add				rsi, 16						; bgra
			add				imr, 16						; imr
			add				img, 16						; img
			add				imb, 16						; imb
			sub				r8d, 1						; xi
			jnz				.loopx


.cooty:		;
			sub				r9d, 1
			jnz				.loopy

			; exit
			return

;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
osc1:
			;
			movdqa			py, [ZEROp]
			mov				h1, dword [rbp - res + 4]	; h

.loopy:
			movdqa			px, [FLUSH]
			mov				w4, dword [rbp - res]		; w4
			;
.loopx:
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
;			addps			xmm4, oword [img]
;			movdqa			oword [img], xmm4
;			; blue
			movdqa			xmm4, oword [rbp - o_fx]
			mulps			xmm4, oword [rbp - o_b]
;			addps			xmm4, oword [imb]
;			movdqa			oword [imb], xmm4

			;
.cootx:		;
			add				imr, 0x10
			add				img, 0x10
			add				imb, 0x10
			addps			px, [FOURp]
			sub				w4, 1
			jnz				osc1.loopx

;-------------------
.cooty:		;
			addps			py, [ONEp]
			sub				h1, 1
			jnz				osc1.loopy
			jz				brodga650.ret
