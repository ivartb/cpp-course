EXTERN printf
EXTERN scanf

SECTION .rodata
frmt: db "%s",0
frmt_ch db "%c",0x0D,0x0A,0

SECTION .text
global main

main:
	push rbp
	mov rbp, rsp
	sub rsp, 2 * 128 * 8; malloc

	mov rcx, rsp
	mov rdx, 128

	call read_long

	mov r8, rcx; now first multiplier in r8
	add rcx, 128*8

	call read_long

	;second multiplier in rcx, ready to mul and write

	call mul_long_long

	call write_long

	mov rsp,rbp
	pop rbp

	ret
	
; Multiplies long number by a short and moves it
;    rcx -- address of multiplier #1 (long number)
;    rdx -- length of long number in qwords
;    r8 -- multiplier #2 (64-bit unsigned)
; 	 r9 -- address for result
; result:
;    product is written to r9
;destroys:
;	r10,r11
mul_mov_long_short:
	push r9
	push rax
	push rdx
	push rcx

	xor r10,r10 ; Using r10 for carry;
	mov r11,rdx ; We will use rdx in mul, so r11 is length now.

.loop:
	mov rax, [rcx]; load rcx to rax
	mul r8; rax = rax * r8
	add rax,r10; adds carry
	adc rdx,0; increments length of multiplier by carry
	mov [r9], rax; store rax to r9
	add rcx,8; shift rcx
	add r9,8; shift r9
	mov r10,rdx; set carry
	dec r11; decrement length
	jnz .loop

	pop rcx
	pop rdx
	pop rax
	pop r9

	ret


;Shift lefts long number
; Args are not in standart registers, for usability in calling code
;	r9 - location
;	rdx - length
;	rdi - amount of shift
; destroys:
;	rax
shift_left_long:
	push r9
	push rdx
	push rdi
	push r15
	push r14

	lea r9, [r9 + 8*rdx - 8]; Go to number's end.
	mov r15,rdx
	sub r15,rdi; r15 - number of significant blocks

.loop:
	;mov rax,[r9 - 8 * rdi] Unfortunately, we can't do this command
	mov rax,r9
	mov r14, rdi; r14 = rdi
	shl r14,3; r14 = 8 * rdi
	sub rax,r14; rax = r9 - 8 * rdi
	mov rax,[rax]

	mov [r9], rax
	sub r9,8
	dec r15
	jnz .loop

	;An issue for zero shift.
	test rdi,rdi
	jz .end
	
	;Fill the rest with zeros
.loop2:
	mov qword [r9],0
	sub r9,8
	dec rdi
	jnz .loop2

.end:
	pop r14
	pop r15
	pop rdi
	pop rdx
	pop r9

	ret


;Moves long number
;	rcx - source
;	rdx - length
;	rsi - destination
;destroys:
;	rax
mov_long_long:
	push rcx
	push rdx
	push rsi

.loop:
	mov rax,[rcx]; load rcx to rax
	mov [rsi],rax; store rax to rsi
	add rcx,8; shift rcx
	add rsi,8; shift rsi
	dec rdx
	jnz .loop

	pop rsi
	pop rdx
	pop rsi

	ret

;Multiplies two long numbers
;	rcx -- address of multiplier #1 (long number)
;	rdx - length of long numbers in qwords (128)
;	r8 -- address of multiplier #2 (long number)
;Result written to rcx
mul_long_long:
	push rcx
	push rdx
	push r8
	push rsi
	push rdi
	push r9
	push r15
	push rbp

	mov rbp, rsp
	sub rsp, 2 * 8 * 128

	mov rsi, rsp; rsi - accumulator
	push rcx
	mov rcx,rsi
	call set_zero
	pop rcx

	lea r9, [rsp + 8 * 128]; r9 - current multiplication result
	xor rdi,rdi; Shift amount
	mov r15,rdx; Actual length

.loop:
	push r8
	lea r8, [r8 + rdi * 8]
	mov r8,[r8]
	call mul_mov_long_short; r9 = rcx * r8[i]
	pop r8

	call shift_left_long; rdx = r9
	inc rdi; increment shift

	push rcx
	push r8

	mov rcx,rsi
	mov r8,r9

	call add_long_long; rcx = rcx + r8

	pop r8
	pop rcx

	dec r15; decrement length
	jnz .loop; while length != 0

	;Finally, we need to move result from rsi to rcx
	xchg rcx,rsi
	call mov_long_long

	mov rsp,rbp ; Clean memory and restore registers

	pop rbp
	pop r15
	pop r9
	pop rdi
	pop rsi
	pop r8
	pop rdx
	pop rcx

	ret
	
; adds two long number
;    rcx -- address of summand #1 (long number)
;    rdx -- length of long numbers in qwords
;    r8 -- address of summand #2 (long number)
; result:
;    sum is written to rcx
; destroys:
; RAX
add_long_long:
	push rcx
	push rdx
	push r8

	clc;clear carry flag

.loop:
	mov rax,[r8]; load r8 to rax
	adc [rcx],rax; add with ca
	lea rcx,[rcx+8]; shift rcx
	lea r8,[r8+8]; shift r8
	dec rdx; decrement length of summand
	jnz .loop; if (length != 0) goto loop ; actually this is for-loop

	pop r8
	pop rdx
	pop rcx

	ret

;Cheks a zero in number
;	RCX - location
;	RDX - length in qwords
check_zero:
	push rdx
	push rcx
	push rax

.loop:
	mov rax, [rcx]; load rcx to rax
	test rax,rax; if (rax == 0) set ZF to 1
	jnz .end_loop
	add rcx, 8; shift rcx
	dec rdx
	jnz .loop
	;ZF already 1/0
.end_loop:
	pop rax
	pop rcx
	pop rdx
	ret

;Sets a zero in number
;	RCX - location
;	RDX - length in qwords
set_zero:
	push rdx
	push rcx

.loop:
	mov qword [rcx], 0; initialise 8 bytes of rcx with 0
	add rcx, 8; shift rcx
	dec rdx
	jnz .loop

.end_loop:
	pop rcx
	pop rdx
	ret


;Multiplies long number by a short
;	rcx - address of multiplier #1 (long number)
;	rdx - length of long number in qwords
; 	r8 - multiplier #2 (64-bit unsigned)
; result:
;   product is written to rcx
;destroys:
;	r10,r11
;You can notice that Long numbers are in Little Endian mode.
mul_long_short:
	push rax
	push rdx
	push rcx

	xor r10,r10 ; Using r10 for carry;
	mov r11,rdx ; We will use rdx in mul, so r11 is length now.

.loop:
	mov rax, [rcx]; load rcx to rax
	mul r8; rax = rax * r8
	add rax,r10; adds carry
	adc rdx,0; increments length of multiplier by carry
	mov [rcx], rax; store rax to rcx
	add rcx,8; shift rcx
	mov r10,rdx; set carry
	dec r11; decrement length
	jnz .loop

	pop rcx
	pop rdx
	pop rax

	ret

; Add 64-bit number to long number
;    rcx -- address of summand #1 (long number)
;    rdx -- length of long number in qwords
;    r8 -- summand #2 (64-bit unsigned)
; result:
;    sum is written to rcx
add_long_short:
	push rax
	push rdx
	push rcx
	push r8

	xor rax,rax ; Using for carry

.loop:
	add [rcx], r8; rcx = rcx + r8
	adc rax,0; add carry to rax
	mov r8,rax; put carry to r8
	xor rax,rax; rax = 0
	add rcx,8; shift rcx
	dec rdx; decrement length
	jnz .loop

	pop r8
	pop rcx
	pop rdx
	pop rax

	ret


; divides long number by a short
;    rcx -- address of dividend (long number)
;    rdx -- length of long number in qwords
;    r8 -- divisor (64-bit unsigned)
; result:
;    quotient is written to rcx
;    rax -- remainder
;destroys:
; 	R9
div_long_short:
	push rcx
	push rdx

	lea rcx, [rcx + 8*rdx - 8]; Go to number's end.
	mov r9,rdx; R9 is actual number's length
	xor rdx,rdx; rdx is remainder now

.loop:
	mov rax, [rcx]; load rcx to rax
	div r8; rax = rax / r8
	mov [rcx], rax; store rax to rcx
	sub rcx,8; shift rcx
	dec r9; decrement length
	jnz .loop

	mov rax,rdx ;move remainder to RAX

	pop rdx
	pop rcx

	ret



;Reading long 128*8-byte number from stdin
;Args:
;RCX - location(reference for output)
;RDX - length in qwords (actually, 128)
read_long:
	push r8
	push rdx
	push rcx
	push rsi; Use rsi as address for string
	push rax; AL for char
	sub rsp, 128 * 8; Place for string
	mov rsi,rsp

	call set_zero; rcx = 0


	push rcx; We need this registers to pass args to scanf
	push rdx

	sub rsp, 0x28 ; stack alignment due to API
	mov rcx,frmt
	mov rdx,rsi; Our string
	call scanf
	add rsp,0x28

	pop rdx
	pop rcx

	xor rax,rax; Storage for current char
.loop:
	mov al, [rsi]; Load char
	cmp al,0x0D ; Carriage Return
	je .done
	cmp al, 0x0A ; Line Feed
	je .done
	cmp al, 0x0 ; Null-terminator
	je .done
	; Assuming we got a valid char

	sub al,'0'
	mov r8,10
	call mul_long_short
	mov r8,rax
	call add_long_short

	inc rsi
	jmp .loop

.done:
	add rsp, 128*8 ; Restore memory back
	pop rax
	pop rsi
	pop rcx
	pop rdx
	pop r8

	ret


;Writing long 128*8-byte number to stdout
;Args:
;RCX - location(reference for output)
;RDX - length in qwords (actually, 128)
write_long:
	push r8
	push rbp
	push rdx
	push rcx
	push rsi ;As address for string
	push r12; because rdx uses in multiply, we'll store length in r12.

	mov r12, rdx

	mov rax,20 ; allocate 20*length memory, because 2^64 ~ 1e19
	mul r12
	mov rbp,rsp
	sub rsp,rax

	mov rdx, r12; Restore
	mov rsi,rbp

	dec rsi
	mov rax, 0x0
	mov [rsi], al ; Adding null-terminator

.loop:
	mov  r8,10
	call div_long_short
	add rax, '0'

	dec rsi
	mov [rsi], al
	call check_zero
	jnz .loop

	; Now we need just to print

	push rdx
	push rcx

	sub rsp,0x28
	mov rcx,frmt
	mov rdx,rsi
	call printf
	add rsp, 0x28

	pop rcx
	pop rdx

	; Free memory, restore stack

	mov rsp,rbp
	pop r12
	pop rsi
	pop rcx
	pop rdx
	pop rbp
	pop r8

	ret