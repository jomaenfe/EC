# Práctica 3: Programación mixta C-asm x86-64 Linux

En esta práctica, el trabajo que vamos a realizar consiste en contar el número de bits activados que contiene una palabra. Para ello lo que vamos a realizar son 10 versiones distintas de un mismo programa. Cada una de ellas estará implementada de forma distinta, en C o combinando C con asm. 

### Diario de trabajo:

En primer lugar la práctica la he ido realizando poco a poco. Me ha costado realizarla aproximadamente una semana para entenderlo todo bien. En los primeros días lo que hice fue comprender bien todo lo que se explica en el tutorial e ir probando cada cosa que se propone. Una vez terminé el tutorial me puse con las versiones que se proponen en el guión. 

### Versiones:

#### Primera versión:

```
int popcount1(unsigned* array, size_t len)
{
    int  i,j, res=0;
    unsigned x;

    for (i=0; i<len; i++){
     x = array[i];
     for(j = 0; j < 8*sizeof(int); j++){
        res += x &0x1;
        x >>=1;
     }   
    }

    return res;
}
```

En esta primera versión lo que se está haciendo es que a cada elemento del array que se pasa por argumento se le aplica una máscara `0x1` y después se le aplica al elemento un desplazamiento a la derecha `x>>=1`. Al plicar la máscara sumamos en la variable res el número de bits activados, y al aplicar el desplazamiento "eliminamos" el último bit de la palabra hasta que deja de tener.

#### Segunda versión:

```
int popcount2(unsigned* array, size_t len)
{
    int  i, res=0;
    unsigned x;

    for (i=0; i<len; i++){
     x = array[i];
     while(x){
        res += x &0x1;
        x >>=1;
     }   
    }

    return res;
}

```

Esta versión es prácticamente igual que la primera, solo que nos ahorramos iteraciones al ser un bucle while. 

#### Tercera versión:

```
int popcount3(unsigned *array, size_t len){
    unsigned x;
    int result = 0;
    
    for (int i = 0; i < len; i++){
        x = array[i];
        
        asm("\n"
        "ini3:  \n\t"               //seguir mientras que x != 0
            "shr %[x]       \n\t"   //LSB en CF
            "adc $0x0, %[r]     \n\t"
            "test %[x], %[x]    \n\t"
            "jnz ini3       \n\t"   //Saltar si es distinto de 0
            : [r] "+r" (result) // e/s: añadir a lo acumulado por el momento
            : [x] "r" (x)           //entrada: valor elemento
        );
    }
    
    return result;
}
```

En esta versión ya introducimos el asm y la programación mixta. Principalmente lo que se está haciendo es que con la instrucción `shr` desplazamos un bit a la derecha, si el bit está activado se activa el flag de acarreo. Entonces, con la instrucción `adc` lo sumamos al resultado para ir contandolos. Después la instrucción `test` se encarga de ver si el bit de la palabra es 0 o no para que vuelva a saltar al inicio del bucle (o no) con la instrucción `jnz`.

#### Cuarta versión:

```
int popcount4(unsigned *array, size_t len){
    unsigned x;
    int result = 0;
    
    for (int i = 0; i < len; i++){
        x = array[i];
        
        asm("\n"
            "clc                    \n\t"
        "ini4:  \n\t"               //seguir mientras que x != 0
            "adc $0x0, %[r]     \n\t"
            "shr %[x]   \n\t"
            "jnz ini4       \n\t"   //Saltar si es distinto de 0
            "adc $0x0, %[r] \n\t"
            : [r] "+r" (result) // e/s: añadir a lo acumulado por el momento
            : [x] "r" (x)           //entrada: valor elemento
        );
    }
    
    return result;
}
```

En esta versión variamos un poco el código. Empezamos utilizando la intrucción `clc` para limpiar el flag de acarreo y usamos `adc` en primer lugar para sumar el primer bit de la palabra. Como la instrucción `shr` también activa el flag cero podemos utilizarlo para condicionar el salto de la intrucción `jnz` y así ahorrarnos una instrucción en el bucle. 

#### Quinta versión:

```
int popcount5(unsigned *array, size_t len){
    int val = 0;
    unsigned  x=0;
    int result = 0;
    for(int j=0; j < len; j++){
        val = 0;
        x = array[j];
        for (int i = 0; i < 8; i++) {
            val += x & 0x0101010101010101L;
            x >>= 1;
        }
        val += (val >> 16);
        val += (val >> 8);
        result += (val & 0xFF);
    }
    
    
    return result;
}
```

En esta quinta versión se nos ocurre la idea de aplicar una forma de sumar a la variable de resultado el total de bits que se encuentra activados en un byte. Para ello, la primera parte del programa es igual que en las versiones anteriores exceptuando la máscara aplicada. El cambio viene en la segunda parte, donde se suma en forma de arbol los bits activados en la variable `val` y después en `result`.

#### Sexta versión:

```
int popcount6(unsigned *array, size_t len){
    const long unsigned m1 = 0x5555555555555555; 
    const long unsigned m2 = 0x3333333333333333; 
    const long unsigned m4 = 0x0f0f0f0f0f0f0f0f; 
    const long unsigned m8 = 0x00ff00ff00ff00ff; 
    const long unsigned m16 = 0x0000ffff0000ffff;
    unsigned x=0;
    int result = 0;
    
    for(int i=0; i < len; i++){
        x = array[i];

        x = (x & m1 ) + ((x >> 1) & m1 ); 
        x = (x & m2 ) + ((x >> 2) & m2 ); 
        x = (x & m4 ) + ((x >> 4) & m4 ); 
        x = (x & m8 ) + ((x >> 8) & m8 ); 
        x = (x & m16) + ((x >> 16) & m16); 

        result += x;
    }
    return result;
}
```

Al ver que los resultados en la quinta versión eran mejores que los de la cuarta versión lo que hacemos ahora es "desenrrollar" completamente el bucle interno. Con esto conseguimos que en una sola iteración se sumen todos los bits de una palabra completa. El cambio en los resultados de tiempo es muy positivo con respecto a la versión anterior.

#### Séptima versión:

int popcount7(unsigned *array, size_t len){
    const unsigned long m1 = 0x5555555555555555; //binary: 0101...
    const unsigned long m2 = 0x3333333333333333; //binary: 00110011..
    const unsigned long m4 = 0x0f0f0f0f0f0f0f0f; //binary: 4 zeros, 4 ones ...
    const unsigned long m8 = 0x00ff00ff00ff00ff; //binary: 8 zeros, 8 ones ...
    const unsigned long m16 = 0x0000ffff0000ffff; //binary: 16 zeros, 16 ones ...

    size_t i;
    unsigned x1,x2;
    int result;

    for(i=0; i < len; i+=2){
        x1 = *(unsigned long*) &array[i ];
        x2 = *(unsigned long*) &array[i+1];

        x1 = (x1 & m1 ) + ((x1 >> 1) & m1 ); //put count of each 2 bits into those 2 bits
        x1 = (x1 & m2 ) + ((x1 >> 2) & m2 ); //put count of each 4 bits into those 4 bits
        x1 = (x1 & m4 ) + ((x1 >> 4) & m4 ); //put count of each 8 bits into those 8 bits
        x1 = (x1 & m8 ) + ((x1 >> 8) & m8 ); //put count of each 16 bits into those 16 bits
        x1 = (x1 & m16) + ((x1 >> 16) & m16); //put count of each 32 bits into those 32 bits


        x2 = (x2 & m1 ) + ((x2 >> 1) & m1 ); //put count of each 2 bits into those 2 bits
        x2 = (x2 & m2 ) + ((x2 >> 2) & m2 ); //put count of each 4 bits into those 4 bits
        x2 = (x2 & m4 ) + ((x2 >> 4) & m4 ); //put count of each 8 bits into those 8 bits
        x2 = (x2 & m8 ) + ((x2 >> 8) & m8 ); //put count of each 16 bits into those 16 bits
        x2 = (x2 & m16) + ((x2 >> 16) & m16); //put count of each 32 bits into those 32 bits

        result+= x1+x2;
    }

    return result;
}

En esta séptima versión lo que se mejora con respecto a la anteior es que sumamos palabras de dos en dos. El bucle sigue "desenrrollado" pero ahora usamos otra variable llamada `x2` y con ella sumamos los bits activados de la siguiente palabra en el array. Esto reduce las iteraciones a la mitad y la mejora de tiempo es positiva también.

#### Octava versión:

```
int popcount8(unsigned* array, int len){
    int i, val=0; 
    int result=0;
    int SSE_mask[] = {0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f};
    int SSE_LUTb[] = {0x02010100, 0x03020201, 0x03020201, 0x04030302};
    
    for (i = 0; i < len; i += 4){
        asm (
            "movdqu     %[x], %%xmm0 \n\t"
            "movdqa   %%xmm0, %%xmm1 \n\t" 
            "movdqu     %[m], %%xmm6 \n\t" 
            "psrlw       $4 , %%xmm1 \n\t"
            "pand     %%xmm6, %%xmm0 \n\t" 
            "pand     %%xmm6, %%xmm1 \n\t" 
            
            "movdqu     %[l], %%xmm2 \n\t" 
            "movdqa   %%xmm2, %%xmm3 \n\t" 
            "pshufb   %%xmm0, %%xmm2 \n\t" 
            "pshufb   %%xmm1, %%xmm3 \n\t" 
            
            "paddb    %%xmm2, %%xmm3 \n\t" 
            "pxor     %%xmm0, %%xmm0 \n\t" 
            "psadbw   %%xmm0, %%xmm3 \n\t" 
            "movhlps  %%xmm3, %%xmm0 \n\t" 
            "paddd    %%xmm3, %%xmm0 \n\t" 
            "movd     %%xmm0, %[val] \n\t"
            :   [val]   "=r" (val)
            :   [x] "m" (array[i]),
                [m] "m" (SSE_mask[0]),
                [l] "m" (SSE_LUTb[0])
        );
        
        result +=val;
    }
    
    return result;
}
```

#### Novena versión: 

```
int popcount9(unsigned* array, size_t len){
    size_t i;
    unsigned x;
    int val, result=0;
    for (i=0; i<len; i++){
        x = array[i];
        asm("popcnt %[x], %[val]"
            : [val] "=r" (val)
            : [x] "r" (x)
        );
        result += val;
    }
    
    return result;
}

```

La penúltima versión propuesta es muy parecida a las dos primeras, sólo que con un par de variaciones. Se usa programación mixta C-asm y usamos la instrucción `popcnt` que se explica en el guión de la práctica. 

#### Décima versión:

```
int popcount10(unsigned* array, size_t len){
    size_t i;
    unsigned long x1,x2;
    long val; long result=0;
    for (i=0; i<len; i+=4) {
        x1 = *(unsigned long*) &array[i ];
        x2 = *(unsigned long*) &array[i+2];
        asm("popcnt %[x1], %[val] \n\t"
            "popcnt %[x2], %%rdi \n\t"
            "add %%rdi, %[val]\n\t"
            : [val]"=&r" (val)
            : [x1] "r" (x1),
            [x2] "r" (x2)
            : "rdi"
            );
        result += val;
    }
    return result;
}
```

Para agilizar más el proceso para contar los bits activados de una palabra, en esta décima versión vamos a realizar lecturas de 64bits en vez de 32. Esto se consigue usando dos veces la instrucción `popcnt` con dos posiciones distintas del array y, además, también debemos usar algunos registros extra.

### Tiempos de ejecución y gráficas:

En la siguiente imagen podemos ver los tiempos de ejecución de cada una de las versiones, también estarán incluidos tiempos de ejecución con diferentes opciones de optimización del compilador.

![Tiempos_ejecucion](https://github.com/jomaenfe/EC/blob/master/Practica%203/Im%C3%A1genes/tiempos.JPG?raw=true)

Con respecto a cada tipo de versión y a cada versión en sí, se han realizado unas gráficas. En la siguiente imagen podemos observarlas. Las diferencias que más resaltan son la gran diferencia de tiempos que se pueden ver entre los bucle for/while, las sumas en árbol y el reportorio multimedia. Partiendo de ese punto, también vemos la diferencia de tiempos que hay cuando el compilador empieza a usar optimización.

![Graficas](https://github.com/jomaenfe/EC/blob/master/Practica%203/Im%C3%A1genes/Gr%C3%A1ficas.JPG?raw=true)



#### Código completo de la práctica:

```
#include <stdio.h>		// para printf()
#include <stdlib.h>		// para exit()
#include <sys/time.h>		// para gettimeofday(), struct timeval

int resultado=0;

#ifndef TEST
#define TEST 5
#endif 

/* -------------------------------------------------------------------- */
#if TEST==1
/* -------------------------------------------------------------------- */
#define SIZE 4
unsigned lista[SIZE]={0x80000000, 0x00400000, 0x00000200, 0x00000001};
#define RESULT 4
/* -------------------------------------------------------------------- */
#elif TEST==2
/* -------------------------------------------------------------------- */
#define SIZE 8
unsigned lista[SIZE]={0x7fffffff, 0xffbfffff, 0xfffffdff, 0xfffffffe,
                     0x01000023, 0x00456700, 0x8900ab00, 0x00cd00ef };
#define RESULT 156                     
/* -------------------------------------------------------------------- */
 #elif TEST==3
/* -------------------------------------------------------------------- */
#define SIZE 8
unsigned lista[SIZE]={0x0, 0x01020408, 0x35906a0c, 0x70b0d0e0,
                     0xffffffff, 0x12345678, 0x9abcdef0, 0xdeadbeef };
#define RESULT 116
/* -------------------------------------------------------------------- */       
#elif TEST==4 || TEST==0
/* -------------------------------------------------------------------- */
#define NBITS 20
#define SIZE (1<<NBITS)
// tamaño suficiente para tiempo apreciable
unsigned lista[SIZE];
// unsigned para desplazamiento derecha lógico
#define RESULT ( NBITS * ( 1 << NBITS-1 ) )
// pistas para deducir fórmula
/* -------------------------------------------------------------------- */
#else
    #error "Definir TEST entre 0..4"
#endif
/* -------------------------------------------------------------------- */

int popcount1(unsigned* array, size_t len)
{
    int  i,j, res=0;
    unsigned x;

    for (i=0; i<len; i++){
     x = array[i];
     for(j = 0; j < 8*sizeof(int); j++){
        res += x &0x1;
        x >>=1;
     }   
    }

    return res;
}

int popcount2(unsigned* array, size_t len)
{
    int  i, res=0;
    unsigned x;

    for (i=0; i<len; i++){
     x = array[i];
     while(x){
        res += x &0x1;
        x >>=1;
     }   
    }

    return res;
}

int popcount3(unsigned *array, size_t len){
    unsigned x;
    int result = 0;
    
    for (int i = 0; i < len; i++){
        x = array[i];
        
        asm("\n"
        "ini3:  \n\t"               //seguir mientras que x != 0
            "shr %[x]       \n\t"   //LSB en CF
            "adc $0x0, %[r]     \n\t"
            "test %[x], %[x]    \n\t"
            "jnz ini3       \n\t"   //Saltar si es distinto de 0
            : [r] "+r" (result) // e/s: añadir a lo acumulado por el momento
            : [x] "r" (x)           //entrada: valor elemento
        );
    }
    
    return result;
}

int popcount4(unsigned *array, size_t len){
    unsigned x;
    int result = 0;
    
    for (int i = 0; i < len; i++){
        x = array[i];
        
        asm("\n"
            "clc                    \n\t"
        "ini4:  \n\t"               //seguir mientras que x != 0
            "adc $0x0, %[r]     \n\t"
            "shr %[x]   \n\t"
            "jnz ini4       \n\t"   //Saltar si es distinto de 0
            "adc $0x0, %[r] \n\t"
            : [r] "+r" (result) // e/s: añadir a lo acumulado por el momento
            : [x] "r" (x)           //entrada: valor elemento
        );
    }
    
    return result;
}

int popcount5(unsigned *array, size_t len){
    int val = 0;
    unsigned  x=0;
    int result = 0;
    for(int j=0; j < len; j++){
        val = 0;
        x = array[j];
        for (int i = 0; i < 8; i++) {
            val += x & 0x0101010101010101L;
            x >>= 1;
        }
        val += (val >> 16);
        val += (val >> 8);
        result += (val & 0xFF);
    }
    
    
    return result;
}

int popcount6(unsigned *array, size_t len){
    const long unsigned m1 = 0x5555555555555555; //binary: 0101...
    const long unsigned m2 = 0x3333333333333333; //binary: 00110011..
    const long unsigned m4 = 0x0f0f0f0f0f0f0f0f; //binary: 4 zeros, 4 ones ...
    const long unsigned m8 = 0x00ff00ff00ff00ff; //binary: 8 zeros, 8 ones ...
    const long unsigned m16 = 0x0000ffff0000ffff; //binary: 16 zeros, 16 ones ...
    unsigned x=0;
    int result = 0;
    
    for(int i=0; i < len; i++){
        x = array[i];

        x = (x & m1 ) + ((x >> 1) & m1 ); //put count of each 2 bits into those 2 bits
        x = (x & m2 ) + ((x >> 2) & m2 ); //put count of each 4 bits into those 4 bits
        x = (x & m4 ) + ((x >> 4) & m4 ); //put count of each 8 bits into those 8 bits
        x = (x & m8 ) + ((x >> 8) & m8 ); //put count of each 16 bits into those 16 bits
        x = (x & m16) + ((x >> 16) & m16); //put count of each 32 bits into those 32 bits

        result += x;
    }
    return result;
}

int popcount7(unsigned *array, size_t len){
    const unsigned long m1 = 0x5555555555555555; //binary: 0101...
    const unsigned long m2 = 0x3333333333333333; //binary: 00110011..
    const unsigned long m4 = 0x0f0f0f0f0f0f0f0f; //binary: 4 zeros, 4 ones ...
    const unsigned long m8 = 0x00ff00ff00ff00ff; //binary: 8 zeros, 8 ones ...
    const unsigned long m16 = 0x0000ffff0000ffff; //binary: 16 zeros, 16 ones ...

    size_t i;
    unsigned x1,x2;
    int result;

    for(i=0; i < len; i+=2){
        x1 = *(unsigned long*) &array[i ];
        x2 = *(unsigned long*) &array[i+1];

        x1 = (x1 & m1 ) + ((x1 >> 1) & m1 ); //put count of each 2 bits into those 2 bits
        x1 = (x1 & m2 ) + ((x1 >> 2) & m2 ); //put count of each 4 bits into those 4 bits
        x1 = (x1 & m4 ) + ((x1 >> 4) & m4 ); //put count of each 8 bits into those 8 bits
        x1 = (x1 & m8 ) + ((x1 >> 8) & m8 ); //put count of each 16 bits into those 16 bits
        x1 = (x1 & m16) + ((x1 >> 16) & m16); //put count of each 32 bits into those 32 bits


        x2 = (x2 & m1 ) + ((x2 >> 1) & m1 ); //put count of each 2 bits into those 2 bits
        x2 = (x2 & m2 ) + ((x2 >> 2) & m2 ); //put count of each 4 bits into those 4 bits
        x2 = (x2 & m4 ) + ((x2 >> 4) & m4 ); //put count of each 8 bits into those 8 bits
        x2 = (x2 & m8 ) + ((x2 >> 8) & m8 ); //put count of each 16 bits into those 16 bits
        x2 = (x2 & m16) + ((x2 >> 16) & m16); //put count of each 32 bits into those 32 bits

        result+= x1+x2;
    }

    return result;
}

int popcount8(unsigned* array, int len){
    int i, val=0; 
    int result=0;
    int SSE_mask[] = {0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f};
    int SSE_LUTb[] = {0x02010100, 0x03020201, 0x03020201, 0x04030302};
    
    for (i = 0; i < len; i += 4){
        asm (
            "movdqu     %[x], %%xmm0 \n\t"
            "movdqa   %%xmm0, %%xmm1 \n\t" 
            "movdqu     %[m], %%xmm6 \n\t" 
            "psrlw       $4 , %%xmm1 \n\t"
            "pand     %%xmm6, %%xmm0 \n\t" 
            "pand     %%xmm6, %%xmm1 \n\t" 
            
            "movdqu     %[l], %%xmm2 \n\t" 
            "movdqa   %%xmm2, %%xmm3 \n\t" 
            "pshufb   %%xmm0, %%xmm2 \n\t" 
            "pshufb   %%xmm1, %%xmm3 \n\t" 
            
            "paddb    %%xmm2, %%xmm3 \n\t" 
            "pxor     %%xmm0, %%xmm0 \n\t" 
            "psadbw   %%xmm0, %%xmm3 \n\t" 
            "movhlps  %%xmm3, %%xmm0 \n\t" 
            "paddd    %%xmm3, %%xmm0 \n\t" 
            "movd     %%xmm0, %[val] \n\t"
            :   [val]   "=r" (val)
            :   [x] "m" (array[i]),
                [m] "m" (SSE_mask[0]),
                [l] "m" (SSE_LUTb[0])
        );
        
        result +=val;
    }
    
    return result;
}

int popcount9(unsigned* array, size_t len){
    size_t i;
    unsigned x;
    int val, result=0;
    for (i=0; i<len; i++){
        x = array[i];
        asm("popcnt %[x], %[val]"
            : [val] "=r" (val)
            : [x] "r" (x)
        );
        result += val;
    }
    
    return result;
}

int popcount10(unsigned* array, size_t len){
    size_t i;
    unsigned long x1,x2;
    long val; long result=0;
    for (i=0; i<len; i+=4) {
        x1 = *(unsigned long*) &array[i ];
        x2 = *(unsigned long*) &array[i+2];
        asm("popcnt %[x1], %[val] \n\t"
            "popcnt %[x2], %%rdi \n\t"
            "add %%rdi, %[val]\n\t"
            : [val]"=&r" (val)
            : [x1] "r" (x1),
            [x2] "r" (x2)
            : "rdi"
            );
        result += val;
    }
    return result;
}


//desplazamiento de 1 bit a la derecha (acaba en el bit de accareo) sumarlo
//y seguir comprobando x hasta que valga 0 (~ 3 instrucciones)
//primer desplazamiento obligatorio, luego realizo la comprobacion
//shr-> desplazamiento a la derecha
//Adc -> suma de acarreo
//test para comprobar cuando x vale 0


void crono(int (*func)(), char* msg){
    struct timeval tv1,tv2; // gettimeofday() secs-usecs
    long    tv_usecs;    // y sus cuentas

    gettimeofday(&tv1,NULL);
    resultado = func(lista, SIZE);
    gettimeofday(&tv2,NULL);

    tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
            (tv2.tv_usec-tv1.tv_usec);
#if TEST==0
    printf("%ld" "\n",tv_usecs);
#else
    printf("resultado = %d\t", resultado);
    printf("%s:%9ld us\n", msg, tv_usecs);
#endif
}

int main()
{
#if TEST==0 || TEST==4
    size_t i;
    for (i=0; i<SIZE; i++)  // inicializar array

    lista[i]=i;
#endif
crono(popcount1 , "popcount1 (lenguaje C -       for)");
crono(popcount2 , "popcount2 (lenguaje C -      while)");
crono(popcount3 , "popcount3 (leng.ASM-body while 4i)");
crono(popcount4 , "popcount4 (leng.ASM-body while 3i)");
crono(popcount5 , "popcount5 (CS:APP2e 3.49-group 8b)");
crono(popcount6 , "popcount6 (Wikipedia- naive - 32b)");
crono(popcount7 , "popcount7 (Wikipedia- naive -128b)");
crono(popcount8 , "popcount8 (asm SSE3 - pshufb 128b)");
crono(popcount9 , "popcount9 (asm SSE4- popcount 32b)");
crono(popcount10, "popcount10(asm SSE4- popcount128b)");

#if TEST != 0
    printf("calculado = %d\n", RESULT);
#endif
    exit(0);
}

```

La práctica completa se puede encontrar en el siguiente [enlace](https://github.com/jomaenfe/EC) a mi repositorio de github donde están subidos el archivo .s y el archivo de hoja de cálculo con las mediciones y las gráficas.