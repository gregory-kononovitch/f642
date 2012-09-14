;
; file    : image1.asm
; project : t508.f640/2
;
; Created on: Sep 14, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;

default rel

%include "include1.txt"

global 	yuv422togray:		function
global 	yuv422tosgray:		function
global 	yuv422togray32:		function
global	yuv422torgb:		function



SECTION .data	ALIGN=16
ALPHA			db		0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,255
;
SHUF0			db		0,2,4,6,8,10,12,14,128,128,128,128,128,128,128,128
SHUF1			db		128,128,128,128,128,128,128,128,0,2,4,6,8,10,12,14
;
SHUFGR321		db		0,0,0,128,2,2,2,128,4,4,4,128,6,6,6,128
SHUFGR322		db		8,8,8,128,10,10,10,128,12,12,12,128,14,14,14,128
;
SHUFRGB1		db		128,128,128,128,0,128,128,128,1,128,128,128,128,128,128,128
SHUFRGB2		db		1,128,128,128,2,128,128,128,3,128,128,128,128,128,128,128
SHUFRGB3		db		3,128,128,128,4,128,128,128,5,128,128,128,128,128,128,128
;
M128			dd		-128., 0., -128., 0;
RED1			dd		+1.370705, 1., 0., 0.
GREEN1			dd		-0.698001, 1., -0.337633, 0.
BLUE1			dd		0., 1., +1.732446, 0.

SECTION .text  	ALIGN=16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; yuv422togray(void *gray, void *yuv422, int width2x16, int height)
yuv422togray:
			shr				rdx, 4
			imul			rdx, rcx
			movaps			xmm2, oword [SHUF0]
			movaps			xmm3, oword [SHUF1]
			;
.loop:
			movaps			xmm0, oword [rsi]
			pshufb			xmm0, xmm2
			add				rsi, 16
			movaps			xmm1, oword [rsi]
			pshufb			xmm1, xmm3
			por				xmm0, xmm1
			movaps			oword [rdi], xmm0
			;
			add				rsi, 16
			add				rdi, 16
			;
			sub				rdx, 1
			jnz				.loop
			;
ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; yuv422tosgray(void *gray, void *yuv422, int width2x16, int height, int shift)
yuv422tosgray:
			begin			16
			;
			shr				rdx, 4
			imul			rdx, rcx
			movaps			xmm2, oword [SHUF0]
			movaps			xmm3, oword [SHUF1]
			xorpd			xmm4, xmm4
			mov				dword [rbp - 0x10], r8d
			movaps			xmm4, oword [rbp - 0x10]
			;
.loop:
			movaps			xmm0, oword [rsi]
			psrlw			xmm0, xmm4
			psllw			xmm0, xmm4
			pshufb			xmm0, xmm2
			add				rsi, 16
			movaps			xmm1, oword [rsi]
			psrlw			xmm1, xmm4
			psllw			xmm1, xmm4
			pshufb			xmm1, xmm3
			por				xmm0, xmm1
			movaps			oword [rdi], xmm0
			;
			add				rsi, 16
			add				rdi, 16
			;
			sub				rdx, 1
			jnz				.loop
			;
return


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; yuv422togray32(void *gray32, void *yuv422, int widtha16, int height)
yuv422togray32:
			shr				rdx, 3
			imul			rdx, rcx
			movaps			xmm2, oword [SHUFGR321]
			movaps			xmm3, oword [SHUFGR322]
			;
.loop:
			movaps			xmm0, oword [rsi]
			movaps			xmm1, xmm0
			pshufb			xmm0, xmm2
			movaps			oword [rdi], xmm0
			add				rdi, 16
			pshufb			xmm1, xmm3
			movaps			oword [rdi], xmm1
			;
			add				rsi, 16
			add				rdi, 16
			;
			sub				rdx, 1
			jnz				.loop
			;
ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; yuv422torgb(void *rgb32, void *yuv422, int width2x16, int height)
yuv422torgb:
			begin			32
			;
			shr				rdx, 3
			movaps			xmm2, oword [SHUF0]
			movaps			xmm3, oword [SHUF1]
			;
.loopx:
			movaps			xmm0, oword [rsi]
			movaps			xmm1, xmm0
			pextrw			r8d, xmm1, 7				; svg
			;
			pshufb			xmm0, oword [SHUFRGB1]
;			cvtpi2ps		xmm0, xmm0					; uyv0
			subps			xmm0, oword [M128]
			movaps			xmm2, xmm0					; svg
			;
			mulps			xmm0, oword [RED1]			; red
			movaps			oword [rbp - 32], xmm0
;			movss			xmm3, oword [rbp - 28]		; y
			addss			xmm0, xmm3					; red
			cvttss2si		eax, xmm0
			mov				byte [rbp-1], al			; red
			;
			movaps			xmm0, xmm2
			mulps			xmm0, oword [GREEN1]
			movaps			oword [rbp - 32], xmm0
;			movss			xmm3, oword [rbp - 28]		; y
			addss			xmm0, xmm3					;
;			movss			xmm3, oword [rbp - 24]		; v
			addss			xmm0, xmm3					; green
			cvttss2si		eax, xmm0
			mov				byte [rbp-2], al			; green
			;
			movaps			xmm0, xmm2
			mulps			xmm0, oword [BLUE1]
			movaps			oword [rbp - 32], xmm0
;			movss			xmm3, oword [rbp - 28]		; y
			addss			xmm0, xmm3					;
;			movss			xmm3, oword [rbp - 24]		; v
			addss			xmm0, xmm3					; blue
			cvttss2si		eax, xmm0
			mov				byte [rbp-3], al			; blue



return
