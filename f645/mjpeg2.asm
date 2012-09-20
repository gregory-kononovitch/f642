;
; file    : mjpeg2.asm
; project : f645 (t508.f640/2)
;
; Created on: Sep 19, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;

default rel

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


global 	decode645:			function

global 	hu2fman645:			function


SECTION .data		ALIGN=16
%include "inclu/mjpeg_tables.s"

ALIGN 16



SECTION .text		ALIGN=16

%macro feed32 1
	;
	; feed@@@
	;
	test		off, 32
	jnz			%1
	;
	movbe		eax, dword [data]
	add			data, 4
	mov			cl, 32
	sub			cl, off
	shl			rax, cl
	or			bits, rax
	add			off, 32
	jmp			%1
%endmacro

%macro	logbits	2
;	mov			r15, %1
;	mov			r13, bits
;.lt1%2
;	shl			r13, 1
;	jc			.lt11%2
;	mov			byte [rsi], 0
;	jmp			.ct1%2
;	;
;.lt11%2
;	mov			byte [rsi], 1
;.ct1%2
;	add			rsi, 1
;	sub			r15, 1
;	jnz			.lt1%2
%endmacro


;;;;;;;;;;;;;;;
; STACK
%define		VAR			0x0100
%define		TREEL		0x17D0
%define		TREEC		0x2EA0
%define		QUANY		0x2FA0
%define		QUANC		0x30A0

%define		STACK		0X4000

; Local variables (RBP - VAR + )
%define		ptreel		0x00
%define		PTREEC		0x08
%define		PQUANL		0x10
%define		PQUANC		0x18
;
%define		pdata		0x20
%define		pdest		0x28

;
%define		rowdu		0x30
%define		coldu		0x34

;
%define		_ri0_us		0x70
%define		_ri_us		0x72
%define		_y0_uc		0x74
%define		_yi_uc		0x75
%define		_u0_uc		0x76
%define		_ui_uc		0x77
%define		_v0_uc		0x78
%define		_vi_uc		0x79

%define		PTEST		0x80




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern long decode645(void *sos, void *dest, int size)
decode645:
%define		symb	cl
%define		off		r8b
%define		bits	r9
%define		data	r10
%define		tree	r11
%define		dest	rsi
;
%define		iz		r12b
;
			;
			begin	STACK
			;
				mov			qword [rbp - VAR + pdata], rdi
				mov			qword [rbp - VAR + pdest], rsi
				mov			dword [rbp - VAR + PTEST], edx
				;
				; AC Lumin tree
				;
				mov			rax, rbp
				sub			rax, TREEL
				mov			qword [rbp - VAR + ptreel], rax
				mov			ecx, dword [NHACL]
				mov			rdx, HACL
				;
.itbl:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; stack tree addr in rax   +   src tree in rdx  +   size in rcx   +   [ return in r9 ]
;;;; r8 used
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

				%include "inclu/huftbl-1.s"
				;
				; Vars
				mov			word [rbp - VAR + _ri0_us], 10
				mov			byte [rbp - VAR + _y0_uc], 2
				mov			byte [rbp - VAR + _u0_uc], 1
				mov			byte [rbp - VAR + _v0_uc], 1
				;
.ifeed			;
				; FEED
				;
				mov			data, rdi
				movbe		bits, qword [data]
				add			data, 8
				mov			off, 64
				logbits 64,a0 ; ###
				;
				mov			rsi, rsi
				xor			rcx, rcx				; @@@ mag shift
				;
				;
				mov			dx, word [rbp - VAR + _ri0_us]
				mov			word [rbp - VAR + _ri_us], dx
.loopri			;
				;
				mov			dl, byte [rbp - VAR + _y0_uc]
				mov			byte [rbp - VAR + _yi_uc], dl
				mov			dl, byte [rbp - VAR + _u0_uc]
				mov			byte [rbp - VAR + _ui_uc], dl
				mov			dl, byte [rbp - VAR + _v0_uc]
				mov			byte [rbp - VAR + _vi_uc], dl

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; rcx (symb) < 256     r8 (off) >= 9     .herr
;;;;      -> value in dl
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
hdcl:			;
.loophdcl
				mov			iz, 1
				logbits 64,a1; ###

%include "inclu/hufdcl-1.s"

				logbits 64,a2; ###

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
				;
				;
.herr			;
				mov			byte [rsi], 254
				mov			rax, data
				return

.donehdcl		;
				; @@@ value
				shl			bits, cl
				sub			off, cl					;
				;

				; svg
				mov			byte [rsi], symb
				add			rsi, 1

				;
				; feed@@@
				;
				feed32		.donedcl

				;
.donedcl:		;
				;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; tree in [rbp - ]		off >= 1         .herr
;;;;	-> value in byte[tree] == [r11]
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
hacl:
.loop
				logbits 64,a3; ###

%include "inclu/hufacl-1.s"

				logbits 64,a4; ###

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.herr:
				mov			byte [rsi], 255
				mov			rax, data
				return

.donehacl
				mov			symb, byte [tree]		; symb

				;
				; EOB
				test		symb, 0xFF
				jz			.eobl					; EOB

				;
				; @@@ iz + value
				; cp
				mov			edx, ecx				;

				; iz
				shr			dl, 4
				add			iz, dl

				; ### svg
				add			rsi, rdx				; nz
				mov			byte [rsi], symb
				add			rsi, 1

				; @@@ value
				and			cl, 0x0F
				shl			bits, cl
				sub			off, cl

				; iz
				add			iz, 1

				;
				; feed@@@
				;
				feed32		.doneacl

				;
.doneacl:		;
				; iz < 63
				test		iz, 64
				jnz			.donel
				jmp			hacl.loop

.eobl			;
				feed32		.donel
				;
.donel:			;
				sub			byte [rbp - VAR + _yi_uc], 1
				jnz			hdcl.loophdcl




.done			;
				mov			byte [rsi], 255			; ###
				mov			rax, data
				return




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern long hu2fman645(void *dseg, int size, void *dest);
hu2fman645:
%define		symb	dl
%define		off		r8d
%define		bits	r9
%define		data	r10
%define		eof		r11
				;
				begin	64
				;
				mov			data, rdi
				mov			eof, rdi
				add			eof, rsi
				mov			dword [rbp - 8], esi
				movbe		bits, qword [data]
				mov			off, 64
				add			data, 8
				;
				xor			rcx, rcx
				mov			rsi, rdx
				xor			rdx, rdx
				xor			rdi, rdi
				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; rdx (symb) < 256     r8 (off) >= 9     .herr
;;;;      -> value in dl
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

hdcl2:			;

%include "inclu/hufdcl-1.s"


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
				;
				;
.herr			;
				mov			rax, data
				return

.donehdcl		;
				; svg
				mov			byte [rsi], dl
				add			rsi, 1
				sub			dword [rbp - 8], 1
				jz			.end
;				return
				;
				;
				test		off, 32
				jnz			hdcl
				;
				movbe		eax, dword [data]
				add			data, 4
				mov			ecx, 32
				sub			ecx, off
				shl			rax, cl
				or			bits, rax
				add			off, 32
				jmp			hdcl


.end:			;
				;mov			rax, data
				mov		rax, rdx
				return

