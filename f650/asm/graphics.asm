


default rel

global draw_linea650: 	function
global draw_zlinea650: 	function

SECTION .data
ZERO	dq				0.0
HALF	dq				0.5
PAS		dq				0.65

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
draw_linea650:
		xor				rcx, rcx
PX:		; prepar : x = sx * (x - x0) + width / 2
		movsd			xmm4, [rdi + 16]	; x0 -> xmm4
		subsd			xmm0, xmm4			; x1 = x1 - x0
		subsd			xmm2, xmm4			; x2 = x2 - x0
		movsd			xmm5, qword [rdi + 32]	; sx -> xmm5
		mulsd			xmm0, xmm5			; x1 = sx * x1
		mulsd			xmm2, xmm5			; x2 = sx * x2
		xor				rax, rax
		mov				ax, [rdi + 8]		; ax = width
		cvtsi2sd		xmm10, eax			; width -> xmm10
		movsd			xmm4, xmm10			; width -> xmm4
		movsd			xmm5, qword [HALF]	; .5 -> xmm5
		mulsd			xmm4, xmm5			; w2 -> xmm4
		addsd			xmm0, xmm4			; x1 = x1 + w2
		addsd			xmm2, xmm4			; x2 = x2 + w2

		; tests x
TX1:	; if (x1 < 0 && x2 < 0) return (opt : if (x1 >= 0 || x2 >= 0) ok)
		ucomisd			xmm0, [ZERO]
		jae				TX2					; xmm0 >= 0

TX11:	;
		ucomisd			xmm2, [ZERO]
		jb				NOPIX				; xmm2 < 0

TX2:	; if (x1 >= w && x2 >= w) return (opt : if (x1 < w || x2 < w) ok)
		ucomisd			xmm0, xmm10
		jb				PY
TX21:
		ucomisd			xmm0, xmm10
		jae				NOPIX

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
		movsd			xmm5, qword [HALF]
		mulsd			xmm4, xmm5			; h2 -> xmm4
		movsd			xmm5, xmm4			; h2 -> xmm5
		subsd			xmm4, xmm1			; xmm4 = h2 - y1
		movsd			xmm1, xmm4			;
		subsd			xmm5, xmm3			; xmm5 = h2 - y2
		movsd			xmm3, xmm5

		; tests x
TY1:	; if (x1 < 0 && x2 < 0) return (opt : if (x1 >= 0 || x2 >= 0) ok)
		ucomisd			xmm1, [ZERO]
		jae				TY2					; xmm1 >= 0
TY11:
		ucomisd			xmm3, [ZERO]
		jb				NOPIX				; xmm3 < 0

TY2:	; if (x1 >= w && x2 >= w) return (opt : if (x1 < w || x2 < w) ok)
		ucomisd			xmm1, xmm11
		jb				ABSX				; y1 < h
TY21:
		ucomisd			xmm3, xmm11
		jae				NOPIX

ABSX:	; x2 - x1 -> xmm4 / abs(x2 - x1) -> xmm5
		movsd			xmm4, xmm2
		subsd			xmm4, xmm0	; xmm4 = x2 - x1
		ucomisd			xmm2, xmm0
		jae				XPOS
		xorpd			xmm5, xmm5
		subsd			xmm5, xmm4
		jmp				ABSY

XPOS:	; xmm4 = x2 - x1 = xmm5 > 0
		movsd			xmm5, xmm4

ABSY:	; y2 - y1 -> xmm6 / abs(y2 - y1) -> xmm7
		movsd			xmm6, xmm3
		subsd			xmm6, xmm1	; xmm4 = x2 - x1
		ucomisd			xmm3, xmm1
		jae				YPOS
		xorpd			xmm7, xmm7
		subsd			xmm7, xmm6
		jmp				CMPdXdY

YPOS:	; xmm6 = x2 - x1 > 0
		movsd			xmm7, xmm6

CMPdXdY:
		ucomisd			xmm7, xmm5
		jae				YAXIS

XAXIS:	; abs(x2 - x1) > abs(y2 - y1)
		ucomisd			xmm2, xmm0
		jae				XLINE				; xmm2 >= xmm0
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
		movsd			xmm5, xmm8			; a
		mulsd			xmm5, xmm0			; a * x1
		movsd			xmm9, xmm1			; y1
		subsd			xmm9, xmm5			; b = y1 - a * x1
WX1:	; if (x1 < 0) { x1 = 0 ; y1 = b;}
		ucomisd			xmm0, [ZERO]
		jae				WX2					; x1 >= 0
		xorpd			xmm0, xmm0			; x1 = 0
		movsd			xmm1, xmm9			; y1 = b

WX2:	; if (x2 >= w) { x2 = w ; y2 = a * x2 + b;}
		ucomisd			xmm2, xmm10
		jb				TX					; x2 < width
WX22:	movsd			xmm2, xmm10			; x2 = width
		movsd			xmm3, xmm10			; y2 =
		mulsd			xmm3, xmm8			; a * width
		addsd			xmm3, xmm9			; + b

TX:		movsd			xmm4, xmm0			; xmm4 = x = x1
		movsd			xmm6, qword [PAS]	; xmm6 = 0.65
		mov				r10, rsi			; color
		xor				r11, r11
		mov				r11w, [rdi + 8]		; width - r9w/d

LOOPX:	; loop
		; xi, yi
		movsd			xmm5, xmm4			; xmm5 = y = x
		mulsd			xmm5, xmm8			; y = a * x
		addsd			xmm5, xmm9			; y = y + b
TIX1:	; if (y < 0) continue;
		ucomisd			xmm5, [ZERO]
		jb				COOPX				; y < 0
TIX2:	; if (y >= h) continue;
		ucomisd			xmm5, xmm11
		jae				COOPX				; y >= h
		inc				rcx

		; index
		cvttsd2si		r8d, xmm5			; (int)y -> r8d
		imul			r8d, r11d			; width * (int)y
		cvttsd2si		r9d, xmm4			; (int)x -> r9d
		add				r8d, r9d			; index
		shl				r8, 2				; index << 2
		mov				rax, qword [rdi]	;
		add				rax, r8				;
		mov				[rax], dword r10d	;

COOPX:	;
		addsd			xmm4, xmm6			; x += 0.65
		ucomisd			xmm4, xmm2
		jbe				LOOPX				; xmm2 >= xmm4
		jmp				RETOK

YAXIS:
		ucomisd			xmm3, xmm1
		jae				YLINE				; xmm3 >= xmm1
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
		movsd			xmm1, xmm4			; y1 = 0
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
		xorpd			xmm12, xmm12		; xmm12 = 0
LOOPY:	; loop
		; xi, yi
		movsd			xmm5, xmm4			; xmm5 = y = x
		mulsd			xmm5, xmm8			; x = a * y
		addsd			xmm5, xmm9			; x = x + b
TIY1:	; if (x < 0) continue;
		ucomisd			xmm5, [ZERO]
		jbe				COOPY				; x < 0
TIY2:	; if (x >= w) continue;
		ucomisd			xmm5, xmm10
		jae				COOPY				; y >= w
		inc				rcx

		; index
		cvttsd2si		r8d, xmm4			; (int)y -> r8d
		imul			r8d, r11d			; width * (int)y
		cvttsd2si		r9d, xmm5			; (int)x -> r9d
		add				r8d, r9d			; index
		shl				r8, 2				; index << 2
		mov				rax, qword [rdi]	;
		add				rax, r8				;
		mov				[rax], dword r10d	;

COOPY:	;
		addsd			xmm4, xmm6			; y += 0.65
		ucomisd			xmm4, xmm3
		jbe				LOOPY				; xmm3 >= xmm4
		jmp				RETOK


RETOK:
		mov				rax, rcx
		ret

NOPIX:	;
		mov				rax, 0
		ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; @@@ z1 & z2 mess all indices / draw_linea650
; double draw_zlinea650(image *img, double x1, double y1, double z1, double x2, double y2, double z2, uint32_t color)
draw_zlinea650:
		jmp				draw_linea650
		ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
