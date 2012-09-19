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

SECTION .data		ALIGN=16
; DCLumin
HUFDCL		db		16,3,6,16,6,9,16,9,12,0,255,255,16,9,12,16,12,15,16,15,18,1,255,255,2,255,255,3,255,255,4,255,255,5,255,255,16,3,6,6,255,255,16,3,6,7,255,255,16,3,6,8,255,255,16,3,6,9,255,255,16,3,6,10,255,255,16,3,255,11,255,255

ALIGN 16

HDCL3		dw		256,1,2,256,2,3,256,3,4,0,256,256,256,3,4,256,4,5,256,5,6,1,256,256,2,256,256,3,256,256,4,256,256,5,256,256,256,1,2,6,256,256,256,1,2,7,256,256,256,1,2,8,256,256,256,1,2,9,256,256,256,1,2,10,256,256,256,1,256,11,256,256
ALIGN 16
NHDCL		dd		24

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
; build hdcl at rax, return to r9
_build_hdcl_:
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
huf3man645:
				begin 432
				;
				xor			r11, r11
				;
				mov			rax, rbp
				sub			rax, 432
				mov			r10, rax
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
				;
.reinit			mov			rdx, r10
				;
.loop:			;
;				bt			r9, rax
				shl			r9, 1
				jnc			.case0
				;
.case1:
				mov			rdx, [rdx + 10]
				test		rdx, 0xffffffffffffffff
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
				test		rdx, 0xffffffffffffffff
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
				mov			r11b, byte [rsi]
				cmp			rcx, r11
				jnz			.err

;				mov			byte [rsi], cl
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




