;
; file    : hufacc-3.s (static generated)
; project : f645 (t508.f640/2)
;
; Created on: Sep 26, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;
;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;
		shl 		bits, 1
		jc		.ac.case111111111111111111111111111111111x
		; 111111111111111111111111111111110x
		shl 		bits, 1
		jc		.ac.case01x
		; 00x
		; 00
		sub			off, 2
		mov			symb, 0
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case01x:
		; 01
		sub			off, 2
		mov			symb, 1
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111111111111111111111111111x:
		shl 		bits, 1
		jc		.ac.case11x
		; 10x
		shl 		bits, 1
		jc		.ac.case101x
		; 100x
		; 100
		sub			off, 3
		mov			symb, 2
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case101x:
		shl 		bits, 1
		jc		.ac.case1011x
		; 1010x
		; 1010
		sub			off, 4
		mov			symb, 3
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1011x:
		; 1011
		sub			off, 4
		mov			symb, 17
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11x:
		shl 		bits, 1
		jc		.ac.case111x
		; 110x
		shl 		bits, 1
		jc		.ac.case1101x
		; 1100x
		shl 		bits, 1
		jc		.ac.case11001x
		; 11000x
		; 11000
		sub			off, 5
		mov			symb, 4
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11001x:
		; 11001
		sub			off, 5
		mov			symb, 5
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1101x:
		shl 		bits, 1
		jc		.ac.case11011x
		; 11010x
		; 11010
		sub			off, 5
		mov			symb, 33
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11011x:
		; 11011
		sub			off, 5
		mov			symb, 49
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111x:
		shl 		bits, 1
		jc		.ac.case1111x
		; 1110x
		shl 		bits, 1
		jc		.ac.case11101x
		; 11100x
		shl 		bits, 1
		jc		.ac.case111001x
		; 111000x
		; 111000
		sub			off, 6
		mov			symb, 6
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111001x:
		; 111001
		sub			off, 6
		mov			symb, 18
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11101x:
		shl 		bits, 1
		jc		.ac.case111011x
		; 111010x
		; 111010
		sub			off, 6
		mov			symb, 65
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111011x:
		; 111011
		sub			off, 6
		mov			symb, 81
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111x:
		shl 		bits, 1
		jc		.ac.case11111x
		; 11110x
		shl 		bits, 1
		jc		.ac.case111101x
		; 111100x
		shl 		bits, 1
		jc		.ac.case1111001x
		; 1111000x
		; 1111000
		sub			off, 7
		mov			symb, 7
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111001x:
		; 1111001
		sub			off, 7
		mov			symb, 97
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111101x:
		shl 		bits, 1
		jc		.ac.case1111011x
		; 1111010x
		; 1111010
		sub			off, 7
		mov			symb, 113
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111011x:
		shl 		bits, 1
		jc		.ac.case11110111x
		; 11110110x
		; 11110110
		sub			off, 8
		mov			symb, 19
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11110111x:
		; 11110111
		sub			off, 8
		mov			symb, 34
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111x:
		shl 		bits, 1
		jc		.ac.case111111x
		; 111110x
		shl 		bits, 1
		jc		.ac.case1111101x
		; 1111100x
		shl 		bits, 1
		jc		.ac.case11111001x
		; 11111000x
		; 11111000
		sub			off, 8
		mov			symb, 50
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111001x:
		; 11111001
		sub			off, 8
		mov			symb, 129
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111101x:
		shl 		bits, 1
		jc		.ac.case11111011x
		; 11111010x
		shl 		bits, 1
		jc		.ac.case111110101x
		; 111110100x
		; 111110100
		sub			off, 9
		mov			symb, 8
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111110101x:
		; 111110101
		sub			off, 9
		mov			symb, 20
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111011x:
		shl 		bits, 1
		jc		.ac.case111110111x
		; 111110110x
		; 111110110
		sub			off, 9
		mov			symb, 66
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111110111x:
		; 111110111
		sub			off, 9
		mov			symb, 145
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111x:
		shl 		bits, 1
		jc		.ac.case1111111x
		; 1111110x
		shl 		bits, 1
		jc		.ac.case11111101x
		; 11111100x
		shl 		bits, 1
		jc		.ac.case111111001x
		; 111111000x
		; 111111000
		sub			off, 9
		mov			symb, 161
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111001x:
		; 111111001
		sub			off, 9
		mov			symb, 177
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111101x:
		shl 		bits, 1
		jc		.ac.case111111011x
		; 111111010x
		; 111111010
		sub			off, 9
		mov			symb, 193
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111011x:
		shl 		bits, 1
		jc		.ac.case1111110111x
		; 1111110110x
		; 1111110110
		sub			off, 10
		mov			symb, 9
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111110111x:
		; 1111110111
		sub			off, 10
		mov			symb, 35
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111x:
		shl 		bits, 1
		jc		.ac.case11111111x
		; 11111110x
		shl 		bits, 1
		jc		.ac.case111111101x
		; 111111100x
		shl 		bits, 1
		jc		.ac.case1111111001x
		; 1111111000x
		; 1111111000
		sub			off, 10
		mov			symb, 51
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111001x:
		; 1111111001
		sub			off, 10
		mov			symb, 82
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111101x:
		shl 		bits, 1
		jc		.ac.case1111111011x
		; 1111111010x
		; 1111111010
		sub			off, 10
		mov			symb, 240
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111011x:
		shl 		bits, 1
		jc		.ac.case11111110111x
		; 11111110110x
		; 11111110110
		sub			off, 11
		mov			symb, 21
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111110111x:
		; 11111110111
		sub			off, 11
		mov			symb, 98
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111x:
		shl 		bits, 1
		jc		.ac.case111111111x
		; 111111110x
		shl 		bits, 1
		jc		.ac.case1111111101x
		; 1111111100x
		shl 		bits, 1
		jc		.ac.case11111111001x
		; 11111111000x
		; 11111111000
		sub			off, 11
		mov			symb, 114
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111001x:
		; 11111111001
		sub			off, 11
		mov			symb, 209
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111101x:
		shl 		bits, 1
		jc		.ac.case11111111011x
		; 11111111010x
		shl 		bits, 1
		jc		.ac.case111111110101x
		; 111111110100x
		; 111111110100
		sub			off, 12
		mov			symb, 10
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111110101x:
		; 111111110101
		sub			off, 12
		mov			symb, 22
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111011x:
		shl 		bits, 1
		jc		.ac.case111111110111x
		; 111111110110x
		; 111111110110
		sub			off, 12
		mov			symb, 36
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111110111x:
		; 111111110111
		sub			off, 12
		mov			symb, 52
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111x:
		shl 		bits, 1
		jc		.ac.case1111111111x
		; 1111111110x
		shl 		bits, 1
		jc		.ac.case11111111101x
		; 11111111100x
		shl 		bits, 1
		jc		.ac.case111111111001x
		; 111111111000x
		shl 		bits, 1
		jc		.ac.case1111111110001x
		; 1111111110000x
		shl 		bits, 1
		jc		.ac.case11111111100001x
		; 11111111100000x
		; 11111111100000
		sub			off, 14
		mov			symb, 225
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111100001x:
		shl 		bits, 1
		jc		.ac.case111111111000011x
		; 111111111000010x
		; 111111111000010
		sub			off, 15
		mov			symb, 37
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111000011x:
		; 111111111000011
		sub			off, 15
		mov			symb, 241
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110001x:
		shl 		bits, 1
		jc		.ac.case11111111100011x
		; 11111111100010x
		shl 		bits, 1
		jc		.ac.case111111111000101x
		; 111111111000100x
		shl 		bits, 1
		jc		.ac.case1111111110001001x
		; 1111111110001000x
		; 1111111110001000
		sub			off, 16
		mov			symb, 23
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110001001x:
		; 1111111110001001
		sub			off, 16
		mov			symb, 24
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111000101x:
		shl 		bits, 1
		jc		.ac.case1111111110001011x
		; 1111111110001010x
		; 1111111110001010
		sub			off, 16
		mov			symb, 25
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110001011x:
		; 1111111110001011
		sub			off, 16
		mov			symb, 26
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111100011x:
		shl 		bits, 1
		jc		.ac.case111111111000111x
		; 111111111000110x
		shl 		bits, 1
		jc		.ac.case1111111110001101x
		; 1111111110001100x
		; 1111111110001100
		sub			off, 16
		mov			symb, 38
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110001101x:
		; 1111111110001101
		sub			off, 16
		mov			symb, 39
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111000111x:
		shl 		bits, 1
		jc		.ac.case1111111110001111x
		; 1111111110001110x
		; 1111111110001110
		sub			off, 16
		mov			symb, 40
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110001111x:
		; 1111111110001111
		sub			off, 16
		mov			symb, 41
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111001x:
		shl 		bits, 1
		jc		.ac.case1111111110011x
		; 1111111110010x
		shl 		bits, 1
		jc		.ac.case11111111100101x
		; 11111111100100x
		shl 		bits, 1
		jc		.ac.case111111111001001x
		; 111111111001000x
		shl 		bits, 1
		jc		.ac.case1111111110010001x
		; 1111111110010000x
		; 1111111110010000
		sub			off, 16
		mov			symb, 42
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110010001x:
		; 1111111110010001
		sub			off, 16
		mov			symb, 53
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111001001x:
		shl 		bits, 1
		jc		.ac.case1111111110010011x
		; 1111111110010010x
		; 1111111110010010
		sub			off, 16
		mov			symb, 54
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110010011x:
		; 1111111110010011
		sub			off, 16
		mov			symb, 55
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111100101x:
		shl 		bits, 1
		jc		.ac.case111111111001011x
		; 111111111001010x
		shl 		bits, 1
		jc		.ac.case1111111110010101x
		; 1111111110010100x
		; 1111111110010100
		sub			off, 16
		mov			symb, 56
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110010101x:
		; 1111111110010101
		sub			off, 16
		mov			symb, 57
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111001011x:
		shl 		bits, 1
		jc		.ac.case1111111110010111x
		; 1111111110010110x
		; 1111111110010110
		sub			off, 16
		mov			symb, 58
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110010111x:
		; 1111111110010111
		sub			off, 16
		mov			symb, 67
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110011x:
		shl 		bits, 1
		jc		.ac.case11111111100111x
		; 11111111100110x
		shl 		bits, 1
		jc		.ac.case111111111001101x
		; 111111111001100x
		shl 		bits, 1
		jc		.ac.case1111111110011001x
		; 1111111110011000x
		; 1111111110011000
		sub			off, 16
		mov			symb, 68
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110011001x:
		; 1111111110011001
		sub			off, 16
		mov			symb, 69
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111001101x:
		shl 		bits, 1
		jc		.ac.case1111111110011011x
		; 1111111110011010x
		; 1111111110011010
		sub			off, 16
		mov			symb, 70
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110011011x:
		; 1111111110011011
		sub			off, 16
		mov			symb, 71
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111100111x:
		shl 		bits, 1
		jc		.ac.case111111111001111x
		; 111111111001110x
		shl 		bits, 1
		jc		.ac.case1111111110011101x
		; 1111111110011100x
		; 1111111110011100
		sub			off, 16
		mov			symb, 72
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110011101x:
		; 1111111110011101
		sub			off, 16
		mov			symb, 73
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111001111x:
		shl 		bits, 1
		jc		.ac.case1111111110011111x
		; 1111111110011110x
		; 1111111110011110
		sub			off, 16
		mov			symb, 74
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110011111x:
		; 1111111110011111
		sub			off, 16
		mov			symb, 83
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111101x:
		shl 		bits, 1
		jc		.ac.case111111111011x
		; 111111111010x
		shl 		bits, 1
		jc		.ac.case1111111110101x
		; 1111111110100x
		shl 		bits, 1
		jc		.ac.case11111111101001x
		; 11111111101000x
		shl 		bits, 1
		jc		.ac.case111111111010001x
		; 111111111010000x
		shl 		bits, 1
		jc		.ac.case1111111110100001x
		; 1111111110100000x
		; 1111111110100000
		sub			off, 16
		mov			symb, 84
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110100001x:
		; 1111111110100001
		sub			off, 16
		mov			symb, 85
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111010001x:
		shl 		bits, 1
		jc		.ac.case1111111110100011x
		; 1111111110100010x
		; 1111111110100010
		sub			off, 16
		mov			symb, 86
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110100011x:
		; 1111111110100011
		sub			off, 16
		mov			symb, 87
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111101001x:
		shl 		bits, 1
		jc		.ac.case111111111010011x
		; 111111111010010x
		shl 		bits, 1
		jc		.ac.case1111111110100101x
		; 1111111110100100x
		; 1111111110100100
		sub			off, 16
		mov			symb, 88
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110100101x:
		; 1111111110100101
		sub			off, 16
		mov			symb, 89
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111010011x:
		shl 		bits, 1
		jc		.ac.case1111111110100111x
		; 1111111110100110x
		; 1111111110100110
		sub			off, 16
		mov			symb, 90
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110100111x:
		; 1111111110100111
		sub			off, 16
		mov			symb, 99
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110101x:
		shl 		bits, 1
		jc		.ac.case11111111101011x
		; 11111111101010x
		shl 		bits, 1
		jc		.ac.case111111111010101x
		; 111111111010100x
		shl 		bits, 1
		jc		.ac.case1111111110101001x
		; 1111111110101000x
		; 1111111110101000
		sub			off, 16
		mov			symb, 100
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110101001x:
		; 1111111110101001
		sub			off, 16
		mov			symb, 101
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111010101x:
		shl 		bits, 1
		jc		.ac.case1111111110101011x
		; 1111111110101010x
		; 1111111110101010
		sub			off, 16
		mov			symb, 102
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110101011x:
		; 1111111110101011
		sub			off, 16
		mov			symb, 103
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111101011x:
		shl 		bits, 1
		jc		.ac.case111111111010111x
		; 111111111010110x
		shl 		bits, 1
		jc		.ac.case1111111110101101x
		; 1111111110101100x
		; 1111111110101100
		sub			off, 16
		mov			symb, 104
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110101101x:
		; 1111111110101101
		sub			off, 16
		mov			symb, 105
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111010111x:
		shl 		bits, 1
		jc		.ac.case1111111110101111x
		; 1111111110101110x
		; 1111111110101110
		sub			off, 16
		mov			symb, 106
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110101111x:
		; 1111111110101111
		sub			off, 16
		mov			symb, 115
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111011x:
		shl 		bits, 1
		jc		.ac.case1111111110111x
		; 1111111110110x
		shl 		bits, 1
		jc		.ac.case11111111101101x
		; 11111111101100x
		shl 		bits, 1
		jc		.ac.case111111111011001x
		; 111111111011000x
		shl 		bits, 1
		jc		.ac.case1111111110110001x
		; 1111111110110000x
		; 1111111110110000
		sub			off, 16
		mov			symb, 116
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110110001x:
		; 1111111110110001
		sub			off, 16
		mov			symb, 117
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111011001x:
		shl 		bits, 1
		jc		.ac.case1111111110110011x
		; 1111111110110010x
		; 1111111110110010
		sub			off, 16
		mov			symb, 118
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110110011x:
		; 1111111110110011
		sub			off, 16
		mov			symb, 119
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111101101x:
		shl 		bits, 1
		jc		.ac.case111111111011011x
		; 111111111011010x
		shl 		bits, 1
		jc		.ac.case1111111110110101x
		; 1111111110110100x
		; 1111111110110100
		sub			off, 16
		mov			symb, 120
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110110101x:
		; 1111111110110101
		sub			off, 16
		mov			symb, 121
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111011011x:
		shl 		bits, 1
		jc		.ac.case1111111110110111x
		; 1111111110110110x
		; 1111111110110110
		sub			off, 16
		mov			symb, 122
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110110111x:
		; 1111111110110111
		sub			off, 16
		mov			symb, 130
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110111x:
		shl 		bits, 1
		jc		.ac.case11111111101111x
		; 11111111101110x
		shl 		bits, 1
		jc		.ac.case111111111011101x
		; 111111111011100x
		shl 		bits, 1
		jc		.ac.case1111111110111001x
		; 1111111110111000x
		; 1111111110111000
		sub			off, 16
		mov			symb, 131
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110111001x:
		; 1111111110111001
		sub			off, 16
		mov			symb, 132
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111011101x:
		shl 		bits, 1
		jc		.ac.case1111111110111011x
		; 1111111110111010x
		; 1111111110111010
		sub			off, 16
		mov			symb, 133
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110111011x:
		; 1111111110111011
		sub			off, 16
		mov			symb, 134
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111101111x:
		shl 		bits, 1
		jc		.ac.case111111111011111x
		; 111111111011110x
		shl 		bits, 1
		jc		.ac.case1111111110111101x
		; 1111111110111100x
		; 1111111110111100
		sub			off, 16
		mov			symb, 135
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110111101x:
		; 1111111110111101
		sub			off, 16
		mov			symb, 136
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111011111x:
		shl 		bits, 1
		jc		.ac.case1111111110111111x
		; 1111111110111110x
		; 1111111110111110
		sub			off, 16
		mov			symb, 137
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111110111111x:
		; 1111111110111111
		sub			off, 16
		mov			symb, 138
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111x:
		shl 		bits, 1
		jc		.ac.case11111111111x
		; 11111111110x
		shl 		bits, 1
		jc		.ac.case111111111101x
		; 111111111100x
		shl 		bits, 1
		jc		.ac.case1111111111001x
		; 1111111111000x
		shl 		bits, 1
		jc		.ac.case11111111110001x
		; 11111111110000x
		shl 		bits, 1
		jc		.ac.case111111111100001x
		; 111111111100000x
		shl 		bits, 1
		jc		.ac.case1111111111000001x
		; 1111111111000000x
		; 1111111111000000
		sub			off, 16
		mov			symb, 146
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111000001x:
		; 1111111111000001
		sub			off, 16
		mov			symb, 147
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111100001x:
		shl 		bits, 1
		jc		.ac.case1111111111000011x
		; 1111111111000010x
		; 1111111111000010
		sub			off, 16
		mov			symb, 148
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111000011x:
		; 1111111111000011
		sub			off, 16
		mov			symb, 149
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111110001x:
		shl 		bits, 1
		jc		.ac.case111111111100011x
		; 111111111100010x
		shl 		bits, 1
		jc		.ac.case1111111111000101x
		; 1111111111000100x
		; 1111111111000100
		sub			off, 16
		mov			symb, 150
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111000101x:
		; 1111111111000101
		sub			off, 16
		mov			symb, 151
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111100011x:
		shl 		bits, 1
		jc		.ac.case1111111111000111x
		; 1111111111000110x
		; 1111111111000110
		sub			off, 16
		mov			symb, 152
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111000111x:
		; 1111111111000111
		sub			off, 16
		mov			symb, 153
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111001x:
		shl 		bits, 1
		jc		.ac.case11111111110011x
		; 11111111110010x
		shl 		bits, 1
		jc		.ac.case111111111100101x
		; 111111111100100x
		shl 		bits, 1
		jc		.ac.case1111111111001001x
		; 1111111111001000x
		; 1111111111001000
		sub			off, 16
		mov			symb, 154
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111001001x:
		; 1111111111001001
		sub			off, 16
		mov			symb, 162
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111100101x:
		shl 		bits, 1
		jc		.ac.case1111111111001011x
		; 1111111111001010x
		; 1111111111001010
		sub			off, 16
		mov			symb, 163
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111001011x:
		; 1111111111001011
		sub			off, 16
		mov			symb, 164
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111110011x:
		shl 		bits, 1
		jc		.ac.case111111111100111x
		; 111111111100110x
		shl 		bits, 1
		jc		.ac.case1111111111001101x
		; 1111111111001100x
		; 1111111111001100
		sub			off, 16
		mov			symb, 165
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111001101x:
		; 1111111111001101
		sub			off, 16
		mov			symb, 166
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111100111x:
		shl 		bits, 1
		jc		.ac.case1111111111001111x
		; 1111111111001110x
		; 1111111111001110
		sub			off, 16
		mov			symb, 167
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111001111x:
		; 1111111111001111
		sub			off, 16
		mov			symb, 168
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111101x:
		shl 		bits, 1
		jc		.ac.case1111111111011x
		; 1111111111010x
		shl 		bits, 1
		jc		.ac.case11111111110101x
		; 11111111110100x
		shl 		bits, 1
		jc		.ac.case111111111101001x
		; 111111111101000x
		shl 		bits, 1
		jc		.ac.case1111111111010001x
		; 1111111111010000x
		; 1111111111010000
		sub			off, 16
		mov			symb, 169
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111010001x:
		; 1111111111010001
		sub			off, 16
		mov			symb, 170
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111101001x:
		shl 		bits, 1
		jc		.ac.case1111111111010011x
		; 1111111111010010x
		; 1111111111010010
		sub			off, 16
		mov			symb, 178
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111010011x:
		; 1111111111010011
		sub			off, 16
		mov			symb, 179
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111110101x:
		shl 		bits, 1
		jc		.ac.case111111111101011x
		; 111111111101010x
		shl 		bits, 1
		jc		.ac.case1111111111010101x
		; 1111111111010100x
		; 1111111111010100
		sub			off, 16
		mov			symb, 180
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111010101x:
		; 1111111111010101
		sub			off, 16
		mov			symb, 181
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111101011x:
		shl 		bits, 1
		jc		.ac.case1111111111010111x
		; 1111111111010110x
		; 1111111111010110
		sub			off, 16
		mov			symb, 182
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111010111x:
		; 1111111111010111
		sub			off, 16
		mov			symb, 183
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111011x:
		shl 		bits, 1
		jc		.ac.case11111111110111x
		; 11111111110110x
		shl 		bits, 1
		jc		.ac.case111111111101101x
		; 111111111101100x
		shl 		bits, 1
		jc		.ac.case1111111111011001x
		; 1111111111011000x
		; 1111111111011000
		sub			off, 16
		mov			symb, 184
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111011001x:
		; 1111111111011001
		sub			off, 16
		mov			symb, 185
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111101101x:
		shl 		bits, 1
		jc		.ac.case1111111111011011x
		; 1111111111011010x
		; 1111111111011010
		sub			off, 16
		mov			symb, 186
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111011011x:
		; 1111111111011011
		sub			off, 16
		mov			symb, 194
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111110111x:
		shl 		bits, 1
		jc		.ac.case111111111101111x
		; 111111111101110x
		shl 		bits, 1
		jc		.ac.case1111111111011101x
		; 1111111111011100x
		; 1111111111011100
		sub			off, 16
		mov			symb, 195
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111011101x:
		; 1111111111011101
		sub			off, 16
		mov			symb, 196
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111101111x:
		shl 		bits, 1
		jc		.ac.case1111111111011111x
		; 1111111111011110x
		; 1111111111011110
		sub			off, 16
		mov			symb, 197
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111011111x:
		; 1111111111011111
		sub			off, 16
		mov			symb, 198
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111111x:
		shl 		bits, 1
		jc		.ac.case111111111111x
		; 111111111110x
		shl 		bits, 1
		jc		.ac.case1111111111101x
		; 1111111111100x
		shl 		bits, 1
		jc		.ac.case11111111111001x
		; 11111111111000x
		shl 		bits, 1
		jc		.ac.case111111111110001x
		; 111111111110000x
		shl 		bits, 1
		jc		.ac.case1111111111100001x
		; 1111111111100000x
		; 1111111111100000
		sub			off, 16
		mov			symb, 199
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111100001x:
		; 1111111111100001
		sub			off, 16
		mov			symb, 200
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111110001x:
		shl 		bits, 1
		jc		.ac.case1111111111100011x
		; 1111111111100010x
		; 1111111111100010
		sub			off, 16
		mov			symb, 201
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111100011x:
		; 1111111111100011
		sub			off, 16
		mov			symb, 202
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111111001x:
		shl 		bits, 1
		jc		.ac.case111111111110011x
		; 111111111110010x
		shl 		bits, 1
		jc		.ac.case1111111111100101x
		; 1111111111100100x
		; 1111111111100100
		sub			off, 16
		mov			symb, 210
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111100101x:
		; 1111111111100101
		sub			off, 16
		mov			symb, 211
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111110011x:
		shl 		bits, 1
		jc		.ac.case1111111111100111x
		; 1111111111100110x
		; 1111111111100110
		sub			off, 16
		mov			symb, 212
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111100111x:
		; 1111111111100111
		sub			off, 16
		mov			symb, 213
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111101x:
		shl 		bits, 1
		jc		.ac.case11111111111011x
		; 11111111111010x
		shl 		bits, 1
		jc		.ac.case111111111110101x
		; 111111111110100x
		shl 		bits, 1
		jc		.ac.case1111111111101001x
		; 1111111111101000x
		; 1111111111101000
		sub			off, 16
		mov			symb, 214
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111101001x:
		; 1111111111101001
		sub			off, 16
		mov			symb, 215
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111110101x:
		shl 		bits, 1
		jc		.ac.case1111111111101011x
		; 1111111111101010x
		; 1111111111101010
		sub			off, 16
		mov			symb, 216
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111101011x:
		; 1111111111101011
		sub			off, 16
		mov			symb, 217
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111111011x:
		shl 		bits, 1
		jc		.ac.case111111111110111x
		; 111111111110110x
		shl 		bits, 1
		jc		.ac.case1111111111101101x
		; 1111111111101100x
		; 1111111111101100
		sub			off, 16
		mov			symb, 218
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111101101x:
		; 1111111111101101
		sub			off, 16
		mov			symb, 226
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111110111x:
		shl 		bits, 1
		jc		.ac.case1111111111101111x
		; 1111111111101110x
		; 1111111111101110
		sub			off, 16
		mov			symb, 227
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111101111x:
		; 1111111111101111
		sub			off, 16
		mov			symb, 228
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111111x:
		shl 		bits, 1
		jc		.ac.case1111111111111x
		; 1111111111110x
		shl 		bits, 1
		jc		.ac.case11111111111101x
		; 11111111111100x
		shl 		bits, 1
		jc		.ac.case111111111111001x
		; 111111111111000x
		shl 		bits, 1
		jc		.ac.case1111111111110001x
		; 1111111111110000x
		; 1111111111110000
		sub			off, 16
		mov			symb, 229
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111110001x:
		; 1111111111110001
		sub			off, 16
		mov			symb, 230
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111111001x:
		shl 		bits, 1
		jc		.ac.case1111111111110011x
		; 1111111111110010x
		; 1111111111110010
		sub			off, 16
		mov			symb, 231
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111110011x:
		; 1111111111110011
		sub			off, 16
		mov			symb, 232
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111111101x:
		shl 		bits, 1
		jc		.ac.case111111111111011x
		; 111111111111010x
		shl 		bits, 1
		jc		.ac.case1111111111110101x
		; 1111111111110100x
		; 1111111111110100
		sub			off, 16
		mov			symb, 233
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111110101x:
		; 1111111111110101
		sub			off, 16
		mov			symb, 234
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111111011x:
		shl 		bits, 1
		jc		.ac.case1111111111110111x
		; 1111111111110110x
		; 1111111111110110
		sub			off, 16
		mov			symb, 242
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111110111x:
		; 1111111111110111
		sub			off, 16
		mov			symb, 243
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111111x:
		shl 		bits, 1
		jc		.ac.case11111111111111x
		; 11111111111110x
		shl 		bits, 1
		jc		.ac.case111111111111101x
		; 111111111111100x
		shl 		bits, 1
		jc		.ac.case1111111111111001x
		; 1111111111111000x
		; 1111111111111000
		sub			off, 16
		mov			symb, 244
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111111001x:
		; 1111111111111001
		sub			off, 16
		mov			symb, 245
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111111101x:
		shl 		bits, 1
		jc		.ac.case1111111111111011x
		; 1111111111111010x
		; 1111111111111010
		sub			off, 16
		mov			symb, 246
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111111011x:
		; 1111111111111011
		sub			off, 16
		mov			symb, 247
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case11111111111111x:
		shl 		bits, 1
		jc		.ac.case111111111111111x
		; 111111111111110x
		shl 		bits, 1
		jc		.ac.case1111111111111101x
		; 1111111111111100x
		; 1111111111111100
		sub			off, 16
		mov			symb, 248
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case1111111111111101x:
		; 1111111111111101
		sub			off, 16
		mov			symb, 249
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;
.ac.case111111111111111x:
		shl 		bits, 1
		jc		.ac.caseerr
		; 1111111111111110x
		; 1111111111111110
		sub			off, 16
		mov			symb, 250
		jmp			.donehacc
		;;;;;;;;;;;;;;;;;;;;;;;;;;;


.ac.caseerr:
				sub			off, 16
				jmp			.herr
				;
				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
