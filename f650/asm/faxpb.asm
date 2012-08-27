; First try to trace a line in a int[] ~ rgb image
;
;
;

default rel

global faxpb: function


SECTION .text  align=16

; axpb(double x, double a, double b)
faxpb:
		mulsd		xmm0, xmm1
		addsd		xmm0, xmm2
        ret
;faxpb  ENDP
