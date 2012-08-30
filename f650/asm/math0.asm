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
; cd mpsadbw
adda050:
		sub				rdx, 1
		shl				rdx, 1
		add				rdi, rdx
		add				rsi, rdx
		shr				rdx, 1
		xor				rax, rax
		mov				ax, [NINE]
		xor				rcx, rcx
LOOP0:
		add				cx, [rdi]
		add				cx, [rsi]
		cmp				cx, ax
		jbe				L9
		sub				cx, 10
		mov				[rdi], cx
		mov				rcx, 1
		jmp				COOP0

L9		; <= 9
		mov				[rdi], word cx
		xor				rcx, rcx
COOP0:
		sub				rdx, 1
		cmp				rdx, 0
		jns				END0
		;
		sub				rdi, 2
		sub				rsi, 2
		jmp				LOOP0
END0:
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int mula050(uint8_t *number1, int len1, uint32_t fac)
mula050:




		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; int diva050(uint8_t *number1, int len1, uint32_t div)
diva050:




		ret
