# 🖱️ ESP32 Air Mouse using MPU6500

This project turns an **ESP32** into a **wireless BLE Air Mouse** using the **MPU6500 IMU sensor**.  
It detects motion in 3D space (accelerometer + gyro) and moves the cursor accordingly.  
Shaking the sensor triggers a **left click**.

---

## 🧰 Components Used
- ESP32 Development Board
- MPU6500 / MPU6050 IMU Sensor (I2C)
- BLE-capable device (Laptop, Tablet, Phone)
- Jumper wires
- Power source for ESP32

---

## 🔌 Pin Connections

### ESP32 ↔ MPU6500
| ESP32 Pin | MPU6500 Pin |
|-----------|------------|
| 21 (SDA) | SDA        |
| 22 (SCL) | SCL        |
| 3.3V      | VCC        |
| GND       | GND        |

> I2C communication is used to read accelerometer and gyro data.

---

## ⚙️ Code Overview
- **BLE Mouse Setup**: `BleMouse` library handles Bluetooth HID connection
- **Sensor Initialization**: Configures MPU6500 registers via I2C
- **Motion Reading**:
  - Accelerometer: X, Y, Z → Controls cursor movement
  - Gyroscope: X, Y, Z → Optional for advanced control
- **Mouse Control**:
  - `moveX`, `moveY` are calculated from accelerometer readings
  - Shake detection (`az_g > 1.5`) triggers left mouse click

---

## 🔧 Adjustable Parameters
```cpp
float sensitivity = 30.0;   // Mouse movement sensitivity
float accel_scale = 4096.0; // Accelerometer scaling
float gyro_scale  = 65.5;   // Gyroscope scaling
