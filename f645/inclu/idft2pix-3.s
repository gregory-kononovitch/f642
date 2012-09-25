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

				; image & corner ( (4,7) )
				xor			r15, r15
				mov			r15d, dword [rbp - VAR + y8x8i]
				imul		r15d, dword [rbp - VAR + width]
				add			r15d, dword [rbp - VAR + x8x8i]
				shl			r15d, 2								; uint32_t
				;
				add			r15,  qword [rbp - VAR + ppix]
				btr			flags, 59

				; Convert ZZiQuanti
				mov			ecx , r13d			; cp ii
				sub			cl, 1
				and			cl, 0xFC
				shl			cx, 2				; int
.cvti:			cvtdq2ps	xmm0, oword [rbp - WORK + ZZI + rcx]
				movaps		oword [rbp - WORK + ZZIF + rcx], xmm0
				sub			cx, 16
				jge			.cvti

				; loop 128
				mov			ebx, 240
				movaps		xmm0, oword [PF128]
.loop128		movaps		oword [rbp - WORK + PIXFI + rbx], xmm0
				sub			bx, 16
				jns			.loop128

				; Only 1 loop ?
				cmp			r13b, 5
				jl			.loop0

				; Main loop
				mov			ecx, r13d					; (izi)
				sub			cl, 1
				shl			cl, 2						; byte offset
.loopzi:
				; zzi in all							;
				movss		xmm1, dword [rbp - WORK + ZZIF + rcx]
				pshufd		xmm1, xmm1, 0x00
				;
				mov			edx, dword [rbp - WORK + IZI + rcx]
				shl			dx, 8						; 64 * 4 : coscos line
				mov			ebx, 240					; (x, y)
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
				sub			bx, 16						;
				jns			.loopxy

.coopzi:		;
				sub			cl, 4
				jg			.loopzi
				;

;;; last/only loop
.loop0
				; zzi in all							;
				movss		xmm1, dword [rbp - WORK + ZZIF]
				pshufd		xmm1, xmm1, 0x00

				;
				mov			edx, dword [rbp - WORK + IZI]
				shl			dx, 8						; 64 * 4 : coscos line
				mov			ebx, 240					; (x, y)
				;
.loopxy0:
				;
				; uvcoscos
				movaps		xmm2, oword [UVCOSCOS + rdx + rbx]

				; mul
				mulps		xmm2, xmm1

				; add
				addps		xmm2, oword [rbp - WORK + PIXFI + rbx]

;				; [mov]		;
;				movaps		oword [rbp - WORK + PIXFI + rbx], xmm2

				; cvt
				cvtps2dq	xmm2, xmm2					; round


%define F645_0255
%ifdef F645_0255
%define stkpix rbp - WORK + PIXII + rbx
				;
				; mov
				movaps		oword [stkpix], xmm2

				; 0 - 255

				;
				test		qword [stkpix], 0x1000100
				jnz			.cmp0
				test		qword [stkpix + 8], 0x1000100
				jnz			.cmp2

				; 0-255
.cmp0			test		dword [stkpix], 256
				jz			.cmp1
				test		dword [stkpix], 0x10000000
				jnz			.cmp0_0
				mov 		dword [stkpix], strict dword 255
				jmp			.cmp1
.cmp0_0:		mov 		dword [stkpix], strict dword 0

.cmp1:			test		dword [stkpix + 4], 256
				jz			.tst2
				test		dword [stkpix + 4], 0x10000000
				jnz			.cmp1_0
				mov 		dword [stkpix + 4], strict dword 255
				jmp			.tst2
.cmp1_0:		mov 		dword [stkpix + 4], strict dword 0

.tst2			test		qword [stkpix + 8], 0x1000100
				jnz			.index
.cmp2:			test		dword [stkpix + 8], 256
				jz			.cmp3
				test		dword [stkpix + 8], 0x10000000
				jnz			.cmp2_0
				mov 		dword [stkpix + 8], strict dword 255
				jmp			.cmp3
.cmp2_0:		mov 		dword [stkpix + 8], strict dword 0

.cmp3:			test		dword [stkpix + 12], 256
				jz			.pix
				test		dword [stkpix + 12], 0x10000000
				jnz			.cmp3_0
				mov 		dword [stkpix + 12], strict dword 255
				jmp			.pix
.cmp3_0:		mov 		dword [stkpix + 12], strict dword 0

%endif

.pix			; pix
				movaps		xmm2, oword [stkpix]
				pshufb		xmm2, [SHUFFBeW]
				movaps		oword [r15], xmm2


.index:			; index
				sub			r15, 16
				btc			flags, 59
				jnc			.coopxy0
				sub 		r15, qword [rbp - VAR + y8offset3]			; @ width / 8
				;

.coopxy0:		;
				sub			bx, 16						;
				jge			.loopxy0

;
				; move block
				mov			edx, dword [rbp - VAR + width]
				add			dword [rbp - VAR + x8x8i], strict dword 8			; x
				cmp			dword [rbp - VAR + x8x8i], edx						; x < width
				jl			.done
				mov			dword [rbp - VAR + x8x8i], strict dword 4
				add			strict dword [rbp - VAR + y8x8i], strict dword 8

.done:
				xor			rcx, rcx
				xor			rdx, rdx
				xor			rax, rax

;				; #############
;				sub			r15,
;				movdqu		qword [rsi], xmm1
;				add			rsi, 8
;				; ####################
;				mov			dword [rsi], -9999
;				add			rsi, 4


;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


