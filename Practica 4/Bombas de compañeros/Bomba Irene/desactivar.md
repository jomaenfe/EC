### Desactivar bomba de Irene.

Para desactivar esta bomba utilizamos el depurador "gdb" que está incluido en el sistema operativo que utilizamos. Abrimos la bomba con el comando `gdb -tui bombaIrene` y ponemos los layout para que se vea el codigo ensamblador y los registros.

Si examinamos la dirección de memoria que se muestra en el código con la etiqueta de "password", vemos que la contraseña que le a puesto a su programa está cifrada. Con la orden `p(char *) <direccion_memoria>` sacamos "geooneoo". Para sacar el método de cifrado que tiene lo que hago es ayudarme del código asm y de una cadena de texto que introduzco para ver como se queda. En este caso introduzco la cadena "abcdefg" y de salida obtengo "bbddffh". Por lo que el método de cifrado que tiene es, sumarle un caractér ASCII a las posiciones pares de la palabra empezando por el 0. Es decir, 0,2,4... Una vez sabiendo el método de cifrado que tiene es facil descodificar la contraseña. La contraseña es "fenomeno". Después de saltarnos la comprobación del tiempo, nos encontramos con el pin. Vemos que hay una instrucción de resta, la instrucción "sub" y que le resta 3 al registro en el que metemos la contraseña por lo que ya sabemos que hay que poner 3 valores mas sumados al pin que el original. Para sacar el pin original solo debemos aplicar la orden `p*(int *) <direccion_memoria>`. Esta orden saca el valor "3000" por lo que para que pase, nosotros debemos introducir "3003".

La bomba original es `bombaIrene_mod`. 

### Cambiando contraseña y pin a la bomba.

Para cambiar la contraseña y el pin lo que he utilizado ha sido el "ghex". Para cambiar la contraseña he buscado en el programa "geooneoo" y como no se puede modificar la longitud he cambiado su contraseña original por "modifica", pero cifrandola con su método para que se quede cifrada. Para cambiar el pin, lo que he hecho ha sido traducir a hexadecimal el valor del mismo y ponerlo en el formato little-endian. Una vez encontrado lo que he hecho ha sido calcular el nuevo pin, que es "3003", y pasar a hexadecimal little-endian.

La bomba con las contraseña modificadas es `bombaIrene_mod`. 
