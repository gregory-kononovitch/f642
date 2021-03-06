;
; file    : idft2pix-1.s
; project : f645 (t508.f640/2)
;
; Created on: Sep 22, 2012 (fork of mjpeg2.asm)
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;

;
; @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
; Left buggy for a simpler version2
; @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;                            DCT Lumin
;                    -------------------------
idctl:
				xor			rcx, rcx			; @@@
				xor			r14, r14
				xor			r15, r15
				xor			rbx, rbx
				mov			r12d, r13d			; cp ii
				mov			cl, r13b			; cp ii
				; raz
				shr			cl, 2
				shl			cl, 2
				test		r12d, 3
				jz			.m4
				add			cl, 4
				mov			byte [rbp - VAR + _vc1], cl			; cp i4i
				;
.lraz
				mov			strict dword [rbp - WORK + ZZI + 4 * r12], strict dword 0
.craz
				add			r12d, 1
				cmp			r12d, ecx
				jl			.lraz

.m4
				shr			cl, 2						; ii4
				mov			r12d, ecx					; svg ii4

				; convert & mult uvz
				xor			ecx, ecx
.loopcvt
				shl			ecx, 2
				cvtdq2ps	xmm0, oword [rbp - WORK + ZZI + 4 * rcx]
				mulps		xmm0, oword [rbp - WORK + UVZI + 4 * rcx]
				movaps		oword [rbp - WORK + ZZI + 4 * rcx], xmm0
				shr			ecx, 2

				add			cl, 1
				cmp			ecx, r12d
				jl			.loopcvt

%define		x		r15d
%define		y		r14d
%define		index	dl

.idct:			; IDCT
				xor			y, y
				xor			rbx, rbx
				xor			rdx, rdx

.loopy:
				xor			x, x
.loopx:

				; prepar cos
				xor			ecx, ecx
				;
.lprepcos
				; cos(x, c)			; @@@@@@@@@@@@@@@@ 8xthe same
				mov			ebx, x
				shl			ebx, 3
				add			ebx, dword [rbp - WORK + COLZI + 4 * rcx]
				mov			ebx, dword [COSF + 4 * rbx]				; @@@ bcp de 1.
				mov			dword [rbp - WORK + COSFI1 + 4 * rcx], ebx


				; cos(y, r)
				mov			ebx, y
				shl			ebx, 3
				add			ebx, dword [rbp - WORK + ROWZI + 4 * rcx]
				mov			ebx, dword [COSF + 4 * rbx]
				mov			dword [rbp - WORK + COSFI2 + 4 * rcx], ebx

				;
				add			ecx, 1
				cmp			ecx, r13d
				jl			.lprepcos
				;
				xor			ecx, ecx
				xorps		xmm0, xmm0
.lsum
				shl			ecx, 1
				movaps		xmm1, oword [rbp - WORK + ZZI + 8*rcx]
				mulps		xmm1, oword [rbp - WORK + COSFI1 + 8 * rcx]
				mulps		xmm1, oword [rbp - WORK + COSFI2 + 8 * rcx]
				addps		xmm0, xmm1

				;
				shr			ecx, 1
				add			cl, 1
				cmp			ecx, r12d
				jl			.lsum

				;
				xorps		xmm1, xmm1
				movaps		oword [rbp - WORK + SUMHI], xmm1		; raz
				movaps		oword [rbp - WORK + SUMHI], xmm0
				addss		xmm0, dword [rbp - WORK + SUMHI + 4]	; @@@ ??
				addss		xmm0, dword [rbp - WORK + SUMHI + 8]
				addss		xmm0, dword [rbp - WORK + SUMHI + 12]
				addss		xmm0, dword [PF128]

				;
				movss		dword [rbp - WORK + PIXFI + 4 * rdx], xmm0
				;
				add			index, 1

.cootx:
				add			x, 1
				cmp			x, 8
				jl			.loopx
.cooty:
				add			y, 1
				cmp			y, 8
				jl			.loopy


				xor			rcx, rcx
				xor			rdx, rdx
				xor			rax, rax
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


