#include <Servo.h>
#include <Arduino.h>


Servo Gripper;

enum MotorPosition { FL, FR, RL, RR, MOTOR_COUNT };
struct Motor { int aPin, bPin, enPin; };

Motor robotMotors[] = { {7, 8, 3}, {A0, A1, 10}, {A5, A4, 9}, {12, 13, 11} };
const int Gripper_Pin = 6, in1Lifter = 2, in2Lifter = 4, enLifter = 5;

void setup() {
  for (int i = 0; i < MOTOR_COUNT; i++) {
    pinMode(robotMotors[i].aPin, OUTPUT);
    pinMode(robotMotors[i].bPin, OUTPUT);
    pinMode(robotMotors[i].enPin, OUTPUT);
  }
  pinMode(in1Lifter, OUTPUT);
  pinMode(in2Lifter, OUTPUT);
  pinMode(enLifter, OUTPUT);
  Gripper.attach(Gripper_Pin);
  Gripper.write(0);
  Serial.begin(9600);
  Serial.println("Commands: F, B, L, R, TR, TL, S, SL, SR, LB, RB, ANGKAT, TURUN, CAPIT, LEPAS");
}

void loop() {
  if (Serial.available() <= 0) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim(); cmd.toUpperCase();
  Serial.println(cmd);

  // Matriks: {RR_dir, RR_spd, RL_dir, RL_spd, FR_dir, FR_spd, FL_dir, FL_spd}
  int motorConfig[][8] = {
    {1, 255, 1, 255, 1, 255, 1, 255}, // F
    {0, 255, 0, 255, 0, 255, 0, 255}, // B
    {0, 0,   1, 255, 1, 255, 0, 0},   // L
    {1, 255, 0, 0,   0, 0,   1, 255}, // R
    {0, 255, 1, 255, 0, 255, 1, 255}, // TR
    {1, 255, 0, 255, 1, 255, 0, 255}, // TL
    {0, 0,   0, 0,   0, 0,   0, 0},   // S
    {0, 255, 1, 255, 1, 255, 0, 255}, // SL
    {1, 255, 0, 255, 0, 255, 1, 255}, // SR
    {0, 0,   0, 255, 0, 255, 0, 0},   // LB
    {0, 255, 0, 0,   0, 0,   0, 255}  // RB
  };

  // Peta perintah ke indeks
  String commands[] = {"F", "B", "L", "R", "TR", "TL", "S", "SL", "SR", "LB", "RB"};
  int idx = -1;
  for (int i = 0; i < 11; i++) {
    if (cmd == commands[i]) { idx = i; break; }
  }

  if (idx >= 0) {
    for (int i = 0; i < MOTOR_COUNT; i++) {
      digitalWrite(robotMotors[i].aPin, motorConfig[idx][i*2]);
      digitalWrite(robotMotors[i].bPin, !motorConfig[idx][i*2]);
      analogWrite(robotMotors[i].enPin, motorConfig[idx][i*2+1]);
    }
    Serial.println("Moving: " + cmd);
  }
  
  else if (cmd == "ANGKAT" || cmd == "TURUN") {
    digitalWrite(in1Lifter, cmd == "ANGKAT");
    digitalWrite(in2Lifter, cmd == "TURUN");
    analogWrite(enLifter, 255);
    Serial.println(cmd == "ANGKAT" ? "Naik" : "Turun");
    delay(2000);
    analogWrite(enLifter, 0);
    Serial.println("Stopped");
  }
  
  else if (cmd == "CAPIT") { Gripper.write(90); Serial.println("Nyapit"); }
  else if (cmd == "LEPAS") { Gripper.write(0); Serial.println("Lepas"); }
}
