;
; file    : idft2pix-3.s
; project : f645 (t508.f640/2)
;
; Created on: Sep 23, 2012 (fork of idft2pix-2.s)
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;




;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;                            DCT Lumin
;                    -------------------------

				; ???
				test		r13b, 0xFF
				jz			.done
				;
				mov			r12d, r13d			; cp ii

				; image & corner
				xor			r15, r15
				mov			r15d, dword [rbp - VAR + y8x8i]
				imul		r15d, dword [rbp - VAR + width]
				add			r15d, dword [rbp - VAR + x8x8i]
				shl			r15d, 2								; uint32_t
				add			r15,  qword [rbp - VAR + ppix]

				; Convert zzi * quanti
				mov			ecx , r13d			; cp ii
				sub			cl, 1
				and			cl, 0xFC
				shl			cx, 2				; int
				;
.cvti:			cvtdq2ps	xmm0, oword [rbp - WORK + ZZI + rcx]
				movaps		oword [rbp - WORK + ZZIF + rcx], xmm0
				sub			cx, 16
				jge			.cvti

				; loop 128
				mov			ebx, 240
				movaps		xmm0, oword [PF128]
.loop128		movaps		oword [rsp - WORK + PIXFI + rbx], xmm0
				sub			bx, 16
				jge			.loop128

				; Main loop
				xor			rcx, rcx					; (izi)
				shl			r12w, 2						; byte offset
.loopzi:
				; zzi in all				;
				movss		xmm1, dword [rbp - WORK + ZZIF + rcx]
				pshufd		xmm1, xmm1, 0x00
				;
				mov			edx, dword [rbp - WORK + IZI + rcx]
				shl			dx, 8						; 64 * 4 : coscos line
				xor			rbx, rbx					; (x, y)
				;
.loopxy:
				;
				; uvcoscos
				movaps		xmm2, oword [UVCOSCOS + rdx + rbx]
				; mul
				mulps		xmm2, xmm1
				; add
				addps		xmm2, oword [rbp - WORK + PIXFI + rbx]
				; save
				movaps		oword [rbp - WORK + PIXFI + rbx], xmm2
				;
.coopxy:		;
				add			bx, 16		; @ sub & jnz
				cmp			bx, 256
				jl			.loopxy

.coopzi:		;
				add			cx, 4
				cmp			cx, r12w
				jl			.loopzi
				;

				; pixels
				cvtps2dq	xmm1, xmm0
				pshufb		xmm1, oword [SHUFFPIX]
				movdqa		oword [r15], xmm1

				;
				add			r15, 16
				btc			flags, 59
				jnc			.coopxy
				add 		r15, qword [rbp - VAR + y8offset]			; @ width / 8


				; relocate
				mov			edx, dword [rbp - VAR + width]
				add			dword [rbp - VAR + x8x8i], strict dword 8			; x
				cmp			dword [rbp - VAR + x8x8i], edx						; x < width
				jl			.done
				mov			dword [rbp - VAR + x8x8i], strict dword 0
				add			strict dword [rbp - VAR + y8x8i], strict dword 8

.done:
;
;				sub			r15, qword [rbp - VAR + ppix]
;				shr			r15, 2
;				mov			dword [rsi], r15d
;				mov			edx, dword [rbp - VAR + x8x8i]
;				mov			dword [rsi + 4], edx
;				mov			edx, dword [rbp - VAR + y8x8i]
;				mov			dword [rsi + 8], edx
;				mov			dword [rsi + 12], -9998
;				add			rsi, 16
;
;
				xor			rcx, rcx
				xor			rdx, rdx
				xor			rax, rax

;				; #############
;				movdqu		oword [rsi], xmm1
;				add			rsi, 16
;				; ####################
;				mov			dword [rsi], -9999
;				add			rsi, 4


;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


