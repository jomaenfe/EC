### Desactivación bomba de Sara.

Para desactivar esta bomba utilizamos el depurador "gdb" que está incluido en el sistema operativo que utilizamos. Abrimos la bomba con el comando `gdb -tui bombaSara` y ponemos los layout para que se vea el codigo ensamblador y los registros.

En primer lugar, lo que hacemos es que vemos la direcció de memoria en la que está la contraseña y la mostramos en modo texto con la orden `p(char *) <direccion_memoria>`. Lo que vemos es una secuencia que no forman ninguna palabra, por lo que sabemos que la contraseña esta cifrada. Empiezo a ejecutar el programa, *habiendo puesto anteriormente un breakpoint donde empieza la función main* y empiezo a observar los registros. Después de pedir la contraseña llama a una función que lo que hace es codificar lo que le hemos introducido. Entonces, introduzco la cadena "abcdefg" y observo que la salida de la función ha modificado mi cadena y saca "acegikm". Haciendo cálculos me doy cuenta de que el cifrado que utiliza es que a las letras les suma en ASCII un número, el mismo número que la posición que ocupa la letra en la misma palabra. Al descifrar la contraseña vemos que es "escalera". Más abajo, después de pasar las comprobaciones de tiempo, nos encontramos con el pin. En código ensamblador veo que al registro en el que ha guardado lo que yo he introducido usa una instrucción add. Por lo que se que le está sumando algún valor, y en la misma intrucción veo que le está sumando un valor directo, es 8. El pin lo sacamos usando la orden `p*(int *) <direccion_memoria` y vemos que es 2026, por lo que tenemos que restarle 8 para saber que pin introducir. Por lo tanto si introducimos como contraseña "escalera" y como pin "2018" la bomba se desactivará.

La bomba original está se llama `bombaSara`.

### Cambiando contreña y pin.

Para cambiar la contraseña y el pin lo que he utilizado ha sido el "ghex". Para cambiar la contraseña he buscado en el programa "etedpjxh" y como no se puede modificar la longitud he cambiado su contraseña original por "modifica", habiendolo cifrado anteriormente. Para cambiar el pin, lo que he hecho ha sido traducir a hexadecimal el valor del mismo y ponerlo en el formato little-endian. Una vez encontrado lo que he hecho ha sido calcular el nuevo pin, que es "2008", y pasar a hexadecimal little-endian.

La bomba con las contraseña modificadas es `bombaSara_mod`. 