default rel

;
global polya650: 	function
global poly2a650: 	function

SECTION .data
NINE	dw				9
TEN		dw				10
ONE		dq				1.0
TWO		dq				2.0
FOUR	dq				4.0
HALF	dq				0.5
PAS		dq				0.65
Ue12	dq				1000000000000

SECTION .text  align=16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; double polya650(double *coe, int len, double x)
polya650:
		mov				rcx, rsi			; len
		; init
		movsd			xmm2, [ONE]
		movsd			xmm3, xmm0			; xmm3 = x
		movsd			xmm0, [rdi]			; xmm0 = cst = res
		sub				rcx, 1
		add				rdi, 8
_lp0:
		mulsd			xmm2, xmm3			; x^i
		movsd			xmm1, [rdi]			; ai
		mulsd			xmm1, xmm2			; ai.x^i
		addsd			xmm0, xmm1			; res
		add				rdi, 8				; next ai
		loop			_lp0

_ed0:
		ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; double polya650(double *coe, int len2, double x)
poly2a650:
			mov			rcx, rsi
			shr			rcx, 1
			sub			rcx, 1
			cmp			rcx, 2
			jb			_ex1_1
			movsd		xmm1, qword [ONE]
			movsd		qword [rbp - 16], xmm1
			movsd		qword [rbp - 8], xmm0
			movdqa		xmm1, oword [rbp - 16]		; xmm1 = 1, x
			movdqa		xmm2, oword [rbp - 16]		;
			movhpd		xmm2, qword [rbp - 8]		; x, x
			mulpd		xmm2, xmm2					; xmm2 = x², x²
_lp1:		; ---------------
			movapd		xmm4, oword [rdi]
			mulpd		xmm4, xmm1
			addpd		xmm5, xmm4
			;
			mulpd		xmm1, xmm2				; i
			add			rdi, 16
			loop		_lp1
			; ----------------
			movapd		xmm4, oword [rdi]
			mulpd		xmm4, xmm1
			addpd		xmm5, xmm4
			;
			movdqa		oword [rbp - 16], xmm5
			movsd		xmm0, qword [rbp - 16]
			addsd		xmm0, qword [rbp - 8]

_ex1_1:		ret
