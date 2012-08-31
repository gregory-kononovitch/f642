default rel

;
global adda050:		function
global mula050:		function
global diva050:		function

SECTION .data
NINE	dw				9
TEN		dw				10
ONE		dq				1.0
TWO		dq				2.0
FOUR	dq				4.0
HALF	dq				0.5
PAS		dq				0.65

SECTION .text  align=16


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int adda050(uint8_t *number1, uint8_t *number2, int len)
; cf movsx and movzx
; cf movdqa paddb
adda050:
			mov			rcx, rdx		;
			shr			rcx, 4			; cpt
			jrcxz		_end0
			mov			rax, rdi		; n1
			mov			rdx, rsi		; n2
			mov			bx, 0			; ret
			jmp			_lp0_0

_end0:		ret

_lp0_0:		; loop nb
			movdqa		xmm0, oword [rax]
			paddb		xmm0, oword [rdx]
			movdqa		[rbp - 16], xmm0
			cmp			bx, 0
			jz			_c0_16
			add			byte [rbp - 16], 1
			; ---------------------------------------------
_c0_16:		cmp			byte [rbp - 16], 9
			ja			_a10_16
			jmp			_c0_15
_a10_16:	;
			sub			byte [rbp - 16], 10
			add			byte [rbp - 15], 1
			; ---------------------------------------------
_c0_15:		cmp			byte [rbp - 15], 9
			ja			_a10_15
			jmp			_c0_14
_a10_15:	;
			sub			byte [rbp - 15], 10
			add			byte [rbp - 14], 1
			; ---------------------------------------------
_c0_14:		cmp			byte [rbp - 14], 9
			ja			_a10_14
			jmp			_c0_13
_a10_14:	;
			sub			byte [rbp - 14], 10
			add			byte [rbp - 13], 1
			; ---------------------------------------------
_c0_13:		cmp			byte [rbp - 13], 9
			ja			_a10_13
			jmp			_c0_12
_a10_13:	;
			sub			byte [rbp - 13], 10
			add			byte [rbp - 12], 1
			; ---------------------------------------------
_c0_12:		cmp			byte [rbp - 12], 9
			ja			_a10_12
			jmp			_c0_11

_w0_12:		jmp			_lp0_0

_a10_12:	;
			sub			byte [rbp - 12], 10
			add			byte [rbp - 11], 1
			; ---------------------------------------------
_c0_11:		cmp			byte [rbp - 11], 9
			ja			_a10_11
			jmp			_c0_10
_a10_11:	;
			sub			byte [rbp - 11], 10
			add			byte [rbp - 10], 1
			; ---------------------------------------------
_c0_10:		cmp			byte [rbp - 10], 9
			ja			_a10_10
			jmp			_c0_9
_a10_10:	;
			sub			byte [rbp - 10], 10
			add			byte [rbp - 9], 1
			; ---------------------------------------------
_c0_9:		cmp			byte [rbp - 9], 9
			ja			_a10_9
			jmp			_c0_8
_a10_9:		;
			sub			byte [rbp - 9], 10
			add			byte [rbp - 8], 1
			; ---------------------------------------------
_c0_8:		cmp			byte [rbp - 8], 9
			ja			_a10_8
			jmp			_c0_7

_w0_8:		jmp			_w0_12

_a10_8:	;
			sub			byte [rbp - 8], 10
			add			byte [rbp - 7], 1
			; ---------------------------------------------
_c0_7:		cmp			byte [rbp - 7], 9
			ja			_a10_7
			jmp			_c0_6
_a10_7:	;
			sub			byte [rbp - 7], 10
			add			byte [rbp - 6], 1
			; ---------------------------------------------
_c0_6:		cmp			byte [rbp - 6], 9
			ja			_a10_6
			jmp			_c0_5
_a10_6:	;
			sub			byte [rbp - 6], 10
			add			byte [rbp - 5], 1
			; ---------------------------------------------
_c0_5:		cmp			byte [rbp - 5], 9
			ja			_a10_5
			jmp			_c0_4
_a10_5:	;
			sub			byte [rbp - 5], 10
			add			byte [rbp - 4], 1
			; ---------------------------------------------
_c0_4:		cmp			byte [rbp - 4], 9
			ja			_a10_4
			jmp			_c0_3

_w0_4:		jmp			_w0_8

_a10_4:	;
			sub			byte [rbp - 4], 10
			add			byte [rbp - 3], 1
			; ---------------------------------------------
_c0_3:		cmp			byte [rbp - 3], 9
			ja			_a10_3
			jmp			_c0_2
_a10_3:	;
			sub			byte [rbp - 3], 10
			add			byte [rbp - 2], 1
			; ---------------------------------------------
_c0_2:		cmp			byte [rbp - 2], 9
			ja			_a10_2
			jmp			_c0_1
_a10_2:	;
			sub			byte [rbp - 2], 10
			add			byte [rbp - 1], 1
			; ---------------------------------------------
_c0_1:		cmp			byte [rbp - 1], 9
			ja			_a10_1
			xor			bx, bx
			jmp			_cn0
_a10_1:		;
			sub			byte [rbp - 1], 10
			mov			bx, 1

_cn0:		; ---------------------------------------------
			movdqa		xmm0, oword [rbp - 16]
			movdqa		oword [rax], xmm0
			;
			add			rax, 16
			add			rdx, 16
			loop		_w0_4

_end1:
		ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int mula050(char *number1, int len1, uint32_t fac)
; [-4  ;  0[  : pad
; [-16 ; -8[  : values
; [-20 ; -16[ : 10
; [-24 ; -20[ : fac
; [-28 ; -24[ : remainder
; [-32 ; -28[ : ecx (loop 0)
; [-40 ; -32[ : digits "be"
; [-44 ; -40[ : 8
mula050:
			mov			rcx, rsi
			shr			rcx, 3
			jrcxz		_1end0
			mov			rcx, 1
			mov			dword [rbp - 20], 10	; 10
			mov			dword [rbp - 24], edx	; fac
			mov			dword [rbp - 28], 0		; reste
			mov			dword [rbp - 44], 8		; 8
			jmp			_1lp0

_1end0:		ret

_1lp0:
			mov			rax, qword [rdi]
			mov			qword [rbp - 16], rax	; digits
			mov			qword [rbp - 64], 0		; mul digits
			mov			dword [rbp - 32], ecx	; save ecx
			mov			r9, 8
			mov			r8, rbp
			sub			r8, 16					; digit
			mov			cl, 0					; shift

_1lp1:
			mov			eax, dword [r8]
			and			rax, 0xFF				; digit
			xor			rdx, rdx				; div
			mul			dword [rbp - 24]		; * fac
			add			eax, dword [rbp - 28]	; rem
			div			dword [rbp - 20]		; / 10
			mov			dword [rbp - 28], eax 	; quotient
			shl			rdx, cl
			add			qword [rbp - 64], rdx

_1ct1:		;
			inc			r8
			add			cl, 8
			dec			r9
			cmp			r9, 0
			jnz			_1lp1

_1ct0:		;
			mov			rax, qword [rbp - 64]
			mov			qword [rdi], rax
			mov			ecx, [rbp - 32]
			add			rdi, 8
			mov			rax, rcx
			loop		_1lp0

			mov			rax, rdi
			ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int diva050(char *number1, int len1, uint32_t div)
diva050:




		ret
