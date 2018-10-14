# gcc media5-3.s -o media5-3	-no-pie	; ./media Para compilar y ejercutar
# gcc -g media5-3.s -o media5-3 -no-pie para compilar con informacion de depuracion

.section .data
	.macro linea
		 # .int -1, -1, -1, -1
		 # .int 0x0fffffff, 0x0fffffff, 0x0fffffff, 0x0fffffff
		 # .int 0x10000000, 0x10000000, 0x10000000, 0x10000000
		 # .int 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	.endm
lista: .irpc i,1234
linea
.endr
longlista:	.int   (.-lista)/4
resultado:	.quad   0
  formato: 	.asciz	"suma = %ld = 0x%lx hex\n"

.section .text
main: .global  main

	mov     $lista, %rbx
	mov  longlista, %ecx
	call suma		# == suma(&lista, longlista);
	mov  %eax, resultado
	mov  %edx, resultado+4
	
	mov   $formato, %rdi
	mov   resultado,%rsi
	mov   resultado,%rdx
	mov          $0,%eax	# varargin sin xmm
	call  printf		# == printf(formato, res, res);

	mov  $0, %edi
	call _exit		# ==  exit(resultado)
	ret

suma:
	push     %rsi
	mov  $0, %eax
	mov  $0, %edx
	mov  $0, %ebp
	mov  $0, %edi
	mov  $0, %rsi

bucle:
	mov  (%rbx,%rsi,4), %eax
	cdq
	add  %eax, %ebp
	adc  %edx, %edi
	inc  %rsi
	cmp  %rsi,%rcx
	jne    bucle
	mov %ebp, %eax
	mov %edi, %edx

	pop   %rsi
	ret
