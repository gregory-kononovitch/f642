


default rel

global a650_draw_line: function


SECTION .text  align=16

; double a650_draw_line(image *img, double x1, double y1, double x2, double y2)
; x1 -> xmm0
a650_draw_line:
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

XPOS:	; xmm4 = x2 - x1 > 0
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

YPOS:	; xmm4 = x2 - x1 > 0 | xmm4 = x1 - x2 > 0
		movsd			xmm7, xmm6

CMPdXdY:
		ucomisd			xmm7, xmm5
		seta			al
		test			al, al
		je				XAXIS
		jmp				YAXIS

XAXIS:	; abs(x2 - x1) > abs(y2 - y1)
		movsd			xmm0, xmm4
		ret

YAXIS:
		movsd			xmm0, xmm6
        ret
;a650_draw_line  ENDP
