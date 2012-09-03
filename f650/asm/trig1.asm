;
; file    : trig1.asm
; project : f640
;
; Created on: Aug 30, 2012
; Author and copyright (C) 2012 : Gregory Kononovitch
; License : GNU Library or "Lesser" General Public License version 3.0 (LGPLv3)
; There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
;

default rel

;
global cosa050:		function
global cosa050f:	function
global sina050:		function
global sina050f:	function
global cossina050:	function
global cossina050f:	function
;
global sqrta050:	function
global sqrta050f:	function


SECTION .data

SECTION .text  align=16

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
cosa050:
;		push			rbp
;		mov				rbp, rsp
		movsd			[rbp - 8], xmm0
		fld				qword [rbp - 8]
		fcos
		fstp			qword [rbp - 8]
		movsd			xmm0, [rbp - 8]
;		pop				rbp
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
cosa050f:
;		push			rbp
;		mov				rbp, rsp
		movss			[rbp - 4], xmm0
		fld				dword [rbp - 4]
		fcos
		fstp			dword [rbp - 4]
		movss			xmm0, [rbp - 4]
;		pop				rbp
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sina050:
;		push			rbp
;		mov				rbp, rsp
		movsd			[rbp - 8], xmm0
		fld				qword [rbp - 8]
		fsin
		fstp			qword [rbp - 8]
		movsd			xmm0, [rbp - 8]
;		pop				rbp
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sina050f:
;		push			rbp
;		mov				rbp, rsp
		movss			[rbp - 4], xmm0
		fld				dword [rbp - 4]
		fsin
		fstp			dword [rbp - 4]
		movss			xmm0, [rbp - 4]
;		pop				rbp
		ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; double cossina050(double rad, double *res)
cossina050:
		movsd			[rbp - 8], xmm0
		fld				qword [rbp - 8]
		fsincos
		fstp			qword [rbp - 8]
		movsd			xmm0, qword [rbp - 8]
		movsd			qword [rdi], xmm0
		fstp			qword [rbp - 8]
		movsd			xmm0, qword [rbp - 8]
		movsd			qword [rdi + 8], xmm0
		ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; float cossina050f(float rad, float *res)
cossina050f:
		movss			dword [rbp - 4], xmm0
		fld				dword [rbp - 4]
		fsincos
		fstp			dword [rbp - 4]
		movss			xmm0, dword [rbp - 4]
		movss			dword [rdi], xmm0
		fstp			dword [rbp - 5]
		movss			xmm0, dword [rbp - 4]
		movss			dword [rdi + 4], xmm0
		ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sqrta050:
		xorpd			xmm10, xmm10
		ucomisd			xmm0, xmm10
		jb				Pb0_1
		movsd			[rbp - 8], xmm0
		fld				qword [rbp - 8]
		fsqrt
		fstp			qword [rbp - 8]
		movsd			xmm0, [rbp - 8]
		ret

Pb0_1:	; < 0
		ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sqrta050f:
;		call			rdi
		xorpd			xmm10, xmm10
		ucomisd			xmm0, xmm10
		jb				Pb0_2
		movsd			[rbp - 8], xmm0
		fld				qword [rbp - 8]
		fsqrt
		fstp			qword [rbp - 8]
		movsd			xmm0, [rbp - 8]
		ret

Pb0_2:	; < 0
		ret