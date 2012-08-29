default rel

extern sin
global polya650: 	function

SECTION .data
ONE		dq				1.0
HALF	dq				0.5
PAS		dq				0.65

SECTION .text  align=16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; double polya650(double *coe, int len, double x)
polya650:
		push			rbx					; need to be saved
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
		pop				rbx					; restore rbx
		ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
