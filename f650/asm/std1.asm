;
; file    : std1.asm
; project : f640
;
; Created on: Aug 30, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;

default rel

;
global memseta650:		function
global memset2a650:		function


SECTION .data

SECTION .text  align=16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; memseta650(void *align_x16, uint32_t fill, uin64_t size)
memseta650:
		mov			rcx, rdx
		shr			rcx, 6
		jrcxz		.end
		mov			rax, rsi
		mov			dword [rbp - 4], eax
		mov			dword [rbp - 8], eax
		mov			dword [rbp - 12], eax
		mov			dword [rbp - 16], eax
		movdqa		xmm0, oword [rbp - 16]
		mov			rax, rdi

.loop:
		movntdq		oword [rax], xmm0
		add			rax, 16
		loop		.loop
.end:
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; memseta650(void *align_x16, uint32_t fill, uin64_t size)
memset2a650:
		mov			eax, esi
		mov			rcx, rdx
		rep stosq
		ret
