default rel

extern sin
global polya650: 	function
global ax2bxca650:	function

SECTION .data
ONE		dq				1.0
TWO		dq				2.0
FOUR	dq				4.0
HALF	dq				0.5
PAS		dq				0.65

SECTION .text  align=16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; double polya650(double *coe, int len, double x)
polya650:
;		push			rbx					; need to be saved
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
;		call			sin
;		pop				rbx					; restore rbx
		ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int ax2bxca650(double a, double b, double c, double *res)
ax2bxca650:
		; a == 0
		xorpd  			xmm9, xmm9
		ucomisd 		xmm9, xmm0
		jne    			Ad0
		jp     			Ad0
		; b == 0
		ucomisd 		xmm9, xmm1
		jne    			A0Bd0
		jp     			A0Bd0
		; a == 0 && b == 0
		mov				eax, 0
		ret

A0Bd0:	; a == 0 && b != 0
		subsd			xmm9, xmm2			; -c
		divsd			xmm9, xmm1
		movsd			[rdi], xmm9
		mov				eax, 1
		ret

Ad0:	; a != 0
		movsd			xmm10, xmm1
		mulsd			xmm10, xmm1
		movsd			xmm11, [FOUR]
		mulsd			xmm11, xmm0
		mulsd			xmm11, xmm2
		subsd			xmm10, xmm11		; b² - 4ac
		;
		ucomisd			xmm10, xmm9
		seta			al
		test			al, al
		je				Dle0				; 0 >= D
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
		seta			al
		test			al, al
		je				Dg0Ag0				; a >(=) 0
		; D > 0 && a < 0
		movsd			[rdi], xmm10
		movsd			[rdi + 8], xmm13
		mov				eax, 2
		ret

Dg0Ag0:	; D > 0 && a > 0
		movsd			[rdi], xmm13
		movsd			[rdi + 8], xmm10
		mov				eax, 2
		ret

Dle0:	; D <= 0
		subsd			xmm9, xmm1				; -b
		mulsd			xmm9, [HALF]
		divsd			xmm9, xmm0
		movsd			[rdi], xmm9
		mov				eax, 1
		ret
