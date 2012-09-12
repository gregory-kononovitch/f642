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
;
global sinta050f:	function


SECTION .data
;
ZEROpf		dd		0.0, 0.0, 0.0, 0.0
ONEpi		dd		1, 1, 1, 1
ONEpf		dd		1.0, 1.0, 1.0, 1.0
FOURpf		dd		4.0, 4.0, 4.0, 4.0
FLUSHf		dd		0.0, 1.0, 2.0, 3.0

; 1/2Pi =f 0.15915494
I2PIf	dd	+0.15915494, +0.15915494, +0.15915494, +0.15915494
EPI2f	dd	+1.5707964, +1.5707964, +1.5707964, +1.5707964
EPIf	dd	+3.1415927, +3.1415927, +3.1415927, +3.1415927
E3PI2f	dd	+4.712389, +4.712389, +4.712389, +4.712389
E2PIf	dd	+6.2831855, +6.2831855, +6.2831855, +6.2831855
; ~sinf
SIN0f	dd	-1.7730452E-8
sin1	dd	+1.0000014
sin2	dd	-1.9113168E-5
sin3	dd	-0.166571
SIN4f	dd	-2.3207942E-4
sin5	dd	+0.00863213
sin6	dd	-2.0244099E-4
sin7	dd	-1.3791343E-4

; float
ABSpf		dd		0x7F7F7F7F, 0x7F7F7F7F, 0x7F7F7F7F, 0x7F7F7F7F
NEGpf		dd		0x80808080, 0x80808080, 0x80808080, 0x80808080



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
; double cossina050(double rad, double *res)
cossina0500:
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
; void cossina050(double rad, double *res)
cossina050:
		movsd			[rsp - 16], xmm0
		fld				qword [rsp - 16]
		fsincos
		fstp			qword [rdi]
		fstp			qword [rdi + 8]
		ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; float cossina050f(float rad, float *res)
cossina050f0:
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
; float cossina050f(float rad, float *res)
cossina050f:
		movss			dword [rbp - 4], xmm0
		fld				dword [rbp - 4]
		fsincos
		fstp			dword [rdi]
		fstp			dword dword [rdi + 4]
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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; @@@
radius90:
			andps			xmm0, oword [ABSpf]
			movaps			xmm1, xmm0
			mulps			xmm1, oword [I2PIf]
			cvttps2dq		xmm1, xmm1
			cvtdq2ps		xmm1, xmm1
			mulps			xmm1, oword [E2PIf]
			subps			xmm0, xmm1
			subps			xmm0, oword [EPIf]
			;
			movaps			oword [rcx], xmm0
			ucomiss			xmm0, dword [EPI2f]


			ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sinta050f:
			mov				eax, dword [ONEpf]
			mov				dword [rsp - 24], eax
			movss			dword [rsp - 20], xmm0
			movss			xmm4, xmm0
			mulss			xmm4, xmm0
			movss			dword [rsp - 16], xmm4
			mulss			xmm4, xmm0
			movss			dword [rsp - 12], xmm4
			mulss			xmm4, xmm0
			movss			dword [rsp - 40], xmm4
			movss			dword [rsp - 36], xmm4
			movss			dword [rsp - 32], xmm4
			movss			dword [rsp - 28], xmm4
			;
			movaps			xmm0, oword [rsp - 24]
			mulps			xmm0, oword [SIN0f]
			movaps			xmm4, oword [rsp - 24]
			mulps			xmm4, oword [rsp - 40]
			mulps			xmm4, oword [SIN4f]
			addps			xmm0, xmm4
			movaps			oword [rsp - 40], xmm0
			addss			xmm0, dword[rsp - 36]
			addss			xmm0, dword[rsp - 32]
			addss			xmm0, dword[rsp - 28]

			ret

; float sinta050f(float rad, float *res)
sina050f:
		movss			dword [rsp - 16], xmm0
		fld				dword [rsp - 16]
		fsin
		fstp			dword [rsp - 16]
		movss			xmm0, dword [rsp - 16]
		ret
