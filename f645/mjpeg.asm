;
; file    : mjpeg.asm
; project : f645 (t508.f640/2)
;
; Created on: Sep 18, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;

default rel

;%include "include1.txt"

%macro  begin 1
	push    rbp
	mov     rbp, rsp
	sub     rsp, %1
%endmacro

%macro  return 0
	mov     rsp, rbp
    pop     rbp
    ret
%endmacro


global 	huffman645:			function

SECTION .data		ALIGN=16
; DCLumin
HUFDCL			db		16,3,6,16,6,9,16,9,12,0,255,255,16,9,12,16,12,15,16,15,18,1,255,255,2,255,255,3,255,255,4,255,255,5,255,255,16,3,6,6,255,255,16,3,6,7,255,255,16,3,6,8,255,255,16,3,6,9,255,255,16,3,6,10,255,255,16,3,255,11,255,255
ALIGN 16

SECTION .text		ALIGN=16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern long huffman645(void *dseg, void *dest);
huffman645:
				;
				mov			rdx, HUFDCL
				xor			rax, rax
				xor			rcx, rcx
				mov			rax, 7
				;
				;
.loop:			bt			[rdi], rax
				jnc			.case0
				;
.case1:
				mov			cl, byte [rdx + 2]
				add			rdx, rcx
				test		byte [rdx], 16
				jnz			.coot
				; ok
				xor			rax, rax
				mov			al, byte [rdx]
				ret

.case0:			;
				mov			cl, byte [rdx + 1]
				add			rdx, rcx
				test		byte [rdx], 16
				jnz			.coot
				; ok
				xor			rax, rax
				mov			al, byte [rdx]
				ret

.coot:			;
				sub			rax, 1
				jnz			.loop
				;
				mov			rax, 7
				add			rdi, 1
				jmp			.loop

