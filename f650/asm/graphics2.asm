


default rel

global draw_line2a650: 	function
global asm_tst1650:		function


SECTION .data
ZERO	dq				0.0
ZEROf	dd				0.0
ONE		dq				1.0
HALF	dq				0.5
HALFf	dd				0.5
PAS		dq				0.65
FOURf	dd				4.0
; special case
ZERO#	dq				0.001
ONE#	dq				1.001
HALF_	dq				0.499



SECTION .text  align=16


; double a650_draw_line(image *img, double x1, double y1, double x2, double y2, uint32_t color)
; x1 -> xmm0
; y1 -> xmm1
; x2 -> xmm2
; y2 -> xmm3
;
; a  -> xmm8
; b  -> xmm9
; w  -> xmm10
; h  -> xmm11
; x0 -
draw_line2a650:
			xor				rcx, rcx
.prepx:		; prepar : x = sx * (x - x0) + width / 2
			movsd			xmm4, qword [rdi + 16]	; x0 -> xmm4
			subsd			xmm0, xmm4			; x1 = x1 - x0
			subsd			xmm2, xmm4			; x2 = x2 - x0
			movsd			xmm5, qword [rdi + 32]	; sx -> xmm5
			mulsd			xmm0, xmm5			; x1 = sx * x1
			mulsd			xmm2, xmm5			; x2 = sx * x2
			xor				rax, rax
			mov				ax, word [rdi + 8]	; ax = width
			cvtsi2sd		xmm10, eax			; width -> xmm10
			movsd			xmm4, xmm10			; width -> xmm4
			mulsd			xmm4, qword [HALF]	; .5 -> xmm5
			addsd			xmm0, xmm4			; x1 = x1 + w2
			addsd			xmm2, xmm4			; x2 = x2 + w2

			; tests x
			; if (x1 < 0 && x2 < 0) return (opt : if (x1 >= 0 || x2 >= 0) ok)
			ucomisd			xmm0, [ZERO]
			jae				.tst1				; xmm0 >= 0
			;
			ucomisd			xmm2, xmm4
			jb				NOPIX				; xmm2 < 0

.tst1:		; if (x1 >= w && x2 >= w) return (opt : if (x1 < w || x2 < w) ok)
			ucomisd			xmm0, xmm10
			jb				.prepy				; xmm0 >= width
			;
			ucomisd			xmm2, xmm10
			jae				NOPIX

.prepy:		; prepar : y = height / 2 - sy * (y - y0)
			movsd			xmm4, [rdi + 24]	; y0 -> xmm4
			subsd			xmm1, xmm4			; y1 = y1 - y0
			subsd			xmm3, xmm4			; y2 = y2 - y0
			movsd			xmm5, [rdi + 40]	; sy -> xmm5
			mulsd			xmm1, xmm5			; y1 = sy * y1
			mulsd			xmm3, xmm5			; y2 = sy * y2
			xor				rax, rax
			mov				ax, [rdi + 10]		; ax = height
			cvtsi2sd		xmm11, eax			; height -> xmm11
			movsd			xmm4, xmm11
			mulsd			xmm4, qword [HALF]	; h2 -> xmm4
			movsd			xmm5, xmm4			; h2 -> xmm5
			subsd			xmm4, xmm1			; xmm4 = h2 - y1
			movsd			xmm1, xmm4			;
			subsd			xmm5, xmm3			; xmm5 = h2 - y2
			movsd			xmm3, xmm5

			; tests y
			; if (y1 < 0 && y2 < 0) return (opt : if (y1 >= 0 || y2 >= 0) ok)
			ucomisd			xmm1, [ZERO]
			jae				.tst2				; xmm1 >= 0
			; y2
			ucomisd			xmm3, [ZERO]
			jb				NOPIX				; xmm3 < 0

.tst2:		; if (y1 >= h && y2 >= h) return (opt : if (y1 < h || y2 < h) ok)
			ucomisd			xmm1, xmm11
			jb				.hvp
			;
			ucomisd			xmm3, xmm11
			jae				NOPIX

			; ---------------------------
.hvp		; test hline, vline, point
			xor				r8, r8
			xor				r9, r9
			xor				r10, r10
			xor				r11, r11
			cvttsd2si		r9, xmm1
			cvttsd2si		r11, xmm3
			cmp				r9, r11
			jz				.hl
			; y1 != y2
			cvttsd2si		r8, xmm0
			cvttsd2si		r10, xmm2
			cmp				r8, r10
			jz				vline
			jmp				.abs
.hl			; y1 == y2
			cvttsd2si		r8, xmm0
			cvttsd2si		r10, xmm2
			cmp				r8, r10
			jnz				hline
			jz				point
			; ---------------------------
.abs:		; x2 - x1 -> xmm4 / abs(x2 - x1) -> xmm5 ; 6, 7 for y
			movsd			xmm4, xmm2
			subsd			xmm4, xmm0	; xmm4 = x2 - x1
			movsd			xmm6, xmm3
			subsd			xmm6, xmm1	; xmm6 = y2 - y1
			; hack abs
			xorpd			xmm8, xmm8
			cmppd			xmm8, xmm8, 0	; 1 in all
			psrlq			xmm8, 1			; 0 in 63
			;x
			movsd			xmm5, xmm4
			andpd			xmm5, xmm8
			;y
			movsd			xmm7, xmm6
			andpd			xmm7, xmm8


.cmpdxdy:
			ucomisd			xmm5, xmm7
			jbe				yaxis

xaxis:		; abs(x2 - x1) > abs(y2 - y1)
			ucomisd			xmm2, xmm0
			ja				.xline				; xmm2 > xmm0
.swapx:		; swap x1, x2 & y1, y2
			movsd			xmm8, xmm0
			movsd			xmm0, xmm2
			movsd			xmm2, xmm8
			movsd			xmm8, xmm1
			movsd			xmm1, xmm3
			movsd			xmm3, xmm8
.xline:		; y = a.x + b
			movsd			xmm8, xmm6			; xmm8 = y2 - y1
			divsd			xmm8, xmm4			; a = xmm8 = (y2 - y1) / (x2 - x1)
			movsd			xmm5, xmm8			; a
			mulsd			xmm5, xmm0			; a * x1
			movsd			xmm9, xmm1			; y1
			subsd			xmm9, xmm5			; b = y1 - a * x1
			; if (x1 < 0) { x1 = 0 ; y1 = b;} -> if (x1 < 1) x1 = 0.5
			ucomisd			xmm0, qword [ONE]
			jae				.tw2				; x1 >= 1
			movsd			xmm0, qword [HALF]	; x1 = 0.5
			movsd			xmm1, xmm0			; 0.5
			mulsd			xmm1, xmm8			; * a
			addsd			xmm1, xmm9			; + b (y1 = a*0.5 + b)

.tw2:		; if (x2 >= w) { x2 = w ; y2 = a * x2 + b;} -> if (x2 >= w1) x2 = w - 0.5
			movsd			xmm12, xmm10
			subsd			xmm12, qword [ONE]	; w1
			ucomisd			xmm2, xmm12
			jb				.twy1				; x2 < w1
			;
			movsd			xmm2, xmm10			; x2 = width - .5
			subsd			xmm2, qword [HALF]	; -.5
			movsd			xmm3, xmm2			; 0.5
			mulsd			xmm3, xmm8			; * a
			addsd			xmm3, xmm9			; + b (y1 = a*0.5 + b)

.twy1		; test yi


			; recalc xi
			; a > 0 ?
			ucomisd			xmm8, [ZERO]
			jb				.twan
			; a > 0
			; y1 >= h nopix
			ucomisd			xmm1, xmm11
			jae				NOPIX
			; y2 < 0 nopix
			ucomisd			xmm3, [ZERO]
			jb				NOPIX
			; pix
			; y1 < 0, recalc x1
			ucomisd			xmm1, [ZERO]
			jae				.twapx2
			xorpd			xmm1, xmm1			; y1 = 0
			xorpd			xmm0, xmm0
			subsd			xmm0, xmm9
			divsd			xmm0, xmm8			; x1 = -b / a

.twapx2		; a > 0, (x1, y1) inside
			; y2 < height ok
			ucomisd			xmm3, xmm11
			jb				.prepax
			; y2 >= height, recalc x2
			movsd			xmm3, xmm11			; y2 = h
			movsd			xmm2, xmm3
			subsd			xmm2, xmm9
			divsd			xmm2, xmm8			; x2 = (h-b)/a
			; falling below h
			ucomisd			xmm5, [ONE_]
			jb				.twap@@@
			subsd			xmm2, [ZERO_]
			movsd			xmm3, xmm2
			mulsd			xmm3, xmm8
			addsd			xmm3, xmm9
			jmp				.prepax

.twap@@@	; radical
			jmp 			point


.twan		; a < 0





.prepax:	;
			mov				r10, rsi			; color
			movzx			r11, word[rdi + 8]	; width = r11w
			mov				rdi, [rdi]			;
			;
			cvttsd2si		edx, xmm0			; @@@ ready in ri
			cvtsi2ss		xmm12, edx			; X0
			cvttsd2si		eax, xmm2
			sub				eax, edx
			mov				edx, eax			; dist @@@
			add				eax, 1				; nb xi
			cmp				eax, 4
			ja				.i4
			xor				eax, eax
.i4			shr				eax, 2
			add				eax, 1
			mov				ecx, eax			; cpt loop
			shl				eax, 2				; nb4 xi
			;
			cvtsi2ss		xmm13, edx			; dist
			cvtsi2ss		xmm14, eax			; nb xi
			divss			xmm13, xmm14		; PAS @@@

						movsd			qword [rdi + 32], xmm8
			movsd			qword [rdi + 40], xmm9

			; 4 * xi
			addss			xmm12, dword [HALFf]
			movss			dword [rbp - 16], xmm12
			addss			xmm12, xmm13
			movss			dword [rbp - 12], xmm12
			addss			xmm12, xmm13
			movss			dword [rbp - 8], xmm12
			addss			xmm12, xmm13
			movss			dword [rbp - 4], xmm12
			movdqa			xmm12, oword [rbp - 16]
			; 4 * 4pas
			mulss			xmm13, dword [FOURf]
			movss			dword [rbp - 16], xmm13
			movss			dword [rbp - 12], xmm13
			movss			dword [rbp - 8], xmm13
			movss			dword [rbp - 4], xmm13
			movdqa			xmm13, oword [rbp - 16]
			; 4 * a
			cvtsd2ss		xmm8, xmm8
			movss			dword [rbp - 16], xmm8
			movss			dword [rbp - 12], xmm8
			movss			dword [rbp - 8], xmm8
			movss			dword [rbp - 4], xmm8
			movdqa			xmm8, oword [rbp - 16]
			; 4 * b
			cvtsd2ss		xmm9, xmm9
			movss			dword [rbp - 16], xmm9
			movss			dword [rbp - 12], xmm9
			movss			dword [rbp - 8], xmm9
			movss			dword [rbp - 4], xmm9
			movdqa			xmm9, oword [rbp - 16]
			; 2 * w
;			xor				r8, r8
;			mov				dword [rbp - 12], dword r8d
;			mov				dword [rbp - 4], dword r8d
			mov				dword [rbp - 16], dword r11d
			mov				dword [rbp - 8], dword r11d
			movdqa			xmm10, oword [rbp - 16]

			xor				rdx, rdx
			movsd			qword [rdi], xmm0
			movsd			qword [rdi + 8], xmm1
			movsd			qword [rdi + 16], xmm2
			movsd			qword [rdi + 24], xmm3
	ret

.loopx:		; loop xi, yi
			movdqa			xmm14, xmm12		; xi
			movdqa			xmm15, xmm12		; yi = xi
			mulps			xmm15, xmm8			; a . xi
			addps			xmm15, xmm9			; + b
			; index
			cvttps2dq		xmm14, xmm14		; xif -> xi
			cvttps2dq		xmm15, xmm15		; yif -> yi
			; prep
			movdqa			oword [rbp - 32], xmm15	; yi, temp
			;
			pmuludq			xmm15, xmm10
			movdqa			oword [rbp - 16], xmm15	; w.yi
			;
			movdqu			xmm15, oword [rbp - 28] ; yi
			pmuludq			xmm15, xmm10
			movdqa			oword [rbp - 32], xmm15	; w.yi
			;
			mov				eax, dword [rbp - 32]
			mov				dword [rbp - 12], eax
			mov				eax, dword [rbp - 24]
			mov				dword [rbp - 4], eax
			;
			paddd			xmm14, oword [rbp - 16]
			movdqa			oword [rbp - 16], xmm14	; i
			;
			mov				eax, dword [rbp - 16]
			mov				dword [rdi + 4*rax], r10d
			mov				eax, dword [rbp - 12]
			mov				dword [rdi + 4*rax], r10d
			mov				eax, dword [rbp - 8]
			mov				dword [rdi + 4*rax], r10d
			mov				eax, dword [rbp - 4]
			mov				dword [rdi + 4*rax], r10d

.coopx:		;
			addps			xmm12, xmm13
			loop			.loopx

.donex		;
			ret


; -------------------------------------------------------------------
yaxis:
		ucomisd			xmm1, xmm3
		jbe				YLINE				; xmm3 >= xmm1
SWPY:	; swap x1, x2 & y1, y2
		movsd			xmm8, xmm0
		movsd			xmm0, xmm2
		movsd			xmm2, xmm8
		movsd			xmm8, xmm1
		movsd			xmm1, xmm3
		movsd			xmm3, xmm8
YLINE:
		movsd			xmm8, xmm4			; xmm8 = x2 - x1
		divsd			xmm8, xmm6			; a = xmm8 = (x2 - x1) / (y2 - y1)
		movsd			xmm5, xmm8			; a
		mulsd			xmm5, xmm1			; a * y1
		movsd			xmm9, xmm0			; x1
		subsd			xmm9, xmm5			; b = x1 - a * y1

		; @@@ case x1 ~= x2 / y1 ~= y2

WY1:	; if (y1 < 0) { y1 = 0 ; x1 = b;}
		ucomisd			xmm1, [ZERO]
		jae				WY2					; y1 >= 0
		xorpd			xmm1, xmm1			; y1 = 0
		movsd			xmm0, xmm9			; x1 = b

WY2:	; if (y2 >= img->height) { y2 = img->height ; x2 = a * y2 + b;}
		ucomisd			xmm3, xmm11
		jb				TY					; y2 < height

WY22:	movsd			xmm2, xmm10			; x2 = width
		mulsd			xmm2, xmm8			; a * width
		addsd			xmm2, xmm9			; + b
		movsd			xmm3, xmm11			; y2 =

TY:		movsd			xmm4, xmm1			; xmm4 = x = x1
		movsd			xmm6, qword [PAS]	; xmm6 = 0.65
		mov				r10, rsi			; color
		xor				r11, r11
		mov				r11w, [rdi + 8]		; width - r9w/d
		mov				rdi, [rdi]
LOOPY:	; loop
		; xi, yi
		movsd			xmm5, xmm4			; xmm5 = y = x
		mulsd			xmm5, xmm8			; x = a * y
		addsd			xmm5, xmm9			; x = x + b
TIY1:	; if (x < 0) continue;
		ucomisd			xmm5, [ZERO]
		jb				COOPY				; x < 0
TIY2:	; if (x >= w) continue;
		ucomisd			xmm5, xmm10
		jae				COOPY				; y >= w
		inc				rcx

		; index
		cvttsd2si		r8d, xmm4			; (int)y -> r8d
		imul			r8d, r11d			; width * (int)y
		cvttsd2si		r9d, xmm5			; (int)x -> r9d
		add				r8d, r9d			; index
		mov				[rdi + 4*r8], dword r10d	;

COOPY:	;
		addsd			xmm4, xmm6			; y += 0.65
		ucomisd			xmm4, xmm3
		jbe				LOOPY				; xmm3 >= xmm4

		; exit
		mov				rax, rcx
		ret


;;;;;;;;;;;
hline:
			xor			r12, r12
			mov			r12w, word [rdi + 8]
			;
			cmp			r10, r8
			jns			.hpos
			; hneg
			mov			rdx, r10
			mov			r10, r8
			mov			r8, rdx
			;
.hpos
			cmp			r8, 0
			js			.hpn		; x1 < 0
			jmp			.hpo

.hpn		; x1 < 0
			xor			r8, r8
.hpo		;
			cmp			r10, r12
			js			.hgo
			mov			r10, r12		; x2 >= w
			sub			r10, 1

.hgo		; r8 = min >= 0 && r10 = max < w
			mov			rdi, [rdi]
			xor			rdx, rdx
			mov			rax, r12		; width
			mul			r9				; * y1 = y2 E [0, h[
			add			rax, r8			; + x1
			shl			rax, 2			; * 4
			add			rdi, rax		; rdi = point 0
			mov			rcx, r10
			sub			rcx, r8
			add			rcx, 1			; rcx
			mov			rax, rcx		; return
			mov			rdx, rsi		; color
			;
.loop
			mov			dword [rdi], edx
			add			rdi, 4
			loop		.loop

			;
			ret

;;;;;;;;;;;
vline:
			xor			r12, r12
			mov			r12w, word [rdi + 10]	; height
			;
			cmp			r11, r9
			jns			.vpos
			; hneg
			mov			rdx, r11
			mov			r11, r9
			mov			r9, rdx
			;
.vpos
			cmp			r9, 0
			js			.vpn		; y1 < 0
			jmp			.vpo

.vpn		; y1 < 0
			xor			r9, r9
.vpo		;
			cmp			r11, r12
			js			.vgo
			mov			r11, r12		; y2 >= h
			sub			r11, 1

.vgo		; r9 = min >= 0 && r11 = max < h
			xor			rdx, rdx
			xor			r12, r12
			mov			r12w, word [rdi + 8]	; width
			mov			rax, r12		; width
			mul			r9				; * x1 = x2 E [0, w[
			add			rax, r8
			shl			rax, 2			; * 4
			mov			rdi, [rdi]
			add			rdi, rax		; rdi = point 0
			mov			rcx, r11
			sub			rcx, r9
			add			rcx, 1			; rcx
			mov			rax, rcx		; return
			mov			rdx, rsi		; color
			shl			r12, 2			; linesize
			;
.loop
			mov			dword [rdi], edx
			add			rdi, r12
			loop		.loop

			;
			ret


;;;;;;;;;;;
point:
			xor			rax, rax
			mov			ax, word [rdi + 8]	; width
			mul			r9				; * y1, x1 = x2 E [0, w[
			add			rax, r8			; + x1
			shl			rax, 2			; * 4
			mov			rdi, [rdi]
			add			rdi, rax		; rdi = point 0
			mov			rax, 1			; return
			mov			dword [rdi], rsi

			ret


;;;;;;;;;;;
NOPIX:	;
		mov				rax, 0
		ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;long asm_tst1650
asm_tst1650:
			mov			rcx, 201
			mov			r10, rsi
			mov			rdi, [rdi]
			;
			xorpd		xmm10, xmm10
			movsd		qword [rbp - 16], xmm10
			movsd		xmm11, [ONE]
			movsd		qword [rbp - 24], xmm11
.loop:		;
			ucomisd		xmm0, qword [rbp - 16]
			jb			.b
			mulsd		xmm0, qword [rbp - 24]

.b:
			;
			mov			dword [rdi + 4*rcx], r10d
			loop		.loop
			ret
