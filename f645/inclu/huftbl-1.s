;
; file    : hufacl-1.s
; project : f645 (t508.f640/2)
;
; Created on: Sep 20, 2012 (fork of mpeg.asm)
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; stack tree addr in rax   +   src tree in rdx  +   size in rcx   +   [ return in r9 ]
;;;; r8 used
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
				;
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
;				jmp			r9
