;
; file    : idft2pix-2.s
; project : f645 (t508.f640/2)
;
; Created on: Sep 22, 2012 (fork of idft2pix-1.s)
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
				xor			rcx, rcx			; @@@
				xor			rbx, rbx
				mov			r12d, r13d			; cp ii
				mov			r14d, r13d			; cp ii
				; corner
				mov			r15d, dword [rsp - VAR + y8x8i]
				imul		r15d, dword [rsp - VAR + width]
				add			r15d, dword [rsp - VAR + x8x8i]

				; Convert zzi * quanti
				sub			r14b, 1
				and			r14b, 0xFC
				add			r14b, 4				; 4i >= ni
				shl			r14w, 2				; int
.cvti:			;
				cvtdq2ps	xmm0, oword [rsp - WORK + ZZI + rcx]
				movaps		oword [rsp - WORK + ZZIF + rcx], xmm0
				;
				add			cx, 16
				cmp			cx, r14w
				jl			.cvti

				; Main loop
				xor			rbx, rbx					; (x, y)
				shl			r12w, 2						; byte offset
				;
.loopxy:
				mov			r14w, bx
				shl			r14w, 2
				xor			rcx, rcx					; (izi)
				movaps		xmm0, oword [F128]
				;
.loopzi:
				; zzi in all			;
				movss		xmm1, dword [rsp - WORK + ZZIF + rcx]
				pshufd		xmm1, xmm1, 0x55
				; uvcoscos
				mov			edx, dword [rsp - WORK + IZI + rcx]
				shl			dx, 8			; 64 * 4
				add			dx, r14w
				movaps		xmm2, oword [UVCOSCOS + rdx]
				; mul & add
				mulps		xmm1, xmm2
				addps		xmm0, xmm1
.coopzi:		;
				add			cx, 4
				cmp			cx, r12w
				jl			.loopzi
				;
				;
				cvtps2dq	xmm0, xmm0
;				movaps		oword [rsp - WORK + PIXII + r14], xmm0		; [0 ; 255] mngmnt
				pshufb		xmm0, oword [SHUFFPIX]
				pextrw		rcx, xmm0, 0
				mov			word [rsp - VAR + ppix + r15], cx
				pextrw		rcx, xmm0, 1
				mov			word [rsp - VAR + ppix + r15 + 2], cx
				;
				add			r15d, 4
				btc			flags, 59
				jnc			.coopxy
				add			r15d, dword [rsp - VAR + width]			; widthx8
				sub			r15d, 8


				;
.coopxy:		;
				add			bx, 4
				cmp			bx, 64
				jl			.loopxy

;
				mov			eax, dword [rbp - VAR + x8x8i]
				mov			edx, dword [rbp - VAR + width]
				add			eax, 8
				cmp			eax, edx
				jl			.done
				mov			dword [rbp - VAR + x8x8i], strict dword 0
				add			strict dword [rbp - VAR + y8x8i], strict dword 8

.done
				xor			rcx, rcx
				xor			rdx, rdx
				xor			rax, rax

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


