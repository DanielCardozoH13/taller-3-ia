#include <Servo.h>

Servo SERVO1;
int VELOCIDAD_MAX = 250;
int VELOCIDAD_MIN = 150;

int IZQ_A 		= 4;               
int IZQ_B 		= 3;               
int DER_A 		= 5;               
int DER_B 		= 6;
int ENB 		= 7; 
int ENA 		= 2;
int VELOCIDAD 	= VELOCIDAD_MIN;
int ESTADO 		= 53; //en switch significa "parar"

int ANGULO_DERECHO 		= 165;
int ANGULO_CENTRO 		= 88;
int ANGULO_IZQUIERDO 	= 15;

int PECHO 		= 11;
int PTRIG 		= 12;
int duracion, distancia, DisDer, DisIzq, DisEcoDer, DisEcoIzq, DisFrenteEco;
int pause_giro = 300;
int dirIzq = 8;
int dirDer = 9;
int pinServo = 10;
int paso_automatico = 1000;

void setup(){ 
	Serial.begin(9600);  
	SERVO1.attach(pinServo);     
	pinMode(DER_A, OUTPUT);
	pinMode(DER_B, OUTPUT);
	pinMode(IZQ_A, OUTPUT);
	pinMode(IZQ_B, OUTPUT);
	pinMode(PECHO, INPUT);
	pinMode(PTRIG,OUTPUT);
  pinMode(dirIzq, OUTPUT);
  pinMode(dirDer, OUTPUT);
} 

/*
 * los valores recibidos por la tarjeta bluetooth llegan codigicados en ascii, osea que:
 * 49 = 1 (flecha arriba)
 * 50 = 2 (flecha izquierda)
 * 51 = 3 (flecha derecha)
 * 52 = 4 (flecha abajo)
 * 53 = 5 (centro - pausa)
 * 54 = 6 (RAPIDO)
 * 55 = 7 (AUTO)
 * 56 = 8 (LENTO)
 * 57 = 9 (NORMAL)
 */

void loop(){
	if(Serial.available()>0){
		ESTADO = Recibir_Datos();
	}
	//Serial.println(ESTADO);
  delay(100);
	
	switch(ESTADO){
    case 49:  //AVANZAR
      Avanzar();
      break;
    case 50: //GIRAR IZQUIERDA
      Parar();
      Parar_Izq();
      delay(pause_giro);
      ESTADO = 49;
      break;
    case 51: //GIRAR DERECHA
      Parar();
      Parar_Der();
      delay(pause_giro);
      ESTADO = 49;
      break;
    case 52: //RETROCEDER
      Retroceder();
      //Retroceder_Izq();
      break;
    case 53:  //PARAR
      Parar();
      break;
		case 54:
			VELOCIDAD = VELOCIDAD_MAX;
      pause_giro = 350;
			break;
    case 55:
      mode_auto();
      break;
		case 56:
			VELOCIDAD = VELOCIDAD_MIN;
      pause_giro = 600;
			break;
    default:
      break;
	}
}

void direccional(int direccion){
  //Serial.println(direccion);
  if (direccion == 8){
    for (int i = 0; i<3; i++){
      digitalWrite(dirIzq, HIGH);
      delay(400);
      digitalWrite(dirIzq, LOW);
      delay(400);
    }
  }else if(direccion == 9){
    for (int i = 0; i<3; i++){
      digitalWrite(dirDer, HIGH);
      delay(400);
      digitalWrite(dirDer, LOW);
      delay(400);
    }
  }else{
    digitalWrite(dirIzq, LOW);
    digitalWrite(dirDer, LOW);
  }
}

int Recibir_Datos(){
	return Serial.read();
}

void Avanzar(){
	digitalWrite(DER_A, LOW);
	digitalWrite(DER_B, HIGH);
	analogWrite(ENB, VELOCIDAD);
	digitalWrite(IZQ_A, HIGH);
	digitalWrite(IZQ_B, LOW);
	analogWrite(ENA, VELOCIDAD);
}

void Parar(){
	analogWrite(ENB, 0);
	analogWrite(ENA, 0);
}

void Retroceder(){
	digitalWrite(DER_A, HIGH);
	digitalWrite(DER_B, LOW);
	analogWrite(ENB, VELOCIDAD);
	digitalWrite(IZQ_A, LOW);
	digitalWrite(IZQ_B, HIGH);
	analogWrite(ENA, VELOCIDAD);
}


void Parar_Der(){
  direccional(dirIzq);
	digitalWrite(DER_A, LOW);
	digitalWrite(DER_B, HIGH);
	analogWrite(ENB, VELOCIDAD);
	analogWrite(ENA, 0);
}

void Parar_Izq(){
  direccional(dirDer);
	analogWrite(ENB, 0);
	digitalWrite(IZQ_A, HIGH);
	digitalWrite(IZQ_B, LOW);
	analogWrite(ENA, VELOCIDAD);
}

int find_obstacule(){
  delay(1000);
  int obstaculo; //variable retorna 1 si hay obstaculo antes de 16 cm, 0 si no hay obstaculo
  digitalWrite(PTRIG, HIGH);
  delay(0.01);
  digitalWrite(PTRIG, LOW);
  duracion = pulseIn(PECHO, HIGH);
  distancia = ((duracion/2)/29);
  
  Serial.println(distancia);
  delay(1000);

  if(distancia <= 20){
     obstaculo = 1;
    }else{
      obstaculo = 0;
    }
  return obstaculo;
  }

int mode_auto(){
  while (ESTADO != 57){
    if(Serial.available()>0){
      ESTADO = Recibir_Datos();
      if (ESTADO == 53 or ESTADO == 57){
        ESTADO = 53;
        return 1;
      }
    }
    delay(1000);
    
    SERVO1.write(ANGULO_CENTRO);
    Serial.println("CENTRO");
    int find_centro = find_obstacule();
    delay(1000);
    SERVO1.write(ANGULO_DERECHO);
    int find_rigth = find_obstacule();
    delay(1000);
    SERVO1.write(ANGULO_IZQUIERDO);
    int find_left = find_obstacule();
    delay(1000);

    if (find_centro == 0){
      Avanzar();
      delay(paso_automatico-150);
      Parar();
      delay(paso_automatico);
    }else{
      if(find_rigth == 0){
        //Parar();
        Parar_Der();
        delay(pause_giro);
        Parar();
        delay(paso_automatico);
      }else{
        if (find_left == 0){
          //Parar();
          Parar_Izq();
          delay(pause_giro);
          Parar();
          delay(paso_automatico);
        }else{
          Retroceder();
          delay(paso_automatico-150);
          Parar();
        }
      }
    }
  }
}
