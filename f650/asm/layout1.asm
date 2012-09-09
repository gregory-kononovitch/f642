;
; file    : layout1.asm
; project : f640
;
; Created on: Sep 9, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;

default rel

global inserta16a650:	function
;
global imgfill1a650:	function
global imgfill2a650:	function
global imgfill12a650:	function
global imgfill123a650:	function


SECTION .data

SECTION .text  align=16


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; img (960x540)  -> (1024, 600)
; inserta650(bgra650 *img16a, bgra650 *into16a)
inserta16a650:
			;
			xor				r8, r8
			xor				r9, r9
			xor				r10, r10
			xor				r11, r11
			mov				r8w,  [rsi + 8]			; wdest
			mov				r9w,  [rsi + 10]		; hdest
			mov				r10w, [rdi + 8]			; wsrc
			mov				r11w, [rdi + 10]		; hsrc
			;
			mov				eax, r8d
			sub				eax, r10d				; dif
			shl				eax, 2					; int
			;
			sub				r9d, r11d
			imul			r9d, r8d
			shl				r9d, 1					; dh2
			mov				ecx, r8d
			sub				ecx, r10d
			shl				ecx, 1					; dw2
			;
			mov				rsi, [rsi]
			add				rsi, r9					; + dh2
			sub				rsi, rcx				; - dw2
			shl				ecx, 1					; dw
			mov				r8d, ecx				; dw
			;
			mov				rdi, [rdi]
			shr				r10d, 2					; wsrc * 4 / 16
;			shr				r10d, 1
			;
			mov				eax, r11d
.loopy:
			add				rsi, r8
			mov				ecx, r10d
			;
.loopx:		;
;			movdqa			xmm1, oword [rdi + 16]
;			movdqa			oword [rsi + 16], xmm1
			movdqa			xmm0, oword [rdi]
			movdqa			oword [rsi], xmm0
			;
.cootx:
			add				rdi, 16
			add				rsi, 16
			sub				ecx, 1
			jnz				.loopx
;			loop			.loopx
			;
.cooty:		;
			sub				eax, 1
			jnz				.loopy
			;
			ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; long imgfill1a650(bgra650 *dest, uint32_t color, void *pties)
imgfill1a650:
			;
			mov				rdi, [rdi]				; img
			mov				eax, dword [rdx + 16]	; offset0
			add				rdi, rax				; + offset0
			;
			xor				r10, r10
			mov				r10w, word [rdx + 20]	; dword1
			;
			xor				r8, r8
			mov				r8w, word [rdx + 26]	; offset4
			;
			xor				r9, r9
			mov				r9w, word [rdx + 28]	; hsrc
			; even
			mov				eax, esi
			shl				rsi, 32
			or				rsi, rax
			test			r10d, 0x01
			jnz				odd
			shr				r10d, 1
.loopy:		; -------------
			mov				ecx, r10d				; dword1
.loopx:		; -------
			mov				qword [rdi], rsi
			;
			add				rdi, 8
			sub				ecx, 1
			jnz				.loopx
			; -------
			add				rdi, r8
			sub				r9d, 1
			jnz				.loopy
			; -------------
			ret
; ---------------------------------
odd:
			mov				eax, esi
			shl				esi, 32
			or				rsi, rax				; color
			;
			shr				r10d, 1
			add				r8d, 4
.loopy
			mov				ecx, r10d				; dword1
			;
.loopx
			mov				qword [rdi], rsi
			;
			add				rdi, 8
			sub				ecx, 1
			jnz				.loopx
;			loop			.loopx
			;
			mov				dword [rdi], esi		; odd
			;
			add				rdi, r8
			sub				r9d, 1					; h
			jnz				.loopy
			;
			ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; long imgfill2a650(bgra650 *dest, uint32_t color, void *pties)
imgfill2a650:


			ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; long imgfill12a650(bgra650 *dest, uint32_t color, void *pties)
imgfill12a650:		; ### broken

			;
			ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; long imgfill123a650(bgra650 *dest, uint32_t color, void *pties)
imgfill123a650:

			;
			ret
