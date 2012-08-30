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
			mov			rcx, rdx
			shr			rcx, 4			;
			jrcxz		_end0
			mov			rax, rdi
			mov			rdx, rsi
_lp0_0:		; loop 16
			movdqa		xmm0, [rax]
			paddb		xmm0, [rdx]
			movdqa		[rbp - 16], xmm0
_cn0_:

_end0:
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int mula050(uint8_t *number1, int len1, uint32_t fac)
mula050:




		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int diva050(uint8_t *number1, int len1, uint32_t div)
diva050:




		ret
