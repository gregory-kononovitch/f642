default rel

;
global memseta650:		function


SECTION .data

SECTION .text  align=16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; memseta650(void *align, uint32_t fill, uin64_t size)
memseta650:
		mov			rcx, rdx
		shr			rcx, 4
		jrcxz		END0
		mov			rax, rdi
		xorpd		xmm0, xmm0
LOOP0:
		movaps		[rax], xmm0
;		add			rax, 16
		loop		LOOP0
END0:
		ret
