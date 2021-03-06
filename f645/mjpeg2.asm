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
	push	rbx
	push	r12
	push	r13
	push	r14
;	push	r15
	mov     rbp, rsp
	sub     rsp, %1
%endmacro

%macro  return 0
	mov     rsp, rbp
;	pop 	r15
	pop 	r14
	pop 	r13
	pop 	r12
	pop		rbx
    pop     rbp
    ret
%endmacro


global 	decode645:			function
global 	scan645:			function
global 	scan645o:			function
global 	test_acl645:		function

SECTION .data		ALIGN=16
FF16			dd	0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF

%include "inclu/mjpeg_tables.s"

ALIGN 16
SHUFFZIF	db		0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3
SHUFFPIX	db		0,0,0,0,4,4,4,4,8,8,8,8,12,12,12,12
SHUFFBeW	db		0,0,0,0,4,4,4,4,8,8,8,8,12,12,12,12
F128		db		128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128
FLUSH		db		15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0

PDECAL		dd		0.0, 0.0, 0.0, 0.0
    		dd		1.0, 0.0, 0.0, 0.0
    		dd		1.0, 1.0, 0.0, 0.0
    		dd		1.0, 1.0, 1.0, 0.0

PF8			dd		8.0, 8.0, 8.0, 8.0
PF128		dd		128.0, 128.0, 128.0, 128.0


SECTION .text		ALIGN=16

; @@@@ jump where?
; 1: return  ;  2:suffix  ;  3: jmp RDi  ; 4: jmp EOI ; 5: jmp ERR
%macro feed 5
			;
			; @@@@
			;
			test		off, 32
			jnz			%1
			bt			flags, 63	; flag stop after a RSTi or EOI (+ flag eoi 62)
			jc			%1
			;						; @@@@@@@@@@@@@@@@@@@@@@@@@@
;			xor		rax, rax		; for now no move at RST (cd .ff)
;			xor		rdx, rdx
			mov		cl, 4
.loop%2		; loop 4 bytes
			mov		dl, byte [data]
			cmp		dl, 0xff
			jz		.ff%2
			; 1 ok
.put%2:		shl		eax, 8
			or		eax, edx
			add		data, 1
			sub		cl, 1
			jnz		.loop%2
			; 4 bytes done
.cp%2		mov		cl, 32
			sub		cl, off
			shl		rax, cl
			or		bits, rax
			add		off, 32
			xor		rax, rax
			xor		rdx, rdx
			jmp		%1

.ff%2:		;
;			add 	data, 1
			mov 	dl, byte [data + 1]
			cmp		dl, 0x00
			jnz		.marker%2
			; FF00
			add 	data, 1				; discard 00
			mov		dl, 0xff
			jmp		.put%2
			;

.marker%2:	;
			cmp		dl, 0xd8
			jge		.mrk2%2				; > ffd7
			cmp		dl, 0xcf
			jle		.mrk1%2				; < ffd0
			; RSTi   [ffD0 - ffd7]
.sh%2		shl		eax, 8				; 0
			sub		cl, 1
			jnz		.sh%2
			bts		flags, 63			; rst stop flag set
			add		data, 2				; rst discard
			jmp		.cp%2
			;

.mrk2%2:	; 				>= 0xFFD8
			je		.soi%2			; ffd8 : SOI*
			cmp		dl, 0xda		;
			jl		.eoi%2			; ffd9 : EOI*
			je		.err%2			; ffda : SOS*
			cmp		dl, 0xdc
			jl		.err%2			; ffdb : DQT*
			je		.err%2			; ffdc : DNL
			cmp		dl, 0xde
			jl		.err%2			; ffdd : DRI*
			je		.err%2			; ffde : DHP
			cmp		dl, 0xe0
			jl		.err%2			; ffdf : EXP
			je		.err%2			; ffe0 : APP0*
			cmp		dl, 0xf0
			jl		.err%2			; ffei : APPi
			je		.err%2			; fff0 : JPG0
			cmp		dl, 0xfe
			jl		.err%2			; fffi : JPGn
			je		.err%2			; fffe : COM
			;
			jmp		%5				; error

.eoi%2:		; EOI
			add		data, 2
			bts		flags, 63			; stop flag set
			bts		flags, 62			; eoi flag set
			jmp		%1

.soi%2:		; SOI
			jmp		%5

.mrk1%2		; < ffd0				; 0xffc[0-15]    SOF0: ffc0, DHT: ffc4, DAC: ffcc, JPEG: ffc8
			cmp		dl, 0xC0
			je		.err%2			; ffC0 : SOF0*
			jmp		%5

.err%2:		; ERR
			jmp		%5

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
%define		VAR			0x100	;0x0200

%define		ZZI			0x200	;0x0200
%define		IZI			0x300	;0x0200
%define		ZZIF		0x400	;0x0200
%define		UVCOSCOSI	0x500	;0x0200
%define		PIXFI		0x600	;0x0200
%define		PIXII		0x700	;0x0200

%define		QUANTIL		0x800	;0x0200
%define		QUANTIC		0x900	;0x0200

;%define		WORK		4608	;
%define		WORK		0x1000	;

%define		TREEL		10448
%define		TREEC		16288

;%define		STACK		0X4000
%define		STACK		WORK

; Local variables (RBP - VAR + )
%define		ptreel		0x00
%define		ptreec		0x08
%define		PQUANL		0x10
%define		PQUANC		0x18
;
%define		pimg		0x20		; struct
%define		pdata		0x28		; data
%define		pimgptr		0x30		; ptr
%define		pdest		0x38		; logs, tmp
;
%define		ppix		0x40		; full decoded img
%define		width		0x48		; width
%define		height		0x4C		; height

;
%define		dcLumin0	0x50
%define		dcChrom0	0x54
%define		x8x8i		0x58		; x coord of decoded 8x8
%define		y8x8i		0x5C		; y coord of decoded 8x8

%define		y8offset	0x60		; long 4 * (width - 8)
%define		y8offset3	0x64		; long 4 * (width - 8)

%define		dcLumin1	0x68
%define		dcChrom1	0x6C

;
%define		_ri0_us		0x70
%define		_ri_us		0x72
%define		_y0_uc		0x74
%define		_yi_uc		0x75
%define		_u0_uc		0x76
%define		_ui_uc		0x77
%define		_v0_uc		0x78
%define		_vi_uc		0x79

%define		nblocks		0x80

;
%define		_vc1		0x90
%define		_vc2		0x91
%define		_vc3		0x92
%define		_vc4		0x93
%define		_vs1		0x94
%define		_vs2		0x96
%define		_vi1		0x98
%define		_vi2		0x9C

%define		PTEST		0x100




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern long decode645(void *img, void *dest, int size)
decode645:
%define		symb	cl
%define		off		r8b
%define		flags	r8
%define		bits	r9
%define		data	r10
%define		tree	r11
%define		dest	rsi
;
%define		iz		r12b
%define		ii		r13
;
			;
			begin	STACK
			;
				mov			qword [rbp - VAR + pimg], rdi
				mov			rax, qword [rdi]							; @@@ img / img.data / img.ptr
				mov			qword [rbp - VAR + pdata], rax
				mov			rax, qword [rdi + 32]
				mov			qword [rbp - VAR + pimgptr], rax
				mov			qword [rbp - VAR + pdest], rsi
				; decode
				mov			rax, qword [rdi + 64]					; decoded pixels
				mov			qword [rbp - VAR + ppix], rax
				mov			eax, dword [rdi + 12]					; width
				mov			dword [rbp - VAR + width], eax
				sub			rax, 8
				shl			rax, 2
				mov			qword [rbp - VAR + y8offset], rax
				mov			qword [rbp - VAR + y8offset3], rax
				mov			eax, dword [rdi + 16]					; height
				mov			dword [rbp - VAR + height], eax
				mov			eax, dword [rdi + 72]					; Ri
				mov			word [rbp - VAR + _ri0_us], ax
				mov			dword [rbp - VAR + nblocks], strict dword 0
				;
;				; idct-2
;				mov			strict dword [rbp - VAR + x8x8i], strict dword 0		; xi8x8
;				mov			strict dword [rbp - VAR + y8x8i], strict dword 0		; yi8x8
				; idct-3
				mov			strict dword [rbp - VAR + x8x8i], strict dword 4		; xi8x8
				mov			strict dword [rbp - VAR + y8x8i], strict dword 7		; yi8x8
				;
				mov			byte [rbp - VAR + _y0_uc], 2
				mov			byte [rbp - VAR + _u0_uc], 2
				mov			byte [rbp - VAR + _v0_uc], 1
				;
				mov			dword [rbp - VAR + PTEST], edx
				;
				;    Quantization coefs
				xor			rcx, rcx
				mov			rax, [rdi + 48]
.lpquantl
				mov			edx, dword [rax + 4 * rcx]
				mov 		dword [rbp - WORK + QUANTIL + 4 * rcx], edx
				add			rcx, 1
				cmp			rcx, 64
				jl			.lpquantl
				;
				xor			rcx, rcx
				mov			rax, [rdi + 56]
.lpquantc
				mov			edx, dword [rax + 4 * rcx]
				mov 		dword [rbp - WORK + QUANTIC + 4 * rcx], edx
				add			rcx, 1
				cmp			rcx, 64
				jl			.lpquantc

;				;
;				; AC Lumin tree
;				;
;				mov			rax, rbp
;				sub			rax, TREEL
;				mov			qword [rbp - VAR + ptreel], rax
;				mov			ecx, dword [NHACL]
;				mov			rdx, HACL
;				;
;				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; stack tree addr in rax   +   src tree in rdx  +   size in rcx   +   [ return in r9 ]
;;;; r8 used
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ltree:
;				%include "inclu/huftbl-1.s"
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;				;
;				; AC Chrom tree
;				;
;				mov			rax, rbp
;				sub			rax, TREEC
;				mov			qword [rbp - VAR + ptreec], rax
;				mov			ecx, dword [NHACC]
;				mov			rdx, HACC
;				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;; stack tree addr in rax   +   src tree in rdx  +   size in rcx   +   [ return in r9 ]
;;;; r8 used
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;ctree:
;				%include "inclu/huftbl-1.s"
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;						Workspace initialization
;              ----------------------------------------

				mov			cx, 32
				xor			rdx, rdx
.loop1



;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

				;
				;
				;
				mov			data, qword [rbp - VAR + pimgptr]
				mov			rsi, rsi
				xor			rdx, rdx				; @@@
				xor			rcx, rcx				; @@@ mag shift
				;
; -------------------------------------------------------------------------------------
;
;
; -------------------------------------------------------------------------------------
				;
decmain:
				;

.loopsample:
				; Ri
				mov			dx, word [rbp - VAR + _ri0_us]
				mov			word [rbp - VAR + _ri_us], dx
				;
				mov			strict dword [rsp - VAR + dcLumin0], strict dword 0
				mov			strict dword [rsp - VAR + dcChrom0], strict dword 0
				mov			strict dword [rsp - VAR + dcLumin1], strict dword 0
				mov			strict dword [rsp - VAR + dcChrom1], strict dword 0
				;
				; initial FEED64
				;
				btr			flags, 63
				movbe		bits, qword [data]
				add			data, 8
				mov			off, 64

				logbits 64,a0 ; ###

.loopri			;
				;
				mov			dl, byte [rbp - VAR + _y0_uc]
				mov			byte [rbp - VAR + _yi_uc], dl
				mov			dl, byte [rbp - VAR + _u0_uc]
				mov			byte [rbp - VAR + _ui_uc], dl
				mov			dl, byte [rbp - VAR + _v0_uc]
				mov			byte [rbp - VAR + _vi_uc], dl

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;						Huffman DCLumin
;	 rcx (symb) < 256     r8 (off) >= 9     .herr
;	      -> value in dl
;           -------------------------------------

hdcl:			;
.loophdcl
				mov			r12, strict qword 1			; iz
				mov			r13, strict qword 1			; ii
				%include "inclu/hufdcl-1.s"

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
				;
				;
.herr			; svg
				mov			dword [rsi], -9999
				mov			dword [rsi+4], 355304
				mov			dword [rsi+8], -10000
				mov			rax, data
				return

.donehdcl		;
				; svg
;				mov			byte [rsi], symb
;				add			rsi, 1

				xor			r15, r15
				test		symb, 0x0F				; @@@ ?
				jz			.val
				; @@@ value
				and			symb, 0x0F				; @@@
				mov			edx, ecx				; @@@ neg
				mov			r15, bits
				shl			bits, cl
				sub			off, cl					;
				;
				sub			cl, 64
				neg			cl
				bt			r15, 63
				jc			.pos
				; neg
				shr			r15, cl
				mov			cl, dl
				mov			r14d, 0xFFFFFFFF		; @@ neg ?
				shl			r14d, cl				;
				or			r15d, r14d
				add			r15d, 1
				jmp			.val
.pos:			; pos
				shr			r15, cl
.val:			;
				test		byte [rsp - VAR + _yi_uc], 0x01
				jnz			.lu1
				add			r15d, dword [rsp - VAR + dcLumin0]			; diff
				mov			dword [rsp - VAR + dcLumin0], r15d			; save
				jmp			.oth
.lu1
				add			r15d, dword [rsp - VAR + dcLumin1]			; diff
				mov			dword [rsp - VAR + dcLumin1], r15d			; save

				;
.oth			imul		r15d, dword [rbp - WORK + QUANTIL]
				mov			dword [rbp - WORK + ZZI], r15d
				mov			strict dword [rbp - WORK + IZI], strict dword 0

				;
				mov			ii, 1

				;
				; feed @@@
				;
.feedtmp		feed .donedcl, dcl, .donedcl, .donedcl, .herr	; 1: return  ;  2:suffix  ;  3: jmp RDi  ; 4: jmp EOI ; 5: jmp ERR
				;
.donedcl:		;
				;


;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;					Huffman ACLumin
;	 tree in [rbp - ]		off >= 1         .herr
;		-> value in byte[tree] == [r11]
;           ----------------------------------
hacl:
.loop
				logbits 64,a3; ###
				;
				mov			tree, qword [rbp - VAR + ptreel]
				;
				;%include "inclu/hufacl-1.s"
				%include "inclu/hufacl-3.s"

				logbits 64,a4; ###

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.herr:			; svg
				mov			dword [rsi], -9999
				mov			dword [rsi+4], 355305
				mov			dword [rsi+8], -10000
				mov			rax, data
				return

.donehacl
				;mov			symb, byte [tree]		; symb

				;
				; EOB
				test		symb, 0xFF
				jz			.eobl					; EOB

				;
				; @@@ iz + value
				; cp
				mov			edx, ecx				; duplic symbol with 0..

				; iz
				shr			dl, 4
				add			iz, dl					; @@@ case F0
				cmp 		symb, 0xF0
				jne			.value
				add			iz, 1
				jmp			.novalue

				; value
.value			and			symb, 0x0F
				mov			dl, symb				; @@@ neg
;				test		symb, 0xFF
;				jz			.novalue
				; @@@ value
				mov			r15, bits
				shl			bits, cl				; feed
				sub			off, cl					;
				;
				sub			cl, 64
				neg			cl
				bt			r15, 63
				jc			.pos
				; neg @@
				shr			r15, cl
				mov			r14d, 0xFFFFFFFF
				mov			cl, dl
				shl			r14d, cl
				or			r15d, r14d
				add			r15d, 1
				jmp			.val
.pos:			; pos
				shr			r15, cl
.val:			; prep
				;
				imul		r15d, dword [rbp - WORK + QUANTIL + 4 * r12]
				mov			dword [rbp - WORK + ZZI + 4 * ii], r15d
				mov			dword [rbp - WORK + IZI + 4 * ii], r12d
				;
				add			ii, 1
				;
.novalue:

				; iz
				add			iz, 1			; @@@ check ssss=0 rules

				;
				; feed @@@
				; err, a part,
				feed .doneacl, acl, .doneacl, .doneacl, .herr

.doneacl:
				;
				; iz < 64
				cmp 		iz, 64
				jge			.donel
				jmp			hacl.loop

.eobl			; svg

				; @@@ feed here?
				feed .donel, eobacl, .donel, .donel, .herr
				;
.donel:			;
				; svg
;				mov			byte [rsi], 0
;				mov			byte [rsi+1], r13b
;				mov			byte [rsi+2], 255
;				add			rsi, 3

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
							  DUMP
;                 ---------------------------
;;%define	LOGZI 1
%ifdef	LOGZI
%define logs rsi
;				mov			logs, qword [rbp - VAR + pdest]
				xor			r15, r15
				xor			r14, r14
;.razrsi0:			; 			RAZ RSI
;				mov         strict qword [rcx + 8 * r14], strict 0
;				add			r14, 1
;				cmp			r14, 256
;				jl			.razrsi0
;				xor			r14, r14

				;;;;;;

				mov			dword [logs], r13d		; ii
				add			logs, 4

.llppoo0:
				mov			r15d, dword [rbp - WORK + ZZI + 4 * r14]
				mov			dword [logs], r15d
				add			logs, 4

				add			r14, 1
				cmp 		r14, r13
				jl			.llppoo0

				mov			dword [logs], -9999		;
				add			logs, 4

%endif
;
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

;
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;                            DCT Lumin
;                    -------------------------
idctl:
					%include "inclu/idft2pix-3.s"

					add dword [rbp - VAR + nblocks], 1
;return
;
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;
				sub			byte [rbp - VAR + _yi_uc], 1
				jnz			hdcl.loophdcl

; ---------------------------------  Luminance -----------------------------
; --------------------------------------------------------------------------


; --------------------------------------------------------------------------
; --------------------------------- Chrominance ----------------------------


;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;					    Huffman DC Chrom
;	 rcx (symb) < 256     r8 (off) >= 9     .herr
;	      -> value in dl
;          ----------------------------------------

hdcc:
.loophdcc
				;
				mov			iz, 1			; iz
				mov			ii, 1			; ii

				%include "inclu/hufdcc-1.s"

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
				;
				;
.herr			; svg
				mov			dword [rsi], -9999
				mov			dword [rsi+4], 355306
				mov			dword [rsi+8], -10000
				mov			rax, data
				return

.donehdcc		;

				and			symb, 0x0F
				mov			r15, bits
				shl			bits, cl
				sub			off, cl					;

				;
				; feed@@@
				;
				feed .donedcc, dcc, .donedcc, .donedcc, .donedcc

				;
.donedcc:		;
				;



;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;                     Huffman AC Chrom
;	 tree in [rbp - ]		off >= 1         .herr
;		-> value in byte[tree] == [r11]
;           -----------------------------------

hacc:
.loop
				logbits 64,a3; ###
				;
				mov			tree, qword [rbp - VAR + ptreec]
				;
				;%include "inclu/hufacc-1.s"
				%include "inclu/hufacc-3.s"

				logbits 64,a4; ###

;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

.herr:			; svg
				mov			dword [rsi], -9999
				mov			dword [rsi+4], 355307
				mov			dword [rsi+8], -10000
				mov			rax, data
				return

.donehacc
				;mov			symb, byte [tree]		; symb

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

				; feed
				and			symb, 0x0F
				mov			r15, bits
				shl			bits, cl			; feed
				sub			off, cl				;

				; iz
				add			iz, 1

				;
				; feed@@@
				;
				feed .doneacc, acc, .doneacc, .doneacc, .doneacc

				;
.doneacc:		;
				; iz < 64
				test		iz, 64
				jnz			.donec
				jmp			hacc.loop

.eobc			; svg

				; feed @@@
				feed .donec, eobacc, .donec, .donec, .herr
				;
.donec:			;


;
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

;
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;                            DCT Chrom
;                    -------------------------

idctc:

;					%include "inclu/idft3pix-2.s"

;return
;
;xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
;


				; svg
;				mov			byte [rsi], 0
;				mov			byte [rsi+1], r13b
;				mov			byte [rsi+2], 255
;				add			rsi, 3

				sub			byte [rbp - VAR + _ui_uc], 1
				jnz			hdcc.loophdcc

				; END MCU
; -----------------------------------------------------------------------------
				; RESTART

.donemcu:		;
				sub			byte [rbp - VAR + _ri_us], 1
				jnz			decmain.loopri

;return
coopsample:
				;
				bt			flags, 62
				jnc 		decmain.loopsample






; -----------------------------------------------------------------------------

decoderdone:	;
				; svg
				mov			eax, dword [rbp - VAR + nblocks]
				mov			dword [rsi], eax
				add			rsi, 4
				mov			dword [rsi], -9999			; ###
				add			rsi, 4
				mov			dword [rsi], -9999			; ###
				mov			qword [rsi + 4], bits
				mov			dword [rsi + 12], -10000		; ###
				mov			rax, data

				;
				return
























;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
lfeed32:
%macro testfeed 5
			; @@@@
			;
			test		off, 32
			jnz			%1
			bt			flags, 63	; flag stop after a RSTi or EOI (+ flag eoi 62)
			jc			%1
			;						; @@@@@@@@@@@@@@@@@@@@@@@@@@
;			xor		rax, rax		; for now no move at RST (cd .ff)
;			xor		rdx, rdx
			mov		cl, 4
.loop%2		; loop 4 bytes
			mov		dl, byte [data]
			cmp		dl, 0xff
			jz		.ff%2
			; 1 ok
.put%2:		shl		eax, 8
			or		eax, edx
			add		data, 1
			sub		cl, 1
			jnz		.loop%2
			; 4 bytes done
.cp%2		mov		cl, 32
			sub		cl, off
			shl		rax, cl
			or		bits, rax
			add		off, 32
			xor		rax, rax
			xor		rdx, rdx
			jmp		%1

.ff%2:		;
;			add 	data, 1
			mov 	dl, byte [data + 1]
			cmp		dl, 0x00
			jnz		.marker%2
			; FF00
			add 	data, 1				; discard 00
			mov		dl, 0xff
			jmp		.put%2
			;

.marker%2:	;
			cmp		dl, 0xd8
			jge		.mrk2%2				; > ffd7
			cmp		dl, 0xcf
			jle		.mrk1%2				; < ffd0
			; RSTi   [ffD0 - ffd7]
.sh%2		shl		eax, 8				; 0
			sub		cl, 1
			jnz		.sh%2
			bts		flags, 63			; rst stop flag set
			add		data, 2				; rst discard
			jmp		.cp%2
			;

.mrk2%2:	; 				>= 0xFFD8
			je		.soi%2			; ffd8 : SOI*
			cmp		dl, 0xda		;
			jl		.eoi%2			; ffd9 : EOI*
			je		.err%2			; ffda : SOS*
			cmp		dl, 0xdc
			jl		.err%2			; ffdb : DQT*
			je		.err%2			; ffdc : DNL
			cmp		dl, 0xde
			jl		.err%2			; ffdd : DRI*
			je		.err%2			; ffde : DHP
			cmp		dl, 0xe0
			jl		.err%2			; ffdf : EXP
			je		.err%2			; ffe0 : APP0*
			cmp		dl, 0xf0
			jl		.err%2			; ffei : APPi
			je		.err%2			; fff0 : JPG0
			cmp		dl, 0xfe
			jl		.err%2			; fffi : JPGn
			je		.err%2			; fffe : COM
			;
			jmp		%5				; error

.eoi%2:		; EOI
			add		data, 2
			bts		flags, 63			; stop flag set
			bts		flags, 62			; eoi flag set
			jmp		%1

.soi%2:		; SOI
			jmp		%5

.mrk1%2		; < ffd0				; 0xffc[0-15]    SOF0: ffc0, DHT: ffc4, DAC: ffcc, JPEG: ffc8
			cmp		dl, 0xC0
			je		.err%2			; ffC0 : SOF0*
			jmp		%5

.err%2:		; ERR
			jmp		%5

%endmacro

			ret

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
				mov			byte [rbp - 0X10 + rax], r12b
				jmp			.coot

.ff:
				add			r9, 1
				cmp			byte [data + 1], 0
				jnz			.marker
				add			r10, 1
				add			data, 2
				sub			rsi, 1
				sub			al, 1
				mov			byte [rbp - 0x10 + rax], 0xFF
				jmp			.coot


.marker
				add			r11, 1
				add			data, 2
				sub			rsi, 1

.coot:
				cmp			al, 0
				jnz			.loop
				mov			ecx, dword [rbp - 0x10]

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
			mov		data, rdi
			mov		rsi, rdx
			xor		r9, r9
			xor		r10, r10
			xor		r11, r11
			xor		r12, r12
			;

.ltest:

			mov		cl, 4
.loop
			mov		dl, byte [data]
			cmp		dl, 0xff
			jz		.ff
			add		data, 1
			mov		dl, byte [data]
			cmp		dl, 0xff
			jz		.ff
			mov		dl, byte [data]
			cmp		dl, 0xff
			jz		.ff
			mov		dl, byte [data]
			cmp		dl, 0xff
			jz		.ff

.ff1:
.ff2:
.ff3:
.ff4:

			; ret ff
.put:		shl		eax, 8
			or		eax, edx
			add		data, 1
			sub		cl, 1
			jnz		.loop
			; ...
			jmp		.ctest
			; ...
			ret
			;
.ff:
;											add		r9, 1		; ff
			;
			add 	data, 1
			mov 	dl, byte [data]
			cmp		dl, 0x00
			jnz		.mrk
;											add		r10, 1		; ff00

			mov		dl, 0xff
			jmp		.put
			;
.mrk:
			;
;											add		r11, 1		; ffxx

			cmp		dl, 0xd9
			jz		.eoi
			shr		dl, 4
			cmp		dl, 0x0d
			jnz		.err
			add		data, 1
			jmp		.loop
			;

.ctest:
			jmp		.ltest


.eoi:
			mov		dword [rsi], r9d
			mov		dword [rsi+4], r10d
			mov		dword [rsi+8], r11d
			mov		rax, 0
			ret

.err:
			mov		rax, -1
			ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; scan645(void *ptr, int size, int[] res)
%define		data	r8

scan645l:
				begin 0x30

				mov			r8, rdi
				xor			r9, r9
				xor			r10, r10
				xor			r11, r11
				xor			r12, r12
				mov			r13, rdx
				xor			rax, rax
				xor			rdx, rdx
				xor			rcx, rcx
				;
				and			rsi, 0xfffffff8
				add			rsi, 8

.loop:
				;
				movdqu		xmm0, oword [data]
				movdqa		xmm1, oword [FF16]
				pxor		xmm1, xmm0
				movdqa		oword [rsp - 0x30], xmm0
				movdqa		oword [rsp - 0x20], xmm1
				movdqa		xmm1, oword [F128]
				movdqa		oword [rsp - 0x10], xmm1
				;
				mov			al, 0
				mov			cl, 0

.t
				add			r12, 1							; xx
				mov			byte [rsp - 0x10 + rcx], al
				test		byte [rsp - 0x20 + rcx], 0xff
				jnz			.n
				; FF
				add			r9, 1							; FF
				test		byte [rsp - 0x30 + rcx + 1], 0xff
				jnz			.m
				; FF00
				add			r10, 1							; FF00
				add			al, 1
				add			r12, 1							; xx
;				add			cl, 1
;				mov			byte [rsp - 0x10 + rcx], al
				jmp			.n

.m				; 0xFFD9 - EOI
				add			r11, 1							; FFxx
				add			r12, 2							; xx
				add			al, 2
				mov			byte [rsp - 0x10 + rcx], al


.n
				add			al, 1
				add			cl, 1
				cmp			cl, 8
				jl			.t

.c:

				;
				movaps		xmm1, oword [rsp - 0x10]
				movaps		xmm0, oword [rsp - 0x30]
				pshufb		xmm0, xmm1
				movaps		oword [rsp - 0x20], xmm0

				movbe		rdx, qword [rsp - 0x20]
				;

				add			data, rax
				sub			rsi, rax
				jg			.loop

.e:
				mov			rax, rdx
				mov			dword [r13], r9d
				mov			dword [r13+4], r10d
				mov			dword [r13+8], r11d
				mov			dword [r13+12], r12d
				return




%define		symb	cl
%define		off		r8d
%define		bits	r9

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int test_acl645(uint8_t data, int off, int *res)
test_acl645:
			xor		rcx, rcx
			xor		r8, r8
			mov		r8, rsi
			mov		rsi, rdx
			mov		bits, rdi


%include "inclu/hufacl-3.s"


.herr:
			mov		rax, -1
			ret

.donehacl:
			mov		dword [rsi], ecx
			mov		dword [rsi+4], r8d
			xor		rax, rax
ret
