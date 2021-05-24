//--------//====================================//--------//
// A duplex comunication between Arduino and Raspberry py //
// Autor: Alvaro Gabriel Gonzalez Martinez                //
// Version 2.0 - Arduino 24/5/2021                        //
//--------//====================================//--------//
// Requirenments
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atol */
/* Here  you  need  to  put the length of the number */
/* of arduino sensors you will use and the amount of */
/* incoming data. */
const int Cdatos = 2, Cdata = 2, Ctotal = Cdatos + Cdata;
const int End = 10; /* A constant to specify a line break */
float Data[Ctotal]; /* Our Data inicialized by Ctotal.    */
/* |==|  Arduino output and input devices |==| */
/* |==|   Ultrasonic sensors  |==| */
const int Su[3][2] = {{9, 10}, {12, 11}};
/* |==|         Motors        |==| */
const int Mo[2][3] = {{8, 7, 6}, {3, 4, 5}};

/* [INIT] */ /* |==| Arduino configuration |==| */
void setup() {
  /* |==| Ultrasonic configuration |==| */
  Serial.begin(115200);

  /*  Output motors configuration */
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 3; j++)
      pinMode(Mo[i][j], OUTPUT);

  /* |==| Ultrasonic configuration |==| */
  for (int i = 0; i < 2; i++)
  {
    pinMode(Su[i][0], OUTPUT); /* Trigger */
    pinMode(Su[i][1], INPUT);  /*  Echo   */
  }
}
/* [F] */ //--------/  A  distance  reader  function   /--------//
// The sensor to be read must be sent as a parameter. //
float Distancia(int SuN)
{
  /* Local variables */
  long Duracion;
  int Distancia;
  /* Clean up Trigger */
  digitalWrite(Su[SuN][0], 0);
  delayMicroseconds(2);
  /* Triggering  */
  digitalWrite(Su[SuN][0], 1);
  delayMicroseconds(10);
  digitalWrite(Su[SuN][0], 0);
  /* Reading the echo */
  Duracion = pulseIn(Su[SuN][1], 1);
  /* We calculate the distance */
  Distancia = Duracion * 0.034 / 2;
  return (Distancia);
}

/* [M] */ //--------/ Our sensor reader method /--------//
void readSensors() {
  Data[0] = Distancia(0);    // Distancia
  Data[1] = Distancia(1);    // Distancia
}
/* [M] */ //--------/ Data send method /-------//
void sendData() {
  readSensors();// Reading the sensors

  //-----// Sending data /-----//
  for (int i = 0; i < Ctotal; i++) {
    Serial.print(String(Data[i]) + ',');
  }
  Serial.println(); // A line break
}
/* [M] */ //--------/ A reader data method /-------//
void readData() {
  if (Serial.available() > 0) {
    String Message = Serial.readStringUntil(End);
    // To read our data we call the method splitData
    // This allow to change our data from String to float
    // and put into our Data array.
    splitData(Message + " ");
  }
}
/* [M] */ //--------/ A Split method /-------//
void splitData(String Message ) {
  char Msg[Message.length() + 1];
  // A new array to split the data.
  Message.toCharArray(Msg, Message.length());
  char* ptr = strtok(Msg, ",");
  // Our index
  byte i = 0;
  while (ptr) {
    // We convert from char to numeric value
    Data[i + Cdatos] = atol(ptr);
    ptr = strtok(NULL, ",");
    i++;
  }
}
/* [M] */ //--------/ Turning off motors method /-------//
void motorsOff() {
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      digitalWrite(Mo[i][j], 0);
}
/* [M] */ //--------/ Motor control method /-------//
/* Parameters */
/* MotorDirecction(leftDirection, rightDirecction) using max power or     */
/* MotorDirecction(leftDirection, rightDirecction, leftPower, rigthPower) */
void motorDirecction(int I, int D) {
  motorsOff();
  digitalWrite(Mo[0][I], 1);
  digitalWrite(Mo[1][D], 1);
  analogWrite(Mo[0][2], 255);
  analogWrite(Mo[1][2], 255);
}
void motorDirecction(int I, int D, int Ipow, int Dpow) {
  motorsOff();
  digitalWrite(Mo[0][I], 1);
  digitalWrite(Mo[1][D], 1);
  analogWrite(Mo[0][2], map(Ipow, 0, 100, 100, 255));
  analogWrite(Mo[1][2], map(Dpow, 0, 100, 100, 255));
}
//--------/  Loop  /--------//
void loop() {
  readData();
  sendData();
  // Test our callouts
  int dataInSerial = 0;
  if (Data[Cdatos + dataInSerial ] == 8) {
    motorDirecction(0, 0, 100, 100);
  }
  else if (Data[Cdatos + dataInSerial ] == 5) {
    motorDirecction(1, 1, 100, 100);
  }
  else if (Data[Cdatos + dataInSerial ] == 4) {
    motorDirecction(0, 1, 100, 100);
  }
  else if (Data[Cdatos + dataInSerial ] == 6) {
    motorDirecction(1, 0, 100, 100);
  }
  else
    motorsOff();
}
