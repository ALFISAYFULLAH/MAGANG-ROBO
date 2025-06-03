#include <Ps3Controller.h>
#include <Servo.h>

Servo Gripper;

enum MotorPosition { FL, FR, RL, RR, MOTOR_COUNT };
struct Motor { int aPin, bPin, enPin; };

// ESP32 pin assignments (PWM-capable pins for enPin and enLifter)
Motor robotMotors[] = {{15, 2, 13}, {4, 16, 14}, {17, 5, 25}, {18, 19, 26}};
const int Gripper_Pin = 27, in1Lifter = 32, in2Lifter = 33, enLifter = 12;

// Gripper servo state
int gripperAngle = 0; // Current servo angle (0 to 90)
const int gripperSpeed = 1; // Degrees per loop for smooth movement

void setup() {
  // Initialize motor and lifter pins
  for (int i = 0; i < MOTOR_COUNT; i++) {
    pinMode(robotMotors[i].aPin, OUTPUT);
    pinMode(robotMotors[i].bPin, OUTPUT);
    pinMode(robotMotors[i].enPin, OUTPUT);
  }
  pinMode(in1Lifter, OUTPUT);
  pinMode(in2Lifter, OUTPUT);
  pinMode(enLifter, OUTPUT);
  
  // Initialize servo
  Gripper.attach(Gripper_Pin);
  Gripper.write(gripperAngle);
  
  // Initialize serial for debugging
  Serial.begin(115200);
  
  // Initialize PS3 controller (replace with your ESP32's Bluetooth MAC address)
  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin("01:02:03:04:05:06");
  
  Serial.println("Ready.");
}

void onConnect() {
  Serial.println("Controller connected.");
}

void notify() {
  // Battery status feedback
  static int battery = -1;
  if (battery != Ps3.data.status.battery) {
    battery = Ps3.data.status.battery;
    Serial.print("Battery: ");
    if (battery == ps3_status_battery_charging) Serial.println("charging");
    else if (battery == ps3_status_battery_full) Serial.println("FULL");
    else if (battery == ps3_status_battery_high) Serial.println("HIGH");
    else if (battery == ps3_status_battery_low) Serial.println("LOW");
    else if (battery == ps3_status_battery_dying) Serial.println("DYING");
    else if (battery == ps3_status_battery_shutdown) Serial.println("SHUTDOWN");
    else Serial.println("UNDEFINED");
  }
}

void setMotorSpeed(int motorIdx, int speed) {
  // Speed: -255 to 255, negative for reverse
  bool direction = speed >= 0;
  int absSpeed = abs(speed);
  digitalWrite(robotMotors[motorIdx].aPin, direction);
  digitalWrite(robotMotors[motorIdx].bPin, !direction);
  analogWrite(robotMotors[motorIdx].enPin, absSpeed);
}

void updateMotors() {
  // Read joystick values (0 to 255, centered at ~128)
  int xAxis = Ps3.data.analog.stick.lx; // Left: 0, Right: 255
  int yAxis = Ps3.data.analog.stick.ly; // Up: 0, Down: 255

  // Apply dead zone and center joystick
  const int deadZone = 20;
  int xCentered = xAxis - 128; // -128 to 127
  int yCentered = yAxis - 128; // -128 to 127
  if (abs(xCentered) < deadZone) xCentered = 0;
  if (abs(yCentered) < deadZone) yCentered = 0;

  // Map joystick to velocities
  int vx = map(xCentered, -128, 127, -255, 255); // Strafe: left (-255) to right (255)
  int vy = map(yCentered, -128, 127, -255, 255); // Forward (-255) to backward (255)

  // Rotation from L1 (CCW) or R1 (CW)
  int omega = 0;
  if (Ps3.data.button.l1) omega = 100; // Counterclockwise
  else if (Ps3.data.button.r1) omega = -100; // Clockwise

  // Calculate omni wheel speeds (assuming square layout, r=1 for scaling)
  int v_FL = vx + vy + omega; // Front-left
  int v_FR = -vx + vy - omega; // Front-right
  int v_RL = -vx + vy + omega; // Rear-left
  int v_RR = vx + vy - omega; // Rear-right

  // Clamp speeds to -255 to 255
  v_FL = constrain(v_FL, -255, 255);
  v_FR = constrain(v_FR, -255, 255);
  v_RL = constrain(v_RL, -255, 255);
  v_RR = constrain(v_RR, -255, 255);

  // Apply speeds to motors
  setMotorSpeed(FL, v_FL);
  setMotorSpeed(FR, v_FR);
  setMotorSpeed(RL, v_RL);
  setMotorSpeed(RR, v_RR);

  // Debug output
  if (xCentered != 0 || yCentered != 0 || omega != 0) {
    Serial.printf("vx: %d, vy: %d, omega: %d | FL: %d, FR: %d, RL: %d, RR: %d\n", 
                  vx, vy, omega, v_FL, v_FR, v_RL, v_RR);
  } else if (!Ps3.data.button.square && !Ps3.data.button.cross && !Ps3.data.button.l3 && !Ps3.data.button.r3) {
    Serial.println("Motors stopped");
  }
}

void updateLifterAndGripper() {
  // Lifter control (Square: ANGKAT, Cross: TURUN)
  if (Ps3.data.button.square) {
    digitalWrite(in1Lifter, HIGH);
    digitalWrite(in2Lifter, LOW);
    analogWrite(enLifter, 255);
    Serial.println("Naik");
  } else if (Ps3.data.button.cross) {
    digitalWrite(in1Lifter, LOW);
    digitalWrite(in2Lifter, HIGH);
    analogWrite(enLifter, 255);
    Serial.println("Turun");
  } else {
    analogWrite(enLifter, 0);
    if (!Ps3.data.button.l3 && !Ps3.data.button.r3 && Ps3.data.analog.stick.lx == 128 && Ps3.data.analog.stick.ly == 128) {
      Serial.println("Lifter stopped");
    }
  }

  // Gripper control (L3: CAPIT, R3: LEPAS)
  if (Ps3.data.button.l3 && gripperAngle < 90) {
    gripperAngle += gripperSpeed;
    gripperAngle = constrain(gripperAngle, 0, 90);
    Gripper.write(gripperAngle);
    Serial.println("Nyapit: " + String(gripperAngle));
  } else if (Ps3.data.button.r3 && gripperAngle > 0) {
    gripperAngle -= gripperSpeed;
    gripperAngle = constrain(gripperAngle, 0, 90);
    Gripper.write(gripperAngle);
    Serial.println("Lepas: " + String(gripperAngle));
  }
}

void loop() {
  if (Ps3.isConnected()) {
    updateMotors(); // Update motor speeds based on joystick and rotation
    updateLifterAndGripper(); // Update lifter and gripper based on button hold
  }
  delay(10); // Small delay for smooth control
}
