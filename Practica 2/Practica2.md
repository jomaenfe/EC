# Práctica 2: Programación en ensamblador x86-64 Linux

En esta práctica el trabajo que tenemos que hacer es mejorar un programa que se nos da en el guión de prácticas. Este se encuentra en un archivo llamada suma.s y el trabajo se divide en varias partes.

### Ejercicio 5.1: Sumar N enteros sin signo de 32 bits sobre dos registros de 32 bits usando uno de ellos como acumulador de acarreos (N≈16)

En esta primera parte, lo que queremos conseguir es que el programa sea capaz de realizar sumas de forma correcta con dos registros de 32 bits. Para lograrlo debemos entender primeramente el concepto del acarreo en una suma binaria. Una vez tenemos claro el concepto de acarreo nos empezamos a plantear el programa en sí. Para que funcione debemos distinguir en algunos casos:

1. Debemos tener dos registros, un acumulador para la suma y otro para el acarreo. Estos registros serán %eax y %edx.
2. Debemos distinguir entre dos situaciones. Si se produce acarreo o si no se produce para incrementar o no el registro %edx.
3. Debemos combinar los dos registros para obtener una solución correcta.

Para realizar el primer paso, lo que hemos hecho ha sido añadir estos dos registros dentro de la etiqueta "suma" y los hemos inicializado con un valor 0. En el segundo paso hemos usado la instrucción "jnc" que lo que hace es saltar a la posición de memoria que se le indica cuando el bit de acarreo no está activo. Cuando este bit está activo lo que hace es saltar a la siguiente isntrucción. Y, por último, para solventar el tercer paso lo que he hecho ha sido mover el contenido de %eax y %edx a una variable de tamaño quad llamada resultado e imprimirla.

En el código que se muestra a continuación se pueden observar todos los cambios anteriormente descritos y en la misma carpeta de este archivo se encuentra el archivo llamado "media5-1.s" donde se encuentra el código.


```
 # gcc media5-1.s -o media5-1	-no-pie	; ./media Para compilar y ejercutar
 # gcc -g media5-1.s -o media5-1 -no-pie para compilar con informacion de depuracion

.section .data
	.macro linea
		 # .int 1,1,1,1
		 # .int 0x0fffffff, 0x0fffffff, 0x0fffffff, 0x0fffffff
		 # .int 0x10000000, 0x10000000, 0x10000000, 0x10000000
		 # .int 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	.endm
lista: .irpc i,1234
linea
.endr
longlista:	.int   (.-lista)/4
resultado:	.quad   0
  formato: 	.asciz	"suma = %lu = 0x%lx hex\n"

.section .text
main: .global  main

	mov     $lista, %rbx
	mov  longlista, %ecx
	call suma		# == suma(&lista, longlista);
	mov  %eax, resultado
	mov %edx , resultado+4
	
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
	mov  $0, %rsi

bucle:
	add  (%rbx,%rsi,4), %eax
	jnc  Acarreo
	inc  %edx

Acarreo:
	inc   %rsi
	cmp   %rsi,%rcx
	jne    bucle

	pop   %rsi
	ret

```


### Ejercicio 5.2: Sumar N enteros sin signo de 32 bits sobre dos registros de 32 bits mediante extensión con ceros (N≈16)

Para superar este segundo ejercicio nos basta con cambiar algunas lineas del ejercicio anterior. Lo que vamos a intentar mejorar en este caso es la suma del bit de acarreo para que se haga más rápido. Para ello vamos a utilizar la instrucción "adc" que lo que hace es sumar el acarreo además de sumar lo que se le indique. Como se puede ver en el siguiente código, sustituimos la instrucción "jnc" e "inc" por "adc".

```
# gcc media5-2.s -o media5-2	-no-pie	; ./media Para compilar y ejercutar
# gcc -g media5-2.s -o media5-2 -no-pie para compilar con informacion de depuracion

.section .data
	.macro linea
		 # .int 1,1,1,1
		 # .int 0x0fffffff, 0x0fffffff, 0x0fffffff, 0x0fffffff
		 # .int 0x10000000, 0x10000000, 0x10000000, 0x10000000
		 .int 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
	.endm
lista: .irpc i,1234
linea
.endr
longlista:	.int   (.-lista)/4
resultado:	.quad   0
  formato: 	.asciz	"suma = %lu = 0x%lx hex\n"

.section .text
main: .global  main

	mov     $lista, %rbx
	mov  longlista, %ecx
	call suma		# == suma(&lista, longlista);
	mov  %eax, resultado
	mov %edx , resultado+4
	
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
	mov  $0, %rsi

bucle:
	add  (%rbx,%rsi,4), %eax
	adc   $0, %edx	
	inc   %rsi
	cmp   %rsi,%rcx
	jne    bucle

	pop   %rsi
	ret


```

### Ejercicio 5.3: Sumar N enteros con signo de 32 bits sobre dos registros de 32 bits (mediante extensión de signo, naturalmente) (N≈16)

Para poder realizar esta tercera parte de la práctica si que tenemos que cambiar el código un poco más. Esto se debe principalmente a que queremos poder sumar numeros con signo en dos registros de 32 bits. Los principales pasos a seguir son:

1. Tenemos que leer de la lista el elemento que vamos a sumar.
2. Tenemos que hacer una extensión de signo a los registros principales.
3. Debemos sumar el componente de la lista en el registro acumulador.
4. Sumamos la extensión de signo y el acarreo al registro encargado del acarreo.

Para este programa vamos a utilizar los registros %eax y %edx como prinicpales, por lo que será a estos a los que le pongamos la extensión de signo. Como registros acumuladores y encargado del bit de acarreo y del signo vamos a usar %ebp y %edi. 

Para leer de la lista en la variable %eax utilizamos la instrucción "mov", la línea quedaría tal que así: ` mov  (%rbx,%rsi,4), %eax `, la siguiente instrucción será "cdq" para darle la extensión de signo. Con esto tendremos cubiertos los dos primero puntos. Las dos siguientes líneas se encargarán de realizar la suma. Estas son: ` add  %eax, %ebp ` para sumar los elementos de la lista y ` adc  %edx, %edi ` para sumar el acarreo y la extensión de signo. 

El siguiente código corresponde con el programa completo, el archivo llamado "media5-3.s" se corresponde con este mismo código y se encuentra en la misma carpeta.

```
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


```
