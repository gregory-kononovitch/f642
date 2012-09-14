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

;%include "include1.txt"

global 	yuv422togray:		function
global 	yuv422tosgray:		function



SECTION .data	ALIGN=16
SHUF0			db		0,2,4,6,8,10,12,14,128,128,128,128,128,128,128,128
SHUF1			db		128,128,128,128,128,128,128,128,0,2,4,6,8,10,12,14


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
			shr				rdx, 4
			imul			rdx, rcx
			movaps			xmm2, oword [SHUF0]
			movaps			xmm3, oword [SHUF1]
			xorpd			xmm4, xmm4
			mov				dword [rsp - 8], r8d
			movaps			xmm4, oword [rsp - 8]
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
			ret







