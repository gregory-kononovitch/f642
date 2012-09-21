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
BITS 	64

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
global 	scan645:			function
global 	scan645o:			function


SECTION .data		ALIGN=16
FF16			dd	0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF

%include "inclu/mjpeg_tables.s"

ALIGN 16
F128		db		128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128
FLUSH		db		15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

SECTION .bss		ALIGN=16
_A0			resb	1
_A1			resb	1
_A2			resb	1
_A3			resb	1
_A4			resb	1
_A5			resb	1
_A6			resb	1
_A7			resb	1
_A8			resb	1
_A9			resb	1
_AA			resb	1
_AB			resb	1
_AC			resb	1
_AD			resb	1
_AE			resb	1
_AF			resb	1

_B0			resb	1
_B1			resb	1
_B2			resb	1
_B3			resb	1
_B4			resb	1
_B5			resb	1
_B6			resb	1
_B7			resb	1
_B8			resb	1
_B9			resb	1
_BA			resb	1
_BB			resb	1
_BC			resb	1
_BD			resb	1
_BE			resb	1
_BF			resb	1

_C0			resb	1
_C1			resb	1
_C2			resb	1
_C3			resb	1
_C4			resb	1
_C5			resb	1
_C6			resb	1
_C7			resb	1
_C8			resb	1
_C9			resb	1
_CA			resb	1
_CB			resb	1
_CC			resb	1
_CD			resb	1
_CE			resb	1
_CF			resb	1

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


%macro feed 3
	;
	;
	test		off, 32
	jnz			%1
	;
	movdqa		xmm2, qword [FF16]
	movdqu		xmm1, oword [data]
	pxor		xmm2, xmm1
	movdqa		oword [rbp - VAR + ppxor], xmm2

	mov			dl, byte [data]
	xor			dl, 0xFF
	jz			.pb1%2
	mov			dl, byte [data+1]
	xor			dl, 0xFF
	jz			.pb2%2
	mov			dl, byte [data+2]
	xor			dl, 0xFF
	jz			.pb3%2
	mov			dl, byte [data+3]
	xor			dl, 0xFF
	jz			.pb4%2

	movbe		eax, dword [data]
	add			data, 4
	mov			cl, 32
	sub			cl, off
	shl			rax, cl
	or			bits, rax
	add			off, 32
	jmp			%1
	;
	;
.pb1%2:
	mov			dl, byte [data+1]
	xor			dl, 0
	jz			%3
	add			data,






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
%define		ptreec		0x08
%define		PQUANL		0x10
%define		PQUANC		0x18
;
%define		pdata		0x20
%define		pdest		0x28

;
%define		rowdu		0x30
%define		coldu		0x34

;
%define		ppxor		0x50
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
				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; stack tree addr in rax   +   src tree in rdx  +   size in rcx   +   [ return in r9 ]
;;;; r8 used
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ltree:
				%include "inclu/huftbl-1.s"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
				;
				; AC Chrom tree
				;
				mov			rax, rbp
				sub			rax, TREEC
				mov			qword [rbp - VAR + ptreec], rax
				mov			ecx, dword [NHACC]
				mov			rdx, HACC
				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; stack tree addr in rax   +   src tree in rdx  +   size in rcx   +   [ return in r9 ]
;;;; r8 used
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ctree:
				%include "inclu/huftbl-1.s"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

				;
				; Vars
				mov			word [rbp - VAR + _ri0_us], 10
				mov			byte [rbp - VAR + _y0_uc], 2
				mov			byte [rbp - VAR + _u0_uc], 2
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
decmain:
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
				;
				mov			tree, qword [rbp - VAR + ptreel]
				;
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
				and			symb, 0x0F
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

; ---------------------------------  Luminance -----------------------------
; --------------------------------------------------------------------------


; --------------------------------------------------------------------------
; --------------------------------- Chrominance ----------------------------


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; rcx (symb) < 256     r8 (off) >= 9     .herr
;;;;      -> value in dl
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
hdcc:			;
.loophdcc
				mov			iz, 1
				logbits 64,a1; ###

%include "inclu/hufdcc-1.s"

				logbits 64,a2; ###

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
				;
				;
.herr			;
				mov			byte [rsi], 254
				mov			rax, data
				return

.donehdcc		;
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
				feed32		.donedcc

				;
.donedcc:		;
				;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; tree in [rbp - ]		off >= 1         .herr
;;;;	-> value in byte[tree] == [r11]
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
hacc:
.loop
				logbits 64,a3; ###
				;
				mov			tree, qword [rbp - VAR + ptreec]
				;
%include "inclu/hufacc-1.s"

				logbits 64,a4; ###

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.herr:
				mov			byte [rsi], 255
				mov			rax, data
				return

.donehacc
				mov			symb, byte [tree]		; symb

				;
				; EOB
				test		symb, 0xFF
				jz			.eobc					; EOB

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
				and			symb, 0x0F
				shl			bits, cl
				sub			off, cl

				; iz
				add			iz, 1

				;
				; feed@@@
				;
				feed32		.doneacc

				;
.doneacc:		;
				; iz < 63
				test		iz, 64
				jnz			.donec
				jmp			hacc.loop

.eobc			;
				feed32		.donec
				;
.donec:			;
				sub			byte [rbp - VAR + _ui_uc], 1
				jnz			hdcc.loophdcc

				; END MCU
; -----------------------------------------------------------------------------
				; RESTART

.donemcu:		;
				sub			byte [rbp - VAR + _ri_us], 1
				jnz			decmain.loopri














.done			;
				mov			byte [rsi], 255			; ###
				mov			qword [rsi + 1], bits
				mov			rax, data
				return


















;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; scan645(void *ptr, int size, int[] res)
%define		data	r8

scan645:
				begin 32
				mov			r8, rdi
				xor			r9, r9
				xor			r10, r10
				xor			r11, r11
				xor			rax, rax

.mloop:
				mov			al, 4
.loop:
				;
				mov			r12b, byte [data]
				cmp			r12b, 0xff
				jz			.ff
				add			data, 1
				sub			al, 1
				mov			byte [_B0 + rax], r12b
				jmp			.coot

.ff:
				add			r9, 1
				cmp			byte [data + 1], 0
				jnz			.marker
				add			r10, 1
				add			data, 2
				sub			rsi, 1
				sub			al, 1
				mov			byte [_B0 + rax], 0xFF
				jmp			.coot


.marker
				add			r11, 1
				add			data, 2
				sub			rsi, 1

.coot:
				cmp			al, 0
				jnz			.loop
				mov			ecx, dword [_B0]

.mcoot			sub			rsi, 4
				jge			.mloop

.end:
				mov			dword [rdx], r9d
				mov			dword [rdx+4], r10d
				mov			dword [rdx+8], r11d
				return



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; scan645(void *ptr, int size, int[] res)
%define		data	r8

scan645o:
				begin 32
				mov			r8, rdi
				xor			r9, r9
				xor			r10, r10
				xor			r11, r11

.loop:
				;
				movdqa		xmm2, oword [FF16]
				movdqu		xmm1, oword [data]
				pxor		xmm2, xmm1
				movdqa		oword [_A0], xmm1
				movdqa		oword [_B0], xmm2
				movdqa		xmm2, oword [F128]
				movdqa		oword [_C0], xmm2
				;
				mov			al, 16

.t0				;
				sub			al, 1
				mov			byte [_C0], al
				test		byte [_B0], 0xff
				jnz			.t1
				add			r9, 1
				test		byte [_A1], 0xff
				jnz			.marker
				add			r10, 1
				sub			al, 2
				mov			byte [_C1], al
				jmp			.t2

.t1				;
				sub			al, 1
				mov			byte [_C1], al
				test		byte [_B1], 0xff
				jnz			.t2
				add			r9, 1
				test		byte [_A2], 0xff
				jnz			.marker
				add			r10, 1
				sub			al, 2
				mov			byte [_C2], al
				jmp			.t3

.t2				;
				sub			al, 1
				mov			byte [_C2], al
				test		byte [_B2], 0xff
				jnz			.t3
				add			r9, 1
				test		byte [_A3], 0xff
				jnz			.marker
				add			r10, 1
				sub			al, 2
				mov			byte [_C3], al
				jmp			.t4

.t3				;
				sub			al, 1
				mov			byte [_C3], al
				test		byte [_B3], 0xff
				jnz			.t4
				add			r9, 1
				test		byte [_A4], 0xff
				jnz			.marker
				add			r10, 1
				sub			al, 2
				mov			byte [_C4], al
				jmp			.t5

.t4				;
				sub			al, 1
				mov			byte [_C4], al
				test		byte [_B4], 0xff
				jnz			.t5
				add			r9, 1
				test		byte [_A5], 0xff
				jnz			.marker
				add			r10, 1
				sub			al, 2
				mov			byte [_C5], al
				jmp			.t6

.t5				;
				sub			al, 1
				mov			byte [_C5], al
				test		byte [_B5], 0xff
				jnz			.t6
				add			r9, 1
				test		byte [_A6], 0xff
				jnz			.marker
				add			r10, 1
				sub			al, 2
				mov			byte [_C6], al
				jmp			.t7

.t6				;
				sub			al, 1
				mov			byte [_C6], al
				test		byte [_B6], 0xff
				jnz			.t7
				add			r9, 1
				test		byte [_A7], 0xff
				jnz			.marker
				add			r10, 1
				sub			al, 2
				mov			byte [_C7], al
				jmp			.t8

.t7				;
				sub			al, 1
				mov			byte [_C7], al
				test		byte [_B7], 0xff
				jnz			.t8
				add			r9, 1
				test		byte [_A8], 0xff
				jnz			.marker
				add			r10, 1
				sub			al, 2
				mov			byte [_C8], al
				jmp			.t9


.t8

.t9

				mov			eax, dword [rbp - 0x20]

.coot:
				add			data, 8
				sub			rsi, 8
				jge			.loop


.end:
				mov			dword [rdx], r9d
				mov			dword [rdx+4], r10d
				mov			dword [rdx+8], r11d
				return

.marker:
				add			r11, 1
				add			data, 8
				sub			rsi, 8
				jge			.loop
				jmp			.end


