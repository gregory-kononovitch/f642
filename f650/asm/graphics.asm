


default rel

global a650_draw_line: function


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
P1:		; prepar : x = sx * (x - x0) + width / 2
		movsd			xmm4, [rdi + 16]	; x0 -> xmm4
		subsd			xmm0, xmm4			; x1 = x1 - x0
		subsd			xmm2, xmm4			; x2 = x2 - x0
		movsd			xmm5, [rdi + 32]	; sx -> xmm5
		mulsd			xmm0, xmm5			; x1 = sx * x1
		mulsd			xmm2, xmm5			; x2 = sx * x2
		xor				rax, rax
		mov				ax, [rdi + 8]		; ax = width
		cvtsi2sd		xmm4, eax			; width -> xmm4
		mov				eax, 2
		cvtsi2sd		xmm5, eax			; 2 -> xmm5
		divsd			xmm4, xmm5			; w2 -> xmm4
		addsd			xmm0, xmm4			; x1 = x1 + w2
		addsd			xmm2, xmm4			; x2 = x2 + w2
		; tests
		movsd			[rdi + 48], xmm0
		movsd			[rdi + 56], xmm1

		; tests x
T1:		; if (x1 < 0 && x2 < 0) return (opt : if (x1 >= 0 || x2 >= 0) ok)
		xorpd			xmm4, xmm4
		ucomisd			xmm4, xmm0
		seta			al
		test			al, al
		je				T2

T11:
		xorpd			xmm4, xmm4
		ucomisd			xmm4, xmm2
		seta			al
		test			al, al
		je				T2
		jmp				TESTKO

T2:		; if (x1 >= w && x2 >= w) return (opt : if (x1 < w || x2 < w) ok)
;		xor				rax, rax
;		mov				ax, [rdi + 8]		; ax = width
;		cvtsi2sd		xmm4, eax			; width -> xmm4
;		ucomisd			xmm0, xmm4
;		setae			al
;		test			al, al
;		je				T21
;		jmp				P2
;T21:
;		ucomisd			xmm2, xmm4
;		setae			al
;		test			al, al
;		je				TESTKO

P2:		; prepar : y = height / 2 - sy * (y - y0)
		movsd			xmm4, [rdi + 24]	; y0 -> xmm4
		subsd			xmm1, xmm4			; y1 = y1 - y0
		subsd			xmm3, xmm4			; y2 = y2 - y0
		movsd			xmm5, [rdi + 40]	; sy -> xmm5
		mulsd			xmm1, xmm5			; y1 = sy * y1
		mulsd			xmm3, xmm5			; y2 = sy * y2
		xor				rax, rax
		mov				ax, [rdi + 10]		; ax = height
		cvtsi2sd		xmm4, eax			; height -> xmm4
		mov				eax, 2
		cvtsi2sd		xmm5, eax			; 2 -> xmm5
		divsd			xmm4, xmm5			; h2 -> xmm4
		movsd			xmm5, xmm4			; h2 -> xmm5
		subsd			xmm4, xmm1			; xmm4 = h2 - y1
		movsd			xmm1, xmm4			;
		subsd			xmm5, xmm3			; xmm5 = h2 - y2
		movsd			xmm3, xmm5
		; tests
		movsd			[rdi + 64], xmm2
		movsd			[rdi + 72], xmm3

ABSX:	; x2 - x1 -> xmm4 / abs(x2 - x1) -> xmm6
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

ABSY:
		movsd			xmm6, xmm3
		subsd			xmm6, xmm1	; xmm4 = x2 - x1
		xorpd			xmm7, xmm7
		ucomisd			xmm7, xmm6
		seta			al
		test			al, al
		je				YPOS
		xorpd			xmm7, xmm5
		subsd			xmm7, xmm6
		jmp				CMPdXdY

YPOS:	; xmm4 = x2 - x1 > 0
		movsd			xmm7, xmm6

CMPdXdY:
		ucomisd			xmm7, xmm5
		seta			al
		test			al, al
		je				XAXIS
		jmp				YAXIS

XAXIS:	; abs(x2 - x1) > abs(y2 - y1)
		movsd			[rdi + 80], xmm8
		movsd			[rdi + 88], xmm9
		ret

YAXIS:
		movsd			[rdi + 80], xmm8
		movsd			[rdi + 88], xmm9
        ret

RETOK:
		xor				rdi, rdi
		cvtsi2sd		xmm0, rdi			; temp for tests
		ret

TESTKO:	;
		mov				rdi, -1
		cvtsi2sd		xmm0, rdi			; temp for tests
		ret

;a650_draw_line  ENDP
