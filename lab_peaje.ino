/*
 * author = Daniel Cardozo
 * date = Noviembre 5 del 2019
 * 
 * Codigo arduino para el control de punto de peaje usando la placa 'Mega 2560'.
 * En este codigo se controlan distintos elementos electronico a través del microchip incorporado y programable que tiene la placa de arduino 'MEGA 2560', durante el proceso
 * se manipulo el funcionamento del sensor ultransonico HC-SR04, el servo motor SG90, la pantalla led de tamaño 16x2 y algunos otros elementos adicionales como luces led y pulsadores
 */

#include <Servo.h>
#include  <LiquidCrystal.h>

 /*
  * LIBRERIAS
  * Se usaron las librerias Servo.h y LiquidCrystal.h para controlar el sensor servo SG90 y pantalla led 16x2, respectivamente
  */

/*
 * VARIABLES GLOBALES:
 * pinServo, pinPulsador, pinLuzRoja, pinLuzVerde, pinTrig, pinEcho, rs, en, d4, d5, d6, d7 --> SON PINES DE LA PLACA DE ARDUINO
 * DURACACION y DISTANCIA --> SON VARIABLES CON LAS QUE SE VA A CALCULAR LA DISTANCIA A LA QUE SE ENCUENTRA UN VEHICULO FRENTE AL SENSOR ULTRASONICO
 */

const int pinServo = 2,pinPulsador = 3, pinLuzRoja = 4, pinLuzVerde = 5, pinTrig = 12, pinEcho = 13;
int DURACION, DISTANCIA;
const int rs = 6, en = 7, d4 = 8, d5 = 9, d6 = 10, d7 = 11;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //se indican los pines de la placa arduino que estan concectados a la pantalla led de 16x2

Servo servo;  //creamos un objeto servo 
int barrera = 1;  //se indica que la barrera va a estar abajo

void setup() {
  servo.attach(pinServo);  // asignamos el pin al servo.
  /*
   * se indica los pines de la placa de arduino a los que hace referencia las variables 'pinPulsador' y 'pinEcho' como pines de entrada  
   * se indica los pines de la placa de arduino a los que hace referencia las variables 'pinLuzRoja' y 'pinTrig' como pines de salida
   */
  pinMode(pinPulsador, INPUT);  
  pinMode(pinLuzRoja, OUTPUT);
  pinMode(pinTrig, OUTPUT);
  pinMode(pinEcho,  INPUT);
  lcd.begin(16, 2); //inicio de pantalla led indicando que es de 16x2
  Serial.begin(9600); //se crea comunicacion con el pc a través del serial
}

void loop() {
  controlLcd(1);  //se hace llamado a la funcion controlLcd() indicando que muestre el mensaje 1
  controlBarrera(); //se hace llamado a la funcion controlBarrera() que manipula el funcionamiento del sensor servo
}

void controlBarrera(){
  /*
   * Función creada para controlar la barrera del peaje.
   * 
   * Se lee el estado del pulsador 
   * Si la señal del pulsador esta en Bajo se envia una señal baja a la luz led de color roja, consecutivamente se envia una señal alta a la luz led de color verde, luego
   * se hace el llamado a la funcion controlLcd() para que imprima en la pantalla lcd que el pago se ha realizado, por ultimo se posiciona el Servo a 120 grados y se llama a la funcion 
   * ultrasonido() que hara una pausa a la ejecución del código si encuentra algun obstaculo.
   * 
   * Si la lectura de la señal del pulsor se alta, se envia una señal alta a la luz led de color roja, consecutivamente se envia una señal baja a la luz led de color verde, por ultimo
   * se posiciona el Servo a 4 grados simulando como su la barrera estuviera abajo
   */
  barrera = digitalRead (pinPulsador);
  if (barrera == LOW) {          
    digitalWrite(pinLuzRoja, LOW);
    digitalWrite(pinLuzVerde, HIGH);
    controlLcd(2);  //se hace llamado a la funcion controlLcd() indicandole que muestre el mensaje 2
    servo.write(120);                  
    ultrasonido();                       
   }
   if (barrera == HIGH){
    digitalWrite(pinLuzRoja, HIGH);
    digitalWrite(pinLuzVerde, LOW);
    servo.write(4);
    delay(15);
   }
}
void ultrasonido(){
  /*
   * Funcion creada para controlar el sensor ultrasonido
   * Basicamente se crea un ciclo while que se repetira hasta que la variable 'BANDERA' sea diferente de 0, dentro del ciclo se lanza la señal ultrasonica y se calcula
   * la distancia a la que puede estar un objeto en frente del sensor, con base al tiempo en que se demora en retornar el eco de la señal.
   * Si el sensor ubica un objeto a una distancia menor a 12 centimetros (que es lo que mide la carretera de la maqueta) se crea una pausa de 4 segundos para permitir el paso 
   * del vhiculo; su el sensor no hubica nada en el rango mencionado anteriormente rompe el ciclo while asignandole a la variable 'BANDERA' un valor de 1
   */
  int BANDERA = 0; //variable para contrar el ciclo while(){}
  while (BANDERA == 0){
    digitalWrite(pinTrig, HIGH);
    delay(1);
    digitalWrite(pinTrig, LOW);
    DURACION = pulseIn(pinEcho, HIGH);
    DISTANCIA = DURACION / 58.2;  //58.2 es una constante especificada por el fabricante del sensor
    Serial.println (DISTANCIA);

    if (DISTANCIA <= 12){
      delay(4000);
    }else{
      BANDERA=1;
      delay(1000);
    }
  }
}

void controlLcd(int estado){
  /*
   * Funcion creada para manipular la informacion mostrada en la pantalla Led
   * 
   * Si el parametro de entrada es 1 se imprimira en la primer fila de la pantalla "BIENVENIDO" y en la segunda "Punto de peaje"
   * Si el parametro de entrada es 2 se imprimira en la primer fila de la pantalla "Pago realizado" y en la segunda "Buen viaje...!"
   */
  if (estado == 1){
    lcd.setCursor(0,0);
    lcd.print("  BIENVENIDO!!  ");
    lcd.setCursor(0,1);
    lcd.print(" Punto de peaje ");
    lcd.display();
  }
  if (estado == 2){
    lcd.setCursor(0,0);
    lcd.print(" Pago realizado ");
    lcd.setCursor(0,1);
    lcd.print(" Buen viaje...! ");
    lcd.display();
  }
}
