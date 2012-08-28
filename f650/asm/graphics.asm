


default rel

global a650_draw_line: function


SECTION .text  align=16

; a650_draw_line(image *img, double x1, double y1, double x2, double y2)
a650_draw_line:
; x2 - x1 -> xmm4
		movsd			xmm4, xmm2
		subsd			xmm4, xmm0
; y2 - y1 -> xmm5
		movsd			xmm5, xmm3
		subsd			xmm5, xmm1
		ucomissd		xmm0, xmm2
		seta			al
		test			al, al
		je





        ret
;a650_draw_line  ENDP
