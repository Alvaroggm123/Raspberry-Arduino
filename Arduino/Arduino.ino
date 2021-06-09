//--------//====================================//--------//
// A duplex comunication between Arduino and Raspberry py //
// Autor: Alvaro Gabriel Gonzalez Martinez                //
// Version 2.0 - Arduino 2/6/2021                        //
//--------//====================================//--------//
// Requirenments
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atol */
/* Here  you  need  to  put the length of the number */
/* of arduino sensors you will use and the amount of */
/* incoming data. */
const int Cdatos = 3, Cdata = 2, Ctotal = Cdatos + Cdata;
const int End = 10, battLED = 13; /* A constant to specify a line break */
float Data[Ctotal]; /* Our Data inicialized by Ctotal.    */
/* |==|  Arduino output and input devices |==| */
/* |==|   Ultrasonic sensors  |==| */
const int Su[3][2] = {{9, 10}, {12, 11}};
/* |==|         Motors        |==| */
const int Mo[2][3] = {{7, 8, 6}, {4, 3, 5}};
bool flag = true;
int Pot[] = {60, 60};
/* |==|   PID variables  |==| */
int maximumRange = 200; // maximo rango
int minimumRange = 0; // Minimo rango
float duration, distance, actual, sp = 20, error;
float previous_error, drive, derivative = 0, integral = 0, kp = 0.28;
float kd = 0.01, ki = 0.0001, salida, x, bandera = 0;

/* [INIT] */ /* |==| Arduino configuration |==| */
void setup() {
  /* |==| Ultrasonic configuration |==| */
  Serial.begin(115200);

  /* Config. battLED */
  pinMode(battLED, OUTPUT);

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
  Data[2] = map(analogRead(A0), 0, 491, 0, 100); // Distancia
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
/* MotorDirecction(leftDirection, rightDirecction, Power) */
void motorDirecction(int I, int D) {
  motorsOff();
  digitalWrite(Mo[0][I], 1);
  digitalWrite(Mo[1][D], 1);
  analogWrite(Mo[0][2], map(Data[Cdatos + 1], 0, 0, 0, 255));
  analogWrite(Mo[1][2], map(Data[Cdatos + 1], 0, 0, 0, 255));
}
void motorDirecction(int I, int D, int Ipow, int Dpow) {
  motorsOff();
  digitalWrite(Mo[0][I], 1);
  digitalWrite(Mo[1][D], 1);
  analogWrite(Mo[0][2], map(Ipow, 0, 100, 0, 150));
  analogWrite(Mo[1][2], map(Dpow, 0, 100, 0, 150));
}
void motorDirecction(int I, int D, int Pow) {
  motorsOff();
  digitalWrite(Mo[0][I], 1);
  digitalWrite(Mo[1][D], 1);
  analogWrite(Mo[0][2], map(Pow, 0, 100, 0, 255));
  analogWrite(Mo[1][2], map(Pow, 0, 100, 0, 255));
}
void antiCollision() {
  if (Data[0] > 15 && Data[0] < 30  && Data[1] > 20) {
    motorDirecction(1, 0, 60);
  }
  else if (Data[0] < 15 && Data[1] > 20) {
    Pot[1] += 1;
    Pot[0] -= 1;
  } else if (Data[0] > 20 && Data[1] > 20) {
    Pot[0] += 1;
    Pot[1] -= 1;
  } else {
    if (flag) {
      motorDirecction(1, 0, 100);
      delay(120);
      flag = !flag;
    }
    else {
      motorDirecction(0, 0, 100);
      delay(64);
    }
    motorDirecction(0, 1, 100);
    delay(128);
  }
  motorDirecction(1, 1, Pot[0], Pot[1]);
  delay(2);
}
/* Batt state */
void battState() {
  // The intensity of the led depends on the load
  analogWrite(battLED, map(Data[2], 0, 110, 0, 200));
  // We check if the battery is at least 80% charge
  if (Data[2] > 98)
    Data[Cdatos + 1] = 30 + (100 - Data[2]) * 1;
  else if (Data[2] > 90)
    Data[Cdatos + 1] = 30 + (100 - Data[2]) * 2;
  else
    Data[Cdatos + 1] = 100;
}
void PID_0(float distance) {
  if (distance >= maximumRange || distance <= minimumRange) {
    /* "Out of range" signal */
    Serial.println("-1");
  }
  else {
    if (drive > -5 && drive < 5)
    {
      error = sp - actual;
      integral = integral + (error);  // Integral value
      derivative = (error - previous_error); // Derivative value
      drive = (error * kp) + (integral * ki) + (derivative * kd); // PID calculation
      previous_error = error;
    } else
    {
      error = sp - actual;
      drive = (error * kp) + (integral * ki) + (derivative * kd);
      previous_error = error;
    }
    if (drive > 5)
    {
      drive  = 5; // 5 as our range value to drive the PWM
    }
    else if (drive < -5)
    {
      drive = -5;
    }
    if (drive < 0)
    {
      motorDirecction(1, 1, map(drive, -5, 0, 90, 0));
    }
    else
    {
      motorDirecction(0, 0, map(drive, 0, 5, 0, 90));
    }
    actual = distance;
  }
  delay(10);
}
void choose() {
  if (Distancia(1) < 15 ) {
    if (Distancia(1) < 10) {
      motorDirecction(0, 0, 80);
      delay(1024);
    }
    motorDirecction(1, 0, 0);
    int distI = Distancia(0);
    motorDirecction(1, 0, 80);
    delay(256);
    motorDirecction(1, 0, 0);
    if (Distancia(1) < distI) {
      motorDirecction(0, 1, 80);
      delay(512);
    }
  } else {
    motorDirecction(1, 1, 70);
    delay(8);
  }
}
//--------/  Loop  /--------//
void loop() {
  readData();
  sendData();
  battState();
  // Test our callouts
  int dataInSerial = 0;
  switch (int(Data[Cdatos + dataInSerial ])) {
    // Case using sensors to detect objects and try to
    case 1:
      antiCollision();
      break;
    case 2:
      PID_0(Data[1]);
      break;
    case 3:
      choose();
      break;
    case 5:
      motorDirecction(0, 0, 100);
      break;
    case 8:
      motorDirecction(1, 1, 100);
      break;
    case 4:
      motorDirecction(0, 1, 100, 100);
      break;
    case 6:
      motorDirecction(1, 0, 100, 100);
      break;
    default:
      motorsOff();
      break;
  }
  // if we try to move in any direction just be able to do it 100us
  if (int(Data[Cdatos + dataInSerial ]) > 3 && int(Data[Cdatos + dataInSerial ] < 10)) {
    delay(700);
    Data[Cdatos + dataInSerial ] = 0;
  }
}
