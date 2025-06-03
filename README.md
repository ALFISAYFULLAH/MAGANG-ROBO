# MAGANG-ROBO

# Penjelasan Kode Robot

Kode ini mengendalikan robot dengan 4 motor DC (depan kiri, depan kanan, belakang kiri, belakang kanan), sebuah gripper servo, dan sebuah motor lifter.

## Struktur Kode

### 1. Library dan Deklarasi Awal
```cpp
#include <Servo.h>
#include <Arduino.h>

Servo Gripper;
```
- Menginclude library Servo untuk mengontrol gripper
- Mendeklarasikan objek Servo bernama `Gripper`

### 2. Enum dan Struct untuk Motor
```cpp
enum MotorPosition { FL, FR, RL, RR, MOTOR_COUNT };
struct Motor { int aPin, bPin, enPin; };
```
- `MotorPosition`: Enum untuk posisi motor (Front Left, Front Right, Rear Left, Rear Right)
- `Motor`: Struct untuk menyimpan pin motor (pin A, pin B, dan pin enable)

### 3. Deklarasi Pin
```cpp
Motor robotMotors[] = { {7, 8, 3}, {A0, A1, 10}, {A5, A4, 9}, {12, 13, 11} };
const int Gripper_Pin = 6, in1Lifter = 2, in2Lifter = 4, enLifter = 5;
```
- Konfigurasi pin untuk 4 motor
- Pin untuk gripper (6) dan lifter (2,4,5)

### 4. Fungsi Setup
```cpp
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
```
- Mengatur semua pin motor sebagai OUTPUT
- Mengattach servo gripper ke pinnya dan set ke posisi 0
- Memulai komunikasi serial dan menampilkan daftar perintah

### 5. Fungsi Loop Utama
```cpp
void loop() {
  if (Serial.available() <= 0) return;
  String cmd = Serial.readStringUntil('\n');
  cmd.trim(); cmd.toUpperCase();
  Serial.println(cmd);
```
- Mengecek input serial
- Membaca perintah, menghapus spasi, dan mengubah ke huruf besar
- Mencetak perintah yang diterima

### 6. Konfigurasi Motor
```cpp
  int motorConfig[][8] = {
    {1, 255, 1, 255, 1, 255, 1, 255}, // F (Maju)
    {0, 255, 0, 255, 0, 255, 0, 255}, // B (Mundur)
    {0, 0,   1, 255, 1, 255, 0, 0},   // L (Kiri)
    {1, 255, 0, 0,   0, 0,   1, 255}, // R (Kanan)
    {0, 255, 1, 255, 0, 255, 1, 255}, // TR (Putar Kanan)
    {1, 255, 0, 255, 1, 255, 0, 255}, // TL (Putar Kiri)
    {0, 0,   0, 0,   0, 0,   0, 0},   // S (Berhenti)
    {0, 255, 1, 255, 1, 255, 0, 255}, // SL (Geser Kiri)
    {1, 255, 0, 255, 0, 255, 1, 255}, // SR (Geser Kanan)
    {0, 0,   0, 255, 0, 255, 0, 0},   // LB (Belok Kiri)
    {0, 255, 0, 0,   0, 0,   0, 255}  // RB (Belok Kanan)
  };
```
- Matriks konfigurasi untuk semua perintah gerakan
- Setiap baris berisi 8 nilai: [arah RR, speed RR, arah RL, speed RL, ...]

### 7. Pemetaan Perintah
```cpp
  String commands[] = {"F", "B", "L", "R", "TR", "TL", "S", "SL", "SR", "LB", "RB"};
  int idx = -1;
  for (int i = 0; i < 11; i++) {
    if (cmd == commands[i]) { idx = i; break; }
  }
```
- Mencocokkan perintah dengan indeks di matriks konfigurasi

### 8. Eksekusi Perintah Motor
```cpp
  if (idx >= 0) {
    for (int i = 0; i < MOTOR_COUNT; i++) {
      digitalWrite(robotMotors[i].aPin, motorConfig[idx][i*2]);
      digitalWrite(robotMotors[i].bPin, !motorConfig[idx][i*2]);
      analogWrite(robotMotors[i].enPin, motorConfig[idx][i*2+1]);
    }
    Serial.println("Moving: " + cmd);
  }
```
- Jika perintah valid, atur motor sesuai konfigurasi
- Set arah dan kecepatan untuk setiap motor

### 9. Kontrol Lifter
```cpp
  else if (cmd == "ANGKAT" || cmd == "TURUN") {
    digitalWrite(in1Lifter, cmd == "ANGKAT");
    digitalWrite(in2Lifter, cmd == "TURUN");
    analogWrite(enLifter, 255);
    Serial.println(cmd == "ANGKAT" ? "Naik" : "Turun");
    delay(2000);
    analogWrite(enLifter, 0);
    Serial.println("Stopped");
  }
```
- "ANGKAT": Naikkan lifter
- "TURUN": Turunkan lifter
- Berhenti setelah 2 detik

### 10. Kontrol Gripper
```cpp
  else if (cmd == "CAPIT") { Gripper.write(90); Serial.println("Nyapit"); }
  else if (cmd == "LEPAS") { Gripper.write(0); Serial.println("Lepas"); }
}
```
- "CAPIT": Tutup gripper (90 derajat)
- "LEPAS": Buka gripper (0 derajat)

## Daftar Perintah
1. Gerakan Dasar:
   - F: Maju
   - B: Mundur
   - L: Geser kiri
   - R: Geser kanan
   - TR: Putar kanan
   - TL: Putar kiri
   - S: Berhenti

2. Gerakan Khusus:
   - SL: Geser kiri
   - SR: Geser kanan
   - LB: Belok kiri
   - RB: Belok kanan

3. Kontrol Aktuator:
   - ANGKAT: Naikkan lifter
   - TURUN: Turunkan lifter
   - CAPIT: Tutup gripper
   - LEPAS: Buka gripper

[###Link Simulasi Tinkercad]([URL-target](https://www.tinkercad.com/things/cUB3jR68jJM-fantastic-kieran))

