# Práctica 6: Caché.

En esta práctica vamos a intentar comprender un poco mejor el funcionamiento de la memoria chaché y de sus líneas. Para ello, lo que vamos a realizar son programas que se encargarán de hacer accesos a la memoria caché aumentando progresivamente la cantidad de los datos a los que vamos a acceder y midiendo el tiempo de acceso entre ellos. Con esto lo que se consigue es acceder a todos los niveles de caché y poder ver en una gráfica como el tiempo de acceso va en aumento conforme aumenta el tamaño de los datos accedidos y el nivel de caché/memoria en el que se encuentran.


### Diario de trabajo:

Esta práctica se ha realiza en dos sesiones en clase. En la primera sesión, se dió una explicación sobre la práctica y se comenzó con el primer ejercicio, el tamaño de línea. En la segunda sesión, se dejó tiempo libre para terminar la práctica y preguntarle dudas al profesor.

### Información sobre mi ordenador:

Si ejecuto make info, la información que se obtiene es la siguiente:

```
line size = 64B
cache size = 32K/32K/256K/4096K/
cache level = 1/1/2/3/
```
Con la orden lscpu, obtengo la siguiente información:

```
Arquitectura:                        x86_64
modo(s) de operación de las CPUs:    32-bit, 64-bit
Orden de los bytes:                  Little Endian
CPU(s):                              4
Lista de la(s) CPU(s) en línea:      0-3
Hilo(s) de procesamiento por núcleo: 2
Núcleo(s) por «socket»:              2
«Socket(s)»                          1
Modo(s) NUMA:                        1
ID de fabricante:                    GenuineIntel
Familia de CPU:                      6
Modelo:                              69
Nombre del modelo:                   Intel(R) Core(TM) i7-4510U CPU @ 2.00GHz
Revisión:                            1
CPU MHz:                             2593.955
CPU MHz máx.:                        3100,0000
CPU MHz mín.:                        800,0000
BogoMIPS:                            5187.72
Virtualización:                      VT-x
Caché L1d:                           32K
Caché L1i:                           32K
Caché L2:                            256K
Caché L3:                            4096K
CPU(s) del nodo NUMA 0:              0-3
```
Por último, buscando mi modelo de CPU en la página cpu-world, obtengo la siguiente información:

![info_cpu](https://github.com/jomaenfe/EC/blob/master/Practica%206/Img/cpuwinfo.png?raw=true)


### Ejercicios:

#### Tamaño de línea:

En primer lugar, vamos a describir lo que es una línea de caché. Una línea de caché es la cantidad de información que viaja entre los niveles de caché y de memoria principal. Una vez dicho esto, podemos ver la gráfica que yo he obtenido.

![line](https://github.com/jomaenfe/EC/blob/master/Practica%206/Img/line.png?raw=true)

En esta gráfica se ve muy claramente como va aumentando el tiempo a medida que aumenta el tamaño de los datos. Hasta 4Bytes, es muy rápida porque la información se está moviendo entre los niveles de caché más rápidos, per conforme aumenta a 4, se puede ver como el tiempo sube de forma muy pronunciada. Esto ocurre porque los datos ya ocupan más tamaño que el mismo bloque de caché por lo que se está empezando a usar la caché de nivel 3 o la memoria principal. Al pasarnos de 64 que es el tamaño máximo de nuestra línea, el tiempo se "normaliza" esto significa que ya está usando siempre memoria principal.

El código a partir del cual se ha calculado la gráfica es el siguiente:

```
#include <algorithm>    // nth_element
#include <array>        // array
#include <chrono>       // high_resolution_clock
#include <iomanip>      // setw
#include <iostream>     // cout
#include <vector>       // vector

using namespace std::chrono;

const unsigned MAXLINE = 1024; // maximun line size to test
const unsigned GAP = 12;       // gap for cout columns
const unsigned REP = 20;      // number of repetitions of every test

int main()
{
	std::cout << "#" 
	          << std::setw(GAP - 1) << "line (B)"
	          << std::setw(GAP    ) << "time (µs)"
	          << std::endl;

	for (unsigned line = 1; line <= MAXLINE; line <<= 1) // line in bytes
	{
		std::vector<duration<double, std::micro>> score(REP);

		for (auto &s: score)
		{
			std::vector<char> bytes(1 << 24); // 16MB

			auto start = high_resolution_clock::now();

			for(unsigned i=0; i < line; i++)
				for (unsigned j = 0; j < bytes.size(); j += line)
					bytes[j]++;

			auto stop = high_resolution_clock::now();

			s = stop - start;
		}

		std::nth_element(score.begin(), 
		                 score.begin() + score.size() / 2, 
		                 score.end());

		std::cout << std::setw(GAP) << line
		          << std::setw(GAP) << std::fixed << std::setprecision(1)
		          << std::setw(GAP) << score[score.size() / 2].count()
		          << std::endl;
	}
}


```

### Tamaño de caché:

La memoria caché, es una de las memorias más rápidas de nuestro ordenador. Se encuentra montada en nuestra CPU. Esta memoria, es rápida pero también muy pequeña (con respecto a la cantidad de datos que puede almacenar), esto se debe a que se debe guardar un equilibrio entre coste, consumo y temperatura. Esta memoria se usa para guardar datos a los que el procesador accede de forma continua y así mejorar el rendimiento del ordenador en sí.

La gráfica que yo he obtenido es la siguiente:

![cache_size](https://github.com/jomaenfe/EC/blob/master/Practica%206/Img/size.png?raw=true)

Primeramente, decir que el tamaño de mi partición de swapping es de 8GB, y el de mi memoria RAM es de 16GB por lo que a mi me ha sido posible realizar este apartado consiguiendo que también actúe la partición de swap en el acceso a los datos.

Como he puesto anteiormente, los tamaños de los niveles de mi memoria caché son `/32K/256K/4096K/` respectivamente a 1, 2 y 3. Es por ello, que en la gráfica podemos observar subidas de tiempo justo en esos valores. Esto es debido a la bajada de velocidad que se da conforme se va subiendo de nivel de caché. En este gráfico, podemos ver que la mayor diferencia se da cuando salimos del primer nivel de caché, cuando se empieza a meter en la memoria principal y ya, es abismal cuando se empieza a trabajar con el disco duro.

El código, a partir del cual se han realizado los accesos a memoria y la gráfica es el siguiente:

```
#include <algorithm>    // nth_element
#include <array>        // array
#include <chrono>       // high_resolution_clock
#include <iomanip>      // setw
#include <iostream>     // cout
#include <vector>       // vector

using namespace std::chrono;

const unsigned long long MINSIZE = 1ULL << 10; // minimun line size to test:  1KB
const unsigned long long MAXSIZE = 1ULL << 34; // maximun line size to test: 16GB
const unsigned GAP = 12;          // gap for cout columns
const unsigned REP = 3;         // number of repetitions of every test
const unsigned long long STEPS = 1e7;       // steps

int main()
{
	std::cout << "#" 
	          << std::setw(GAP - 1) << "line (B)"
	          << std::setw(GAP    ) << "time (µs)"
	          << std::endl;

	for (unsigned long long size = MINSIZE; size <= MAXSIZE; size *= 2)
	{
		std::vector<duration<double, std::micro>> score(REP);

		for (auto &s: score)
		{
			std::vector<char> bytes(size);

			auto start = high_resolution_clock::now();
			unsigned long long uno=1;
			unsigned long long num_iter_vector = std::max(STEPS*64/size,uno);
			for (unsigned long long i = 0; i < num_iter_vector; ++i)
				for(unsigned long long j=0; j < size; j+=64)
					bytes[j]++;

			// for (unsigned i = 0; i < STEPS; ++i)
			// 	bytes[(i * 64) & (size - 1)]++;

			auto stop = high_resolution_clock::now();

			s = stop - start;
		}

		std::nth_element(score.begin(), 
		                 score.begin() + score.size() / 2, 
		                 score.end());

		std::cout << std::setw(GAP) << size
		          << std::setw(GAP) << std::fixed << std::setprecision(1)
		          << std::setw(GAP) << score[score.size() / 2].count()
		          << std::endl;
	}
}


```
