#include <IBusBM.h>
IBusBM IBus; 
uint16_t arm = 0;
uint16_t arm_n = 0;
uint16_t steering_l = 0;
uint16_t steering_r = UINT16_MAX;
uint16_t steering = 0;
bool isArmed = false;

#include <I2Cdev.h>
#include <MPU6050.h>
#include <Wire.h>
MPU6050 accelgyro;
int16_t gx, gy, gz, gxn, gyn, gzn;
int16_t r; // rotate
int16_t rn; // rotate neutral
float rd;  // rotate delta
float rdr = 0.0001; // rotate delta rate
float sr = -0.11; // steering rate

#include <Servo.h>
Servo servo;
int servo_out;
#define SERVO_MIN 49
#define SERVO_MAX 151
int sn = abs(SERVO_MIN - SERVO_MAX) / 2;
int sadj;

void setup() {
//  while (!Serial) { /*pro micro*/ }
//  Serial.begin(115200);

  IBus.begin(Serial1);

  Wire.begin();
  accelgyro.initialize();
//  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
//  Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
//  Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
//  Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
//  Serial.print("\n");
//  accelgyro.setXGyroOffset(220);
//  accelgyro.setYGyroOffset(76);
//  accelgyro.setZGyroOffset(-85);
//  Serial.print("\n");

  servo.attach(9);
}

void loop() {
  steering = IBus.readChannel(0);
  accelgyro.getRotation(&gx, &gy, &gz);
  if (!isArmed) {
    arm = IBus.readChannel(3);
    if (arm != 0) {
      if (!arm_n) { arm_n = arm; }
    }
    if (steering != 0) {
      steering_l = max(steering, steering_l);
      steering_r = min(steering, steering_r);
      servo_out = map(steering, steering_r, steering_l, SERVO_MIN, SERVO_MAX);
      servo.write(servo_out);
    }
//    Serial.print(steering); Serial.print("\t");
//    Serial.print(steering_l); Serial.print("\t");
//    Serial.print(steering_r); Serial.print("\t");
//    Serial.print(arm); Serial.print("\t");
//    Serial.println();
    if (arm - arm_n >= 800) {
      gxn = gx;
      gyn = gy;
      gzn = gz;
      isArmed = true;
    }
  }
  else {
    servo_out = map(steering, steering_r, steering_l, SERVO_MIN, SERVO_MAX);
    r = gx;
    rn = gxn;
    rd = (r - rn) * rdr;
    sadj = rd * sn * sr;
    servo.write(servo_out + sadj);
//    Serial.print(servo_out); Serial.print("\t");
//    Serial.print(gx); Serial.print("\t");
//    Serial.print(gy); Serial.print("\t");
//    Serial.print(gz); Serial.print("\t");
//    Serial.print(rd); Serial.print("\t");
//    Serial.print(sa); Serial.print("\t");
//    Serial.println();
  }
//  delay(50);
}
