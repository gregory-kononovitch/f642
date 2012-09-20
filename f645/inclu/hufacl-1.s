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
;;;; tree in [rbp - ]		off >= 1         .herr
;;;;	-> value in byte[tree] == [r11]
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;
%define		symb	dl
%define		off		r8d
%define		bits	r9
%define		data	r10
%define		tree	r11
;;;;;;;
				;
				;
.reinit			mov			tree, qword [rbp - 8]
				;
.loop:			;
				shl 		bits, 1
				jnc			.case0
				;
.case1:
				mov			tree, [tree + 10]
				test		tree, 0xffffffff
				jz			.herr
				test		word [tree], 256
				jz			.done
				; ko
				sub			off, 1
				jnz			.loop
				movbe		bits, qword [data]
				add			data, 8
				mov			off, 64
				jmp			.loop

.case0:			;
				mov			tree, [tree + 2]
				test		tree, 0xffffffff
				jz			.herr
				test		word [tree], 256
				jz			.done
				; ko
				sub			off, 1
				jnz			.loop
				movbe		bits, qword [data]
				add			data, 8
				mov			off, 64
				jmp			.loop
				;
				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

