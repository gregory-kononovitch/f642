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
global inserta650:		function
;
global imgfill1a650:	function

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


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; img (960x540)  -> (1024, 600)
; inserta650(bgra650 *img, bgra650 *into)
inserta650:
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
			loop			.loopx
			;
.cooty:		;
			sub				eax, 1
			jnz				.loopy
			;
			ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; long imgfill1a650(bgra *dest, uint32_t color, void *pties)
imgfill1a650:
			;
			mov				rdi, [rdi]
			mov				eax, dword [rdx]		; offset0
			add				rdi, rax				; + offset0
			mov				r8d, dword [rdx + 16]	; offset4
			;
			mov				r10d, dword [rdx + 4]	; dword1
			mov				r9d, dword [rdx + 20]	; hsrc
			;
.loopy
			mov				ecx, r10d				; dword1
.loopx
			mov				dword [rdi], esi
			;
			add				rdi, 4
			loop			.loopx
			;
			add				rdi, r8
			sub				r9d, 1					; h
			jnz				.loopy
			;
			ret


