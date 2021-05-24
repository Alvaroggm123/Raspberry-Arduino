//--------//====================================//--------//
// Programa con comunicacion duplex entre serial y arduino
// Alvaro Gabriel Gonzalez Martinez
// Version 2.0 - Arduino 24/5/2021
//--------//====================================//--------//
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atol */
const int Cdatos = 2, Cdata = 2, Ctotal = Cdatos + Cdata; // Cantidad de datos a enviar
const int End = 10;
float Var[Ctotal];  // Inicializa arreglo (lista) llamada Var con n elementos
/* |==|  Sensores  |==| */
const int Su[3][2] = {{9, 10}, {12, 11}};
void setup() {
  Serial.begin(115200);
  //establishContact();

  /* Configuracion de  pins  de  sensores  ultrasonicos */
  for (int i = 0; i < 2; i++)
  {
    pinMode(Su[i][0], OUTPUT); /* Trigger como salida */
    pinMode(Su[i][1], INPUT);  /*  Echo como  entrada */
  }
}
//-----/ Procedimiento que inicializa la comunicacion Serial/-----//
void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("1,2,3"); // Envia datos de prueba
    delay(200);
  }
}

//--------/ Fin procedimiento establecer contacto /--------//
float Distancia(int SuN)
{
  /* Variable local para calculos */
  long Duration;
  int Distancia;
  /* Limpieza de Trigger */
  digitalWrite(Su[SuN][0], 0);
  delayMicroseconds(2);
  /* Encendido de Trigger 10 segundos */
  digitalWrite(Su[SuN][0], 1);
  delayMicroseconds(10);
  digitalWrite(Su[SuN][0], 0);
  /* Lectura del Echo del sesor */
  Duration = pulseIn(Su[SuN][1], 1);
  /* Calculos para la distancia */
  Distancia = Duration * 0.034 / 2;
  return (Distancia);
}

//--------/ Inicia procedimiento de envio de datos /-------//
void sendData() {
  readSensors();//----------------------------// Llamada al procedimiento leerDatos

  //-----// Separacion de datos /-----//
  for (int i = 0; i < Ctotal; i++) {
    Serial.print(String(Var[i]) + ',');     // Uso de coma para separar (',')
  }
  Serial.println();                   // Fin envio de linea de datos
  //-----// Fin arreglo de separacion /-----//
}
//--------/ Fin procedimiento de envio de datos /-------//

//--------/ Inicia procedimiento de recepcion de datos /-------//
void readData() {
  if (Serial.available() > 0) {
    String Message = Serial.readStringUntil(End);
    splitData(Message + " ");
  }
}
void splitData(String Message ) {
  char Msg[Message.length() + 1];
  Message.toCharArray(Msg, Message.length());
  char* ptr = strtok(Msg, ",");
  byte i = 0;
  while (ptr) {
    Var[i + Cdatos] = atol(ptr);
    ptr = strtok(NULL, ",");
    i++;
  }
}
//--------/ Fin procedimiento de envio de datos /-------//

//--------/ Inicia procedimiento de lectura de datos/--------//
void readSensors() {
  Var[0] = Distancia(0);    // Distancia
  Var[1] = Distancia(1);    // Distancia
  //Var[i]=lectura de sensor; // Ejemplo de asignacion de mas lecturas
}
//--------/ Fin procedimiento de lectura de sensores/--------//


void loop() {
  readData();
  sendData();      // Llamada a procedimiento envia
  delay(500);
  if (Var[2] == 1)
    Serial.println("Algo prendio");
}