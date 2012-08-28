


default rel

global a650_draw_line: function

SECTION .data
HALF	dq				0.5
PAS		dq				0.65

SECTION .text  align=16

; double a650_draw_line(image *img, double x1, double y1, double x2, double y2)
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
a650_draw_line:
PX:		; prepar : x = sx * (x - x0) + width / 2
		movsd			xmm4, [rdi + 16]	; x0 -> xmm4
		subsd			xmm0, xmm4			; x1 = x1 - x0
		subsd			xmm2, xmm4			; x2 = x2 - x0
		movsd			xmm5, [rdi + 32]	; sx -> xmm5
		mulsd			xmm0, xmm5			; x1 = sx * x1
		mulsd			xmm2, xmm5			; x2 = sx * x2
		xor				rax, rax
		mov				ax, [rdi + 8]		; ax = width
		cvtsi2sd		xmm10, eax			; width -> xmm10
		movsd			xmm4, xmm10			; width -> xmm4
		movsd			xmm5, QWORD [HALF]	; .5 -> xmm5
		mulsd			xmm4, QWORD [HALF]			; w2 -> xmm4
		addsd			xmm0, xmm4			; x1 = x1 + w2
		addsd			xmm2, xmm4			; x2 = x2 + w2
		; tests
		movsd			[rdi + 48], xmm0
		movsd			[rdi + 64], xmm2

		; tests x
TX1:	; if (x1 < 0 && x2 < 0) return (opt : if (x1 >= 0 || x2 >= 0) ok)
		xorpd			xmm4, xmm4
		ucomisd			xmm4, xmm0
		seta			al
		test			al, al
		je				TX2					; xmm0 >= 0

TX11:
		ucomisd			xmm4, xmm2
		seta			al
		test			al, al
		je				TX2					; xmm2 >= 0
		jmp				TESTKO				; xmm2 < 0

TX2:		; if (x1 >= w && x2 >= w) return (opt : if (x1 < w || x2 < w) ok)
		movsd			xmm4, xmm10			; width -> xmm4
		ucomisd			xmm4, xmm0
		seta			al
		test			al, al
		je				TX21				; xmm0 >= width
		jmp				PY
TX21:
		ucomisd			xmm4, xmm2
		seta			al
		test			al, al
		je				TESTKO

PY:		; prepar : y = height / 2 - sy * (y - y0)
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
		mov				eax, 2
		cvtsi2sd		xmm5, eax			; 2 -> xmm5
		divsd			xmm4, xmm5			; h2 -> xmm4
		movsd			xmm5, xmm4			; h2 -> xmm5
		subsd			xmm4, xmm1			; xmm4 = h2 - y1
		movsd			xmm1, xmm4			;
		subsd			xmm5, xmm3			; xmm5 = h2 - y2
		movsd			xmm3, xmm5
		; tests
		movsd			[rdi + 56], xmm1
		movsd			[rdi + 72], xmm3

		; tests x
TY1:	; if (x1 < 0 && x2 < 0) return (opt : if (x1 >= 0 || x2 >= 0) ok)
		xorpd			xmm4, xmm4
		ucomisd			xmm4, xmm1
		seta			al
		test			al, al
		je				TY2					; xmm1 >= 0

TY11:
		ucomisd			xmm4, xmm3
		seta			al
		test			al, al
		je				TY2					; xmm3 >= 0
		jmp				TESTKO				; xmm3 < 0

TY2:	; if (x1 >= w && x2 >= w) return (opt : if (x1 < w || x2 < w) ok)
		movsd			xmm4, xmm11			; height -> xmm4
		ucomisd			xmm4, xmm1
		seta			al
		test			al, al
		je				TY21				; xmm1 >= width
		jmp				ABSX
TY21:
		ucomisd			xmm4, xmm3
		seta			al
		test			al, al
		je				TESTKO

ABSX:	; x2 - x1 -> xmm4 / abs(x2 - x1) -> xmm5
		movsd			xmm4, xmm2
		subsd			xmm4, xmm0	; xmm4 = x2 - x1
		xorpd			xmm5, xmm5
		ucomisd			xmm5, xmm4
		seta			al
		test			al, al
		je				XPOS
		xorpd			xmm5, xmm5
		subsd			xmm5, xmm4
		jmp				ABSY

XPOS:	; xmm4 = x2 - x1 = xmm5 > 0
		movsd			xmm5, xmm4

ABSY:	; y2 - y1 -> xmm6 / abs(y2 - y1) -> xmm7
		movsd			xmm6, xmm3
		subsd			xmm6, xmm1	; xmm4 = x2 - x1
		xorpd			xmm7, xmm7
		ucomisd			xmm7, xmm6
		seta			al
		test			al, al
		je				YPOS
		xorpd			xmm7, xmm7
		subsd			xmm7, xmm6
		jmp				CMPdXdY

YPOS:	; xmm6 = x2 - x1 > 0
		movsd			xmm7, xmm6

CMPdXdY:
		ucomisd			xmm7, xmm5
		seta			al
		test			al, al
		je				XAXIS
		jmp				YAXIS

XAXIS:	; abs(x2 - x1) > abs(y2 - y1)
		ucomisd			xmm0, xmm2
		seta			al
		test			al, al
		je				XLINE				; xmm2 >= xmm0
SWPX:	; swap x1, x2 & y1, y2
		movsd			xmm8, xmm0
		movsd			xmm0, xmm2
		movsd			xmm2, xmm8
		movsd			xmm8, xmm1
		movsd			xmm1, xmm3
		movsd			xmm3, xmm8
XLINE:
		movsd			xmm8, xmm6			; xmm8 = y2 - y1
		divsd			xmm8, xmm4			; a = xmm8 = (y2 - y1) / (x2 - x1)
		movsd			[rdi + 80], xmm8	; store a for tests
		movsd			xmm5, xmm8			; a
		mulsd			xmm5, xmm0			; a * x1
		movsd			xmm9, xmm1			; y1
		subsd			xmm9, xmm5			; b = y1 - a * x1
		movsd			[rdi + 88], xmm9
WX1:	; if (x1 < 0) { x1 = 0 ; y1 = b;}
		xorpd			xmm4, xmm4
		ucomisd			xmm4, xmm0
		seta			al
		test			al, al
		je				WX2					; x1 >= 0
		movsd			xmm0, xmm4			; x1 = 0
		movsd			xmm1, xmm9			; y1 = b

WX2:	; if (x2 >= w) { x2 = w ; y2 = a * x2 + b;}
		movsd			xmm4, xmm10
		ucomisd			xmm4, xmm2
		seta			al
		test			al, al
		je				WX22				; x2 >= width
		jmp				TX
WX22:	movsd			xmm2, xmm10			; x2 = width
		mulsd			xmm4, xmm8			; a * width
		addsd			xmm4, xmm9			; + b
		movsd			xmm3, xmm4			; y2 =

TX:		movsd			xmm4, xmm0			; xmm4 = x = x1
		movsd			xmm6, QWORD [PAS]	; xmm6 = 0.65
LOOPX:	; loop
		; xi, yi
		movsd			xmm5, xmm4			; xmm5 = y = x
		mulsd			xmm5, xmm8			; y = a * x
		addsd			xmm5, xmm9			; y = y + b
		; if (y < 0 || y >= h) continue;

		; index
		cvttsd2si		r8d, xmm5			; (int)y -> r8d
		xor				r9, r9
		mov				r9w, [rdi + 8]		; width - r9w/d
		imul			r8d, r9d			; width * (int)y
		xor				r9, r9
		cvttsd2si		r9d, xmm4			; (int)x -> r9d
		add				r8d, r9d			; index
		mov				[rdi + 96], r8d		; @@@ test
		mov				rax, [rdi]			;
		shl				r8, 2				;
		add				rax, r8				;
		mov				r9d, [rdi + 100]
		mov				[rax], r9d			;
;		mov				[rax], DWORD [rdi + 100]

COOPX:	;
		addsd			xmm4, xmm6			; x += 0.65
		ucomisd			xmm4, xmm2
		seta			al
		test			al, al
		je				LOOPX				; xmm2 >= xmm4
		jmp				RETOK

YAXIS:
		ucomisd			xmm1, xmm3
		seta			al
		test			al, al
		je				YLINE				; xmm3 >= xmm1
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
		movsd			[rdi + 80], xmm8	; store a for tests
		movsd			xmm5, xmm8			; a
		mulsd			xmm5, xmm1			; a * y1
		movsd			xmm9, xmm0			; x1
		subsd			xmm9, xmm5			; b = x1 - a * y1
		movsd			[rdi + 88], xmm9
		jmp				RETOK

RETOK:
		xor				rdi, rdi
		cvtsi2sd		xmm0, rdi			; temp for tests
		ret

TESTKO:	;
		mov				rdi, -1
		cvtsi2sd		xmm0, rdi			; temp for tests
		ret

;a650_draw_line  ENDP
