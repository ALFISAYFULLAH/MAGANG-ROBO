# Dokumentasi Proyek Robot Kontrol PS3

## 📌 Deskripsi Proyek
Proyek ini mengimplementasikan kontrol robot 4 roda omnidirectional menggunakan PS3 controller via Bluetooth. Robot dilengkapi dengan:
- 4 motor DC dengan kontrol kecepatan PWM
- Sistem gripper servo
- Mekanisme lifter/angkat

## 🛠️ Komponen Utama
- **Mikrokontroler**: ESP32
- **Driver Motor**: L298N (4 unit)
- **Motor DC**: 4 unit
- **Servo Motor**: 1 unit (gripper)
- **PS3 Controller**: via Bluetooth

## 📋 Spesifikasi Pin ESP32
| Komponen       | Pin ESP32       |
|----------------|-----------------|
| Motor FL       | 15 (a), 2 (b), 13 (en) |
| Motor FR       | 4 (a), 16 (b), 14 (en) |
| Motor RL       | 17 (a), 5 (b), 25 (en) |
| Motor RR       | 18 (a), 19 (b), 26 (en) |
| Gripper Servo  | 27              |
| Lifter IN1     | 32              |
| Lifter IN2     | 33              |
| Lifter EN      | 12              |

## 🎮 Konfigurasi Kontrol PS3
### **Gerakan Dasar**
- **Analog Kiri**:
  - Atas/Bawah: Maju/Mundur
  - Kiri/Kanan: Geser kiri/kanan
- **L1/R1**: Rotasi CCW/CW

### **Kontrol Aktuator**
| Tombol       | Aksi               |
|--------------|--------------------|
| SQUARE (□)   | Angkat lifter      |
| CROSS (X)    | Turunkan lifter    |
| L3           | Tutup gripper      |
| R3           | Buka gripper       |

## ⚙️ Fungsi Utama
```cpp
void setMotorSpeed(int motorIdx, int speed) 
```
- Mengatur kecepatan motor (-255 sampai 255)

```cpp 
void updateMotors()
```
- Mengolah input joystick menjadi kecepatan motor

```cpp
void updateLifterAndGripper() 
```
- Mengontrol aktuator berdasarkan input tombol

## 🔧 Instalasi
1. Sambungkan komponen sesuai diagram pin
2. Upload sketch ke ESP32
3. Hubungkan PS3 controller via Bluetooth
   ```cpp
   Ps3.begin("01:02:03:04:05:06"); // Ganti dengan MAC ESP32
   ```

## 📊 Logika Kontrol Motor
```
v_FL =  vx + vy + omega  
v_FR = -vx + vy - omega
v_RL = -vx + vy + omega
v_RR =  vx + vy - omega
```
Dimana:
- `vx`: Gerakan lateral (-255 kiri, 255 kanan)
- `vy`: Gerakan maju/mundur (-255 mundur, 255 maju)
- `omega`: Rotasi (positif CCW, negatif CW)

## 🚀 Fitur Tambahan
- **Deadzone joystick** (20 unit) untuk menghindari drift
- **Gerakan halus gripper** dengan increment bertahap
- **Feedback baterai** controller via serial monitor

## ⚠️ Catatan Penting
1. Pastikan ESP32 mendukung Bluetooth
2. Ubah MAC address sesuai perangkat
3. Gunakan power supply yang memadai untuk motor
4. Kalibrasi deadzone sesuai kebutuhan

## 📜 Lisensi
Proyek ini menggunakan lisensi MIT. Lihat file LICENSE untuk detail lengkap.

---

Dokumentasi ini terakhir diperbarui pada 3 Juni 2023. Untuk pertanyaan atau masalah, silakan buka issue di repositori proyek.
