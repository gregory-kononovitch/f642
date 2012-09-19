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
global 	huf2man645:			function
global 	huf3man645:			function
global 	huf4man645:			function


SECTION .data		ALIGN=16
%include "mjpeg_tables.ash"

ALIGN 16



SECTION .text		ALIGN=16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern long huffman645(void *dseg, void *dest);
huffman645:



.end:			ret




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern long huf2man645(void *dseg, void *dest);
%define		tree	rdx
%define		data	rdi
%define		bit		rax
%define		loff	r8
huf2man645:
				;
				xor			rax, rax
				xor			rcx, rcx
				xor			r8, r8
				mov			r8, 50000
				;
				movbe		r9, qword [rdi]
				mov			rax, 63
				;
.reinit			mov			rdx, HUFDCL
				;
.loop:			bt			r9, rax
				jnc			.case0
				;
.case1:
				mov			cl, byte [rdx + 2]
				add			rdx, rcx
				test		byte [rdx], 16
				jz			.done
				; ko
				sub			rax, 1
				jns			.loop
				add			rdi, 8
				movbe		r9, qword [rdi]
				mov			rax, 63
				jmp			.loop

.case0:			;
				mov			cl, byte [rdx + 1]
				add			rdx, rcx
				test		byte [rdx], 16
				jz			.done
				; ko
				sub			rax, 1
				jns			.loop
				add			rdi, 8
				movbe		r9, qword [rdi]
				mov			rax, 63
				jmp			.loop


.done			; svg
				mov			cl, byte [rdx]
				mov			byte [rsi], cl
				add			rsi, 1
				;
				; cpt svg
				sub			r8, 1
				jz			.end
				;
				; rax
				sub			rax, 1
				jns			.reinit
				add			rdi, 8
				movbe		r9, qword [rdi]
				mov			rax, 63
				jmp			.reinit



.end:			;
				mov			rax, rdi
				ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; build hdcl 2-8-8 at rax, return to r9
_build_hdcl_:
				;
				xor			r8, r8
				;
.loop			;
				xor			r8, r8
				mov			r8w, word [rdx]
				mov			word [rax], r8w
.ptr0			;
				xor			r8, r8
				mov			r8w, word [rdx + 2]
				bt			r8, 8
				jc			.no0
				imul		r8, 18
				add			r8, rax
				mov			qword [rax + 2], r8
				jmp			.ptr1
				;
.no0			;
				mov			qword [rax + 2], 0
				;
.ptr1			;
				xor			r8, r8
				mov			r8w, word [rdx + 4]
				bt			r8, 8
				jc			.no1
				imul		r8, 18
				add			r8, rax
				mov			qword [rax + 10], r8
				jmp			.coop
				;
.no1			;
				mov			qword [rax + 10], 0
				;
.coop			;
				add			rax, 18
				add			rdx, 6
				;
				sub			ecx, 1
				jnz			.loop

				;
				jmp			r9


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; build hdcl 2-1-1 at rax, return to r9
_build_hdcl2_:
				;
				mov			ecx, dword [NHDCL]
				mov			rdx, HDCL3
				xor			r8, r8
				;
.loop			;
				xor			r8, r8
				mov			r8w, word [rdx]
				mov			word [rax], r8w
.ptr0			;
				xor			r8, r8
				mov			r8w, word [rdx + 2]
				bt			r8, 8
				jc			.no0
				imul		r8, 18
				add			r8, rax
				mov			qword [rax + 2], r8
				jmp			.ptr1
				;
.no0			;
				mov			qword [rax + 2], 0
				;
.ptr1			;
				xor			r8, r8
				mov			r8w, word [rdx + 4]
				bt			r8, 8
				jc			.no1
				imul		r8, 18
				add			r8, rax
				mov			qword [rax + 10], r8
				jmp			.coop
				;
.no1			;
				mov			qword [rax + 10], 0
				;
.coop			;
				add			rax, 18
				add			rdx, 6
				;
				sub			ecx, 1
				jnz			.loop

				;
				jmp			r9




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern long huf3man645(void *dseg, void *dest);
; OK, but too much stack for tables
huf3man645:
				begin 432
				;
				xor			r11, r11
				;
				mov			rax, rbp
				sub			rax, 432
				mov			r10, rax
				mov			ecx, dword [NHDCL]
				mov			rdx, HDCL3
				mov			r9, .frombuild
				;
				jmp			_build_hdcl_
				;
.frombuild		;
				;
				xor			rax, rax
				xor			rcx, rcx
				xor			r8, r8
				mov			r8, 50000
				;
				movbe		r9, qword [rdi]
				mov			rax, 63
;				add			rdi, 8	@@@@@@@@@@@@@@@@@@@@@@@@@@
				;
.reinit			mov			rdx, r10
				;
.loop:			;
				shl 		r9, 1
				jnc			.case0
				;
.case1:
				mov			rdx, [rdx + 10]
				test		rdx, 0xffffffff
				jz			.err
				test		word [rdx], 256
				jz			.done
				; ko
				sub			eax, 1
				jns			.loop
				add			rdi, 8
				movbe		r9, qword [rdi]
				mov			eax, 63
				jmp			.loop

.case0:			;
				mov			rdx, [rdx + 2]
				test		rdx, 0xffffffff
				jz			.err
				test		word [rdx], 256
				jz			.done
				; ko
				sub			eax, 1
				jns			.loop
				add			rdi, 8
				movbe		r9, qword [rdi]
				mov			eax, 63
				jmp			.loop


.done			; svg
				mov			cl, byte [rdx]
;				mov			r11b, byte [rsi]
;				cmp			rcx, r11
;				jnz			.err

				mov			byte [rsi], cl
				add			rsi, 1
				;
				; cpt svg
				sub			r8, 1
				jmp			.end
				jz			.end
				;
				; rax
				sub			rax, 1
				jns			.reinit
				add			rdi, 8
				movbe		r9, qword [rdi]
				mov			rax, 63
				jmp			.reinit

.err			;
				mov			rax, rdi
				return

.end:			;
				mov			rax, rdi
				return



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern long huf4man645(void *dseg, void *dest);
; OK, but too much stack for tables
huf4man645:
				begin 5832

				;
				;
				mov			rax, rbp
				sub			rax, 5832
				mov			r10, rax
				mov			ecx, dword [NHACL]
				mov			rdx, HACL2
				mov			r9, .frombuild
				;
				jmp			_build_hdcl_
				;
.frombuild		;
				;
				xor			rax, rax
				xor			rcx, rcx
				xor			r8, r8
				mov			r8, 1
				;
				movbe		r9, qword [rdi]
				mov			rax, 63
				;
.reinit			mov			rdx, r10
				;
.loop:			;
				shl 		r9, 1
				jnc			.case0
				;
.case1:
				mov			rdx, [rdx + 10]
				test		rdx, 0xffffffff
				jz			.err
				test		word [rdx], 256
				jz			.done
				; ko
				sub			eax, 1
				jns			.loop
				add			rdi, 8
				movbe		r9, qword [rdi]
				mov			eax, 63
				jmp			.loop

.case0:			;
				mov			rdx, [rdx + 2]
				test		rdx, 0xffffffff
				jz			.err
				test		word [rdx], 256
				jz			.done
				; ko
				sub			eax, 1
				jns			.loop
				add			rdi, 8
				movbe		r9, qword [rdi]
				mov			eax, 63
				jmp			.loop


.done			; svg
				mov			cl, byte [rdx]
				mov			byte [rsi], cl
				add			rsi, 1
				;
				; cpt svg
				sub			r8, 1
				jz			.end
				;
				; rax
				sub			rax, 1
				jns			.reinit
				add			rdi, 8
				movbe		r9, qword [rdi]
				mov			rax, 63
				jmp			.reinit

.err			;
				mov			rax, rdi
				return

.end:			;
				mov			rax, rdi
				return



