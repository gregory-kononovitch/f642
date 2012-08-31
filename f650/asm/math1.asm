default rel

global ax2bxca650:	function
global ax2bxca650f:	function

SECTION .data
ONE		dq				1.0
TWO		dq				2.0
FOUR	dq				4.0
FOURs	dd				4.0
HALF	dq				0.5
HALFs	dd				0.5
PAS		dq				0.65

SECTION .text  align=16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; double polya650(double *coe, int len, double x)
polya650:
		xor				rax, rax			; eax = index
		mov				rcx, rsi
		movsd			xmm2, [ONE]
		movsd			xmm3, xmm0			; xmm3 = x
		movsd			xmm0, [rdi]			; xmm0 = cst = res
		add				eax, 1
		add				rdi, 8
LOOP1:
		cmp				eax, ecx
		jge				END1
		;
		mulsd			xmm2, xmm3
		movsd			xmm1, [rdi]
		mulsd			xmm1, xmm2
		addsd			xmm0, xmm1
		add				eax, 1
		add				rdi, 8
		jmp				LOOP1

END1:
		ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int ax2bxca650(double a, double b, double c, double *res)
ax2bxca650:
		; a == 0 ?
		xorpd  			xmm9, xmm9		; @@@ negsd ?
		ucomisd 		xmm9, xmm0
		je    			A0
Ad0:	; a != 0
		movsd			xmm10, xmm1
		mulsd			xmm10, xmm1
		movsd			xmm11, [FOUR]
		mulsd			xmm11, xmm0
		mulsd			xmm11, xmm2
		subsd			xmm10, xmm11		; b² - 4ac
		;
		ucomisd			xmm10, xmm9
		jbe				Dbe0				; D <= 0
		; D > 0
		sqrtsd			xmm10, xmm10
		movsd			xmm12, [HALF]
		divsd			xmm12, xmm0			; 1 / (2 * a)
		xorpd			xmm13, xmm13
		subsd			xmm13, xmm10		; -d
		subsd			xmm13, xmm1			; -b
		mulsd			xmm13, xmm12		; x-
		subsd			xmm10, xmm1
		mulsd			xmm10, xmm12		; x+
		ucomisd			xmm9, xmm0
		jb				Da0Ab0				; a < 0
		; D > 0 && a > 0
		movsd			[rdi], xmm10		; x-
		movsd			[rdi + 8], xmm13	; x+
		mov				eax, 2
		ret

Da0Ab0:	; D > 0 && a < 0
		movsd			[rdi], xmm13
		movsd			[rdi + 8], xmm10
		mov				eax, 2
		ret

Dbe0:	; D <= 0
		je				De0
		; D < 0
		mov				eax, 0
		ret

De0:	; D == 0
		subsd			xmm9, xmm1				; -b
		mulsd			xmm9, [HALF]
		divsd			xmm9, xmm0
		movsd			[rdi], xmm9
		mov				eax, 1
		ret

A0:		; a == 0
		ucomisd 		xmm1, xmm9
		je    			A0B0
		; a == 0 && b != 0
		subsd			xmm9, xmm2			; -c
		divsd			xmm9, xmm1
		movsd			[rdi], xmm9
		mov				eax, 1
		ret

A0B0:
		; a == 0 && b == 0
		mov				eax, 0
		ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int ax2bxca650(float a, float b, float c, float *res)
ax2bxca650f:
		; a == 0 ?
		xorpd  			xmm9, xmm9		; @@@ negsd ?
		ucomiss 		xmm9, xmm0
		je    			A0s
Ad0s:	; a != 0
		movss			xmm10, xmm1
		mulss			xmm10, xmm1
		movss			xmm11, [FOURs]
		mulss			xmm11, xmm0
		mulss			xmm11, xmm2
		subss			xmm10, xmm11		; b² - 4ac
		;
		ucomiss			xmm10, xmm9
		jbe				Dbe0s				; D <= 0
		; D > 0
		sqrtss			xmm10, xmm10
		movss			xmm12, [HALFs]
		divss			xmm12, xmm0			; 1 / (2 * a)
		xorpd			xmm13, xmm13
		subss			xmm13, xmm10		; -d
		subss			xmm13, xmm1			; -b
		mulss			xmm13, xmm12		; x-
		subss			xmm10, xmm1
		mulss			xmm10, xmm12		; x+
		ucomiss			xmm9, xmm0
		jb				Da0Ab0s				; a < 0
		; D > 0 && a > 0
		movss			[rdi], xmm10		; x-
		movss			[rdi + 4], xmm13	; x+
		mov				eax, 2
		ret

Da0Ab0s:; D > 0 && a < 0
		movss			[rdi], xmm13
		movss			[rdi + 4], xmm10
		mov				eax, 2
		ret

Dbe0s:	; D <= 0
		je				De0s
		; D < 0
		mov				eax, 0
		ret

De0s:	; D == 0
		subss			xmm9, xmm1				; -b
		mulss			xmm9, [HALFs]
		divss			xmm9, xmm0
		movss			[rdi], xmm9
		mov				eax, 1
		ret

A0s:	; a == 0
		ucomiss 		xmm1, xmm9
		je    			A0B0s
		; a == 0 && b != 0
		subss			xmm9, xmm2			; -c
		divss			xmm9, xmm1
		movss			[rdi], xmm9
		mov				eax, 1
		ret

A0B0s:
		; a == 0 && b == 0
		mov				eax, 0
		ret
