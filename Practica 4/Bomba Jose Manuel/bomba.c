//linea para compilacion -> gcc -Og bomba.c -o bomba -no-pie -fno-guess-branch-probability

#include <stdio.h>	// para printf(), fgets(), scanf()
#include <stdlib.h>	// para exit()
#include <string.h>	// para strncmp()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define SIZE 100
#define TLIM 60

char password[]="zzrunzz\n";	// contraseña 
int  passcode=0;			// pin -> 131 generado en la funcion crearPin

int calcularPosicionAbecedario(int letra){
	int resultado = 0;
	const int inicio = 96;
	resultado = letra-inicio;
	return resultado;

}

void crearPin( int *passcode,char password[]){
	for(int i = 0; i < 6;i++){
		*passcode = *passcode + calcularPosicionAbecedario(password[i]);
	}
}

void boom(void){
	printf(	"\n"
		"***************\n"
		"*** BOOM!!! ***\n"
		"***************\n"
		"\n");
	exit(-1);
}

void defused(void){
	printf(	"\n"
		"·························\n"
		"··· bomba desactivada ···\n"
		"·························\n"
		"\n");
	exit(0);
}

int main(){
	char pw[SIZE];
	int  pc, n;

	
	crearPin(&passcode, password);

	struct timeval tv1,tv2;	// gettimeofday() secs-usecs
	gettimeofday(&tv1,NULL);

	do	printf("\nIntroduce la contraseña: ");
	while (	fgets(pw, SIZE, stdin) == NULL );
	if    (	strncmp(pw,password,sizeof(password)) )
	    boom();

	gettimeofday(&tv2,NULL);
	if    ( tv2.tv_sec - tv1.tv_sec > TLIM )
	    boom();

	do  {	printf("\nIntroduce el pin: ");
	 if ((n=scanf("%i",&pc))==0)
		scanf("%*s")    ==1;         } //limpia todo lo que no sea el numero
	while (	n!=1 );
	if    (	pc != passcode )
	    boom();

	gettimeofday(&tv1,NULL);
	if    ( tv1.tv_sec - tv2.tv_sec > TLIM )
	    boom();

	defused();
	
}
