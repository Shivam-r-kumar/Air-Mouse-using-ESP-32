#include <Wire.h>
#include <BleMouse.h>

BleMouse bleMouse(std::string("ESP32 Air Mouse"));

// MPU6500 I2C address
const uint8_t MPU_ADDR = 0x68;

// MPU registers
#define REG_WHO_AM_I     0x75
#define REG_PWR_MGMT_1   0x6B
#define REG_PWR_MGMT_2   0x6C
#define REG_CONFIG       0x1A
#define REG_GYRO_CONFIG  0x1B
#define REG_ACCEL_CONFIG 0x1C
#define REG_ACCEL_XOUT_H 0x3B
#define REG_GYRO_XOUT_H  0x43

float accel_scale = 4096.0;
float gyro_scale  = 65.5;
float sensitivity = 30.0;

// I2C write
void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

// I2C read 1 byte
uint8_t readReg(uint8_t reg) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)1);

  if (Wire.available()) return Wire.read();
  return 0xFF;
}

// I2C read 16-bit
int16_t read16(uint8_t reg) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_ADDR, (uint8_t)2);
  while (Wire.available() < 2);

  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();
  return (int16_t)((hi << 8) | lo);
}

// INIT MPU6500
void setupSensor() {
  writeReg(REG_PWR_MGMT_1, 0x00);
  delay(50);
  writeReg(REG_PWR_MGMT_1, 0x01);
  delay(20);
  writeReg(REG_CONFIG, 0x03);
  writeReg(REG_GYRO_CONFIG, 0x08);
  writeReg(REG_ACCEL_CONFIG, 0x10);
  writeReg(REG_PWR_MGMT_2, 0x00);
  delay(20);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Wire.begin(21, 22);     // ESP32 I2C Pins
  Serial.println("MPU6500 + BLE Mouse Test");

  uint8_t who = readReg(REG_WHO_AM_I);
  Serial.print("WHO_AM_I: 0x");
  Serial.println(who, HEX);

  if (who != 0x70 && who != 0x68) {
    Serial.println("Unexpected WHO_AM_I! Check sensor model or wiring.");
    while(1) delay(100);
  }

  setupSensor();

  Serial.println("Starting BLE Mouse...");
  bleMouse.begin();
}

void loop() {

  // ---------------- ACCEL ------------------
  int16_t ax = read16(REG_ACCEL_XOUT_H);
  int16_t ay = read16(REG_ACCEL_XOUT_H + 2);
  int16_t az = read16(REG_ACCEL_XOUT_H + 4);

  float ax_g = (float)ax / accel_scale;
  float ay_g = (float)ay / accel_scale;
  float az_g = (float)az / accel_scale;

  // ---------------- GYRO ------------------
  int16_t gx = read16(REG_GYRO_XOUT_H);
  int16_t gy = read16(REG_GYRO_XOUT_H + 2);
  int16_t gz = read16(REG_GYRO_XOUT_H + 4);

  float gx_dps = (float)gx / gyro_scale;
  float gy_dps = (float)gy / gyro_scale;
  float gz_dps = (float)gz / gyro_scale;

  // ---------------- BLE MOUSE CONTROL ------------------
  if (bleMouse.isConnected()) {
    int moveX = (int)(ax_g * sensitivity);
    int moveY = (int)(ay_g * sensitivity);
    bleMouse.move(moveX, -moveY);
  }

  // Shake click
  if (bleMouse.isConnected() && az_g > 1.5) {
    bleMouse.click(MOUSE_LEFT);
    delay(200);
  }

  // Debug Print
  Serial.print("Accel: X=");
  Serial.print(ax_g,3);
  Serial.print(" Y=");
  Serial.print(ay_g,3);
  Serial.print(" Z=");
  Serial.println(az_g,3);

  Serial.print("Gyro: X=");
  Serial.print(gx_dps,3);
  Serial.print(" Y=");
  Serial.print(gy_dps,3);
  Serial.print(" Z=");
  Serial.println(gz_dps,3);

  Serial.println("---------------------------");
  delay(50);
}
