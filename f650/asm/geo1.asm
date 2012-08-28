default rel

global norma650: function
global unita650: function
global scala650: function
global vecta650: function
global adda650: function
global suba650: function
global mul_and_adda650: function
global mul_and_suba650: function
global change_vecta650: function
global change_pointa650: function


SECTION .data
ONE		dq			1.0


SECTION .text  align=16

; double norma650(vect650 *u)
norma650:
		movsd		xmm0, [rdi + 0]
		mulsd		xmm0, xmm0
		movsd		xmm1, [rdi + 8]
		mulsd		xmm1, xmm1
		addsd		xmm0, xmm1
		movsd		xmm1, [rdi + 16]
		mulsd		xmm1, xmm1
		addsd		xmm0, xmm1
		sqrtsd		xmm0, xmm0
		ret

; double unita650(vect650 *u)
unita650:
		movsd		xmm0, [rdi + 0]
		mulsd		xmm0, xmm0
		movsd		xmm1, [rdi + 8]
		mulsd		xmm1, xmm1
		addsd		xmm0, xmm1
		movsd		xmm1, [rdi + 16]
		mulsd		xmm1, xmm1
		addsd		xmm0, xmm1
		;
		xorpd		xmm1, xmm1
		ucomisd		xmm0, xmm1
		seta		al
		test		al, al
		je			END					; 0 >= xmm0
		sqrtsd		xmm0, xmm0
		;
		movsd		xmm4, qword [ONE];
		divsd		xmm4, xmm0
		movsd		xmm1, [rdi + 0]
		mulsd		xmm1, xmm4
		movsd		[rdi + 0], xmm1
		movsd		xmm1, [rdi + 8]
		mulsd		xmm1, xmm4
		movsd		[rdi + 8], xmm1
		movsd		xmm1, [rdi + 16]
		mulsd		xmm1, xmm4
		movsd		[rdi + 16], xmm1
END:	ret

; void adda650(vect650 *u, vect650 *v)
adda650:
		movsd		xmm0, [rdi + 0]
		movsd		xmm1, [rsi + 0]
		addsd		xmm0, xmm1
		movsd		[rdi + 0], xmm0
		;
		movsd		xmm0, [rdi + 8]
		movsd		xmm1, [rsi + 8]
		addsd		xmm0, xmm1
		movsd		[rdi + 8], xmm0
		;
		movsd		xmm0, [rdi + 16]
		movsd		xmm1, [rsi + 16]
		addsd		xmm0, xmm1
		movsd		[rdi + 16], xmm0
		ret

; void adda650(vect650 *u, vect650 *v)
suba650:
		movsd		xmm0, [rdi + 0]
		movsd		xmm1, [rsi + 0]
		subsd		xmm0, xmm1
		movsd		[rdi + 0], xmm0
		;
		movsd		xmm0, [rdi + 8]
		movsd		xmm1, [rsi + 8]
		subsd		xmm0, xmm1
		movsd		[rdi + 8], xmm0
		;
		movsd		xmm0, [rdi + 16]
		movsd		xmm1, [rsi + 16]
		subsd		xmm0, xmm1
		movsd		[rdi + 16], xmm0
		ret

; void add_mula650(vect650 *u, double a, vect650 *v)
mul_and_adda650:
		movsd		xmm1, [rdi + 0]
		movsd		xmm2, [rsi + 0]
		mulsd		xmm2, xmm0
		addsd		xmm1, xmm2
		movsd		[rdi + 0], xmm1
		;
		movsd		xmm1, [rdi + 8]
		movsd		xmm2, [rsi + 8]
		mulsd		xmm2, xmm0
		addsd		xmm1, xmm2
		movsd		[rdi + 8], xmm1
		;
		movsd		xmm1, [rdi + 16]
		movsd		xmm2, [rsi + 16]
		mulsd		xmm2, xmm0
		addsd		xmm1, xmm2
		movsd		[rdi + 16], xmm1
		ret

; void sub_mula650(vect650 *u, double a, vect650 *v)
mul_and_suba650:
		movsd		xmm1, [rdi + 0]
		movsd		xmm2, [rsi + 0]
		mulsd		xmm2, xmm0
		subsd		xmm1, xmm2
		movsd		[rdi + 0], xmm1
		;
		movsd		xmm1, [rdi + 8]
		movsd		xmm2, [rsi + 8]
		mulsd		xmm2, xmm0
		subsd		xmm1, xmm2
		movsd		[rdi + 8], xmm1
		;
		movsd		xmm1, [rdi + 16]
		movsd		xmm2, [rsi + 16]
		mulsd		xmm2, xmm0
		subsd		xmm1, xmm2
		movsd		[rdi + 16], xmm1
		ret


; double scala650(vect650 *u, vect650 *v)
scala650:
		movsd		xmm0, [rdi + 0]
		movsd		xmm1, [rsi + 0]
		mulsd		xmm0, xmm1
		;
		movsd		xmm1, [rdi + 8]
		movsd		xmm2, [rsi + 8]
		mulsd		xmm1, xmm2
		addsd		xmm0, xmm1
		;
		movsd		xmm1, [rdi + 16]
		movsd		xmm2, [rsi + 16]
		mulsd		xmm1, xmm2
		addsd		xmm0, xmm1
		ret

; double vecta650(vect650 *u, vect650 *v, vect650 *w) : RDI, RSI, RDX
vecta650:
		movsd		xmm0, [rdi + 8]
		movsd		xmm1, [rsi + 16]
		mulsd		xmm0, xmm1
		movsd		xmm1, [rdi + 16]
		movsd		xmm2, [rsi + 8]
		mulsd		xmm1, xmm2
		subsd		xmm0, xmm1
		movsd		[rdx + 0], xmm0
		;
		movsd		xmm0, [rdi + 16]
		movsd		xmm1, [rsi + 0]
		mulsd		xmm0, xmm1
		movsd		xmm1, [rdi + 0]
		movsd		xmm2, [rsi + 16]
		mulsd		xmm1, xmm2
		subsd		xmm0, xmm1
		movsd		[rdx + 8], xmm0
		;
		movsd		xmm0, [rdi + 0]
		movsd		xmm1, [rsi + 8]
		mulsd		xmm0, xmm1
		movsd		xmm1, [rdi + 8]
		movsd		xmm2, [rsi + 0]
		mulsd		xmm1, xmm2
		subsd		xmm0, xmm1
		movsd		[rdx + 16], xmm0
		ret

;vect650 *change_vecta650(vect650 *u)
change_vecta650:


;vect650 *change_pointa650(vect650 *p);
change_pointa650:
