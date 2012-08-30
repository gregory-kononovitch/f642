default rel

;
global memseta650:		function


SECTION .data

SECTION .text  align=16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; memseta650(void *align_x16, uint32_t fill, uin64_t size)
memseta650:
		mov			rcx, rdx
		shr			rcx, 4
		jrcxz		END0
		mov			rax, rsi
		mov			[rbp - 4], eax
		mov			[rbp - 8], eax
		mov			[rbp - 12], eax
		mov			[rbp - 16], eax
		movdqa		xmm0, oword [rbp - 16]
		mov			rax, rdi

LOOP0:
		movntdq		[rax], xmm0
		add			rax, 16
		loop		LOOP0
END0:
		ret
