# Práctica 5: E/S con Arduino.

En esta prática se van a montar dos circuitos con placas de arduino. En el primero de los tres, no hay que montar nada. Tan solo abrimos un código de ejemplo que viene en el editor de arduino y haremos parpadear el led. En el segundo ejercicio, tenemos que instalar en la placa un altavoz y desde el código se podrán reproducir canciones. Por último, en el tercer ejercicio, tendremos que montar un theremín de luz. 

### Diario de trabajo:

Esta práctica se ha divido en dos sesiones. La primera sesión fue guiada por el profesor, y configuramos la placa para que fuese reconocida en nuestro ordenador. También probamos la placa con el primer ejercicio. En la segunda sesión, directamente empezamos a montar el theremín de luz. En mi caso también he montado los leds que indican la intensidad.

### Ejercicios:

#### Ejercicio 1:

En este ejercicio no tenemos mas que coger el ejemplo que se encuentra en el mismo editor de arduino. Este programa se encuentra en la ruta `Archivo > Ejemplos > 01.Basics > Blink`. Tan solo tenemos que configurar el editor, diciendole el puerto donde está conectada la placa y el modelo de placa. Con esto, el led debería parpadear.

#### Ejercicio 2: 

En este segundo ejercicio tendremos que montar en la placa un pequeño altavoz. En la **imagen 1**, podemos ver una imagen con la placa montanda. 

![Imagen 1](https://github.com/jomaenfe/EC/blob/master/Practica%205/Imagenes/ejercicio2_imagen.jpg?raw=true)

El esquema de montaje que hemos seguido para poder montar este circuito, es el que se encuentra en la **imagen 2**.

![Imagen 2](https://github.com/jomaenfe/EC/blob/master/Practica%205/Imagenes/esquema_ejercicio2.jpg?raw=true)

Para hacer funcionar este ejercicio, necesitamos el código que se encuentra en los arhcivos que descargamos de la página web de la placa. El siguiente código es el que se utiliza para el ejercicio.

```

//www.elegoo.com
//2016.12.08

#include "pitches.h"
 
// notes in the melody:
int melody[] = {
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};
int duration = 500;  // 500 miliseconds
 
void setup() {
 
}
 
void loop() {  
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    // pin8 output the voice, every scale is 0.5 sencond
    tone(8, melody[thisNote], duration);
     
    // Output the voice after several minutes
    delay(1000);
  }
   
  // restart after two seconds 
  delay(2000);
}


```

El vídeo correspondiente a este ejercicio se encuentra en la misma carpeta que este pdf y se llama "ejercicio2_video".

#### Ejercicio 3:

En este ejercicio hemos montado un theremín de luz. Es decir, con una celula fotosensible y un altavoz, vamos variando la cantidad de luz que la célula recibe y por lo tanto el altavoz también varía su sonido. Adicionalmente también he montado en este circuito 8 leds que van encendiendose conforme aumenta la intensidad de luz recibida. En la **imagen 3** se puede ver una imagen del circuito montado.

![Theremín_luz](https://github.com/jomaenfe/EC/blob/master/Practica%205/Imagenes/Ejercicio3_imagen.jpg?raw=true)

El esquema que he seguido para montarlo se puede ver en la **imagen 4**.

![Esquema_theremin](https://github.com/jomaenfe/EC/blob/master/Practica%205/Imagenes/esquema_ejercicio3.jpg?raw=true)

Para que funcione este circuito correctamente, hemos tenido que "mezclar" el código de las lecciones 7 y 26, siendo el siguiente código el resultado final.

```

// variable to hold sensor value
int sensorValue;
// variable to calibrate low value
int sensorLow = 1023;
// variable to calibrate high value
int sensorHigh = 0;
// LED pin
const int ledPin = 13;


int lightPin = 0;
int latchPin = 11;
int clockPin = 9;
int dataPin = 12;

int leds = 0;


void setup() {
  // Make the LED pin an output and turn it on
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  // calibrate for the first five seconds after program runs
  while (millis() < 5000) {
    // record the maximum sensor value
    sensorValue = analogRead(A0);
    if (sensorValue > sensorHigh) {
      sensorHigh = sensorValue;
    }
    // record the minimum sensor value
    if (sensorValue < sensorLow) {
      sensorLow = sensorValue;
    }
  }
  // turn the LED off, signaling the end of the calibration period
  digitalWrite(ledPin, LOW);

    pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
}

void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}

void loop() {

  int reading  = analogRead(lightPin);
  int numLEDSLit = map(sensorValue, sensorLow, sensorHigh, 0, 8);
  if (numLEDSLit > 8) numLEDSLit = 8;
  leds = 0;   // no LEDs lit to start
  for (int i = 0; i < numLEDSLit; i++)
  {
    leds = leds + (1 << i);  // sets the i'th bit
  }
  updateShiftRegister();
  
  //read the input from A0 and store it in a variable
  sensorValue = analogRead(A0);

  // map the sensor values to a wide range of pitches
  int pitch = map(sensorValue, sensorLow, sensorHigh, 50, 4000);

  // play the tone for 20 ms on pin 8
  tone(8, pitch, 20);

  // wait for a moment
  delay(10);
}


```


Por último, el vídeo donde se ve en funcionamiento se encuentra en la misma carpeta que este pdf y se llama "ejercicio3_video".
