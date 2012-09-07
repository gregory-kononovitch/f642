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
ONEpi		dd		1, 1, 1, 1
ONEp		dd		1.0, 1.0, 1.0, 1.0
FOURp		dd		4.0, 4.0, 4.0, 4.0
FLUSH		dd		0.0, 1.0, 2.0, 3.0
ALPHAp		dd		0xff000000, 0xff000000, 0xff000000, 0xff000000
WHITEp		dd		255.9, 255.9, 255.9, 255.9
ABSp		dd		0x7F7F7F7F, 0x7F7F7F7F, 0x7F7F7F7F, 0x7F7F7F7F
NEGp		dd		0x80808080, 0x80808080, 0x80808080, 0x80808080


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
%define		o_dist	0xA0
%define		o_fx	0xB0
%define		o_mi	0xC0
%define		o_h		0xD0
;
%define		o_cos	0xE0
%define		o_sin	0xF0
%define		o_sx	0x100
%define		o_sy	0x110
%define		o_ref11	0x120
%define		o_ref12	0x130
%define		o_ref21	0x140
%define		o_ref22	0x150
;
%define		stack_size	0x150

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
			mov				rax, [rsi]
			mov				qword [rbp - 16], rax		; save bgra
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
			mov				eax, dword[rbp - res + 8]
			mov				imr, qword [rbp - 8]		; imr
			mov				img, imr					; img
			add				img, rax					; + s4
			mov				imb, img					; imb
			add				imb, rax					; + s4

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

			; h
			mov				eax, dword [osc + 36]		; h
			mov				dword [rbp - o_h], eax
			mov				dword [rbp - o_h + 4], eax
			mov				dword [rbp - o_h + 8], eax
			mov				dword [rbp - o_h + 12], eax

			; m
			mov				eax, dword [osc + 16]		; m
			mov				dword [rbp - o_m], eax
			mov				dword [rbp - o_m + 4], eax
			mov				dword [rbp - o_m + 8], eax
			mov				dword [rbp - o_m + 12], eax
			movaps			xmm4, oword [rbp - o_m]
			movss			xmm5, xmm4
			addss			xmm5, dword [rbp - res + 12]
			movss			dword [rbp - res + 12], xmm5	; sum mi

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

			;
			jmp				osc1						; process
.ret:		; osc return


			sub				ecx, 1
			cmp				ecx, 0
			jge				.loopo


;------------------------------------------------------------------------
rgb:		; make rgb
			mov				rsi, qword [rbp - 16]		; rest image
			; images
			mov				eax, dword[rbp - res + 8]	; + s4
			mov				imr, qword [rbp - 8]		; imr
			mov				img, imr					; img
			add				img, rax					; + s4
			mov				imb, img					; imb
			add				imb, rax					; + s4
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
			mov				h1, dword [rbp - res + 4]	; h
.loopy:		;
			;
			mov				w4, dword [rbp - res]		; w4
.loopx:		;
			movaps			xmm0, oword [imr]			; red
			mulps			xmm0, xmm4					; 255 / smi
			movaps			xmm3, xmm0
			;
			movaps			xmm1, oword [img]			; green
			mulps			xmm1, xmm4					; 255 / smi
			maxps			xmm3, xmm1
			;
			movaps			xmm2, oword [imb]			; blue
			mulps			xmm2, xmm4					; 255 / smi
			maxps			xmm3, xmm2
			;
			movaps			xmm5, oword [WHITEp]
			divps			xmm5, xmm3
			mulps			xmm0, xmm5
			mulps			xmm1, xmm5
			mulps			xmm2, xmm5
			;
			cvttps2dq		xmm0, xmm0
;			pslld			xmm0, 16
			por				xmm0, [ALPHAp]
			cvttps2dq		xmm1, xmm1
			pslld			xmm1, 8
			por				xmm0, xmm1
			cvttps2dq		xmm2, xmm2
			pslld			xmm2, 16
			por				xmm0, xmm2
			;
			movdqa			oword [rsi], xmm0
			;

.cootx:
			add				rsi, 16						; bgra
			add				imr, 16						; imr
			add				img, 16						; img
			add				imb, 16						; imb
			sub				w4, 1						; xi
			jnz				rgb.loopx

.cooty:		;
			sub				h1, 1
			jnz				rgb.loopy

			; exit
			return

;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
osc1:
			; type / flags
			movaps			xmm0, oword [ONEp]
			movaps			xmm1, oword [ZEROp]
			movaps			xmm2, oword [ZEROp]
			movaps			xmm3, oword [ONEp]
			;
			mov				eax, dword [osc + 44]
			and				eax, 0x01
			cmp				eax, 0
			jz				.setscale
			;
			; cos
			mov				eax, dword [osc + 48]		; cos
			mov				dword [rbp - o_cos], eax
			mov				dword [rbp - o_cos + 4], eax
			mov				dword [rbp - o_cos + 8], eax
			mov				dword [rbp - o_cos + 12], eax
			mulps			xmm0, oword [rbp - o_cos]
			; sin
			mov				eax, dword [osc + 52]		; sin
			mov				dword [rbp - o_sin], eax
			mov				dword [rbp - o_sin + 4], eax
			mov				dword [rbp - o_sin + 8], eax
			mov				dword [rbp - o_sin + 12], eax
			movaps			xmm1, oword [rbp - o_sin]
			;
			movaps			xmm2, xmm1
			andps			xmm2, oword [NEGp]
			movaps			xmm3, xmm0

.setscale:
			;
			mov				eax, dword [osc + 44]
			and				eax, 0x02
			cmp				eax, 0
			jz				.refdone
			;
			; a
			mov				eax, dword [osc + 56]		; a
			mov				dword [rbp - o_sx], eax
			mov				dword [rbp - o_sx + 4], eax
			mov				dword [rbp - o_sx + 8], eax
			mov				dword [rbp - o_sx + 12], eax
			mulps			xmm0, oword [rbp - o_sx]
			mulps			xmm2, oword [rbp - o_sx]
			; b
			mov				eax, dword [osc + 60]		; b
			mov				dword [rbp - o_sy], eax
			mov				dword [rbp - o_sy + 4], eax
			mov				dword [rbp - o_sy + 8], eax
			mov				dword [rbp - o_sy + 12], eax
			mulps			xmm1, oword [rbp - o_sy]
			mulps			xmm3, oword [rbp - o_sy]


.refdone:
			movaps			oword [rbp - o_ref11], xmm0
			movaps			oword [rbp - o_ref12], xmm1
			movaps			oword [rbp - o_ref21], xmm2
			movaps			oword [rbp - o_ref22], xmm3

			;
			movdqa			py, [ZEROp]
			mov				h1, dword [rbp - res + 4]	; h

.loopy:
			movdqa			px, [FLUSH]
			mov				w4, dword [rbp - res]		; w4
			;
.loopx:
			; x - x0 : vline
			movaps			xmm4, px					; xi
			subps			xmm4, px0					; xi - x0
			; y - y0 : hline
			movaps			xmm5, py					; yi
			subps			xmm5, py0					; yi - y0

			; Axis
			mov				eax, dword [osc + 44]
			and				eax, 0x03
			cmp				eax, 0
			jz				.axisok

			; axis
			movaps			xmm6, xmm4					; x
			movaps			xmm7, xmm5					; y
			mulps			xmm6, xmm0					; x cos
			mulps			xmm7, xmm1					; y sin
			addps			xmm7, xmm6					; +
			movaps			xmm6, xmm4					; x
			movaps			xmm4, xmm7					; x done
			movaps			xmm7, xmm5					; y
			mulps			xmm6, xmm1					; x sin
			mulps			xmm7, xmm0					; y cos
			subps			xmm7, xmm6					; -
			movaps			xmm5, xmm7					; y done


.axisok:
			;
			mov				eax, dword [osc + 44]
			test			eax, 0x100
			jnz				.line
			test			eax, 0x200
			jnz				.line
			test			eax, 0x400
			jnz				.psquare
			test			eax, 0x800
			jnz				.hsquare
			test			eax, 0x1000
			jnz				.round
			test			eax, 0x2000
			jnz				.parabole
			test			eax, 0x4000
			jnz				.hyperbole




			;
.line		; sum : (1,1) line
			addps			xmm4, xmm5
			jmp				.draw

			; mul & sum : ax + b lines


.psquare	; abs+ : square
			andps			xmm4, oword [ABSp]					; abs(xi - x0)
			andps			xmm5, oword [ABSp]					; abs(yi - y0)
			addps			xmm4, xmm5
			jmp				.draw


.hsquare	; abs- : square
			andps			xmm4, oword [ABSp]					; abs(xi - x0)
			andps			xmm5, oword [ABSp]					; abs(yi - y0)
			subps			xmm4, xmm5
			andps			xmm4, oword [ABSp]					;
			jmp				.draw

.parabole	; ^2 : parabole
			mulps			xmm4, xmm4					; ^2
			subps			xmm4, xmm5					; -
			andps			xmm4, oword [ABSp]					; abs
			sqrtps			xmm4, xmm4					; sqrt dist
			jmp				.draw


.round		; dist : circle / ellipse
			mulps			xmm4, xmm4					; ^2
			mulps			xmm5, xmm5					; ^2
			addps			xmm4, xmm5					; +
			sqrtps			xmm4, xmm4					; sqrt dist
			jmp				.draw


.hyperbole	; tsid : hyperbole
			mulps			xmm4, xmm4					; ^2
			mulps			xmm5, xmm5					; ^2
			subps			xmm4, xmm5					; -
			andps			xmm4, oword [ABSp]					; abs
			sqrtps			xmm4, xmm4					; sqrt dist

; ----------
.draw
			; h
			addps			xmm4, oword [rbp - o_h]		; + h
			mulps			xmm4, pp
;			movaps			oword [rbp - o_dist], xmm4	; dist

			; fx
			cvttps2dq		xmm5, xmm4
;			paddd			xmm5, [ONEpi]
			cvtdq2ps		xmm5, xmm5
			subps			xmm4, xmm5					;
;			movaps			xmm4, xmm5
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
