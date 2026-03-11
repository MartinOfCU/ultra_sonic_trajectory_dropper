#include <Wire.h>
#include <math.h>


const int BUTTON_PIN = 7; // Pin connected to the button
const int LED_PIN = 13; // Pin connected to the LED


// MPU6050 Registers
#define MPU6050_ADDR   0x68
#define ACCEL_XOUT_H   0x3B
#define GYRO_XOUT_H    0x43

// assist using https://controllerstech.com/mpu6050-arduino-tutorial/
// for conneting the I2C mpu6050 device

class MpuModule {
  public:
    // Scales
    float accScale = 16384.0;   // For ±2g (this value is 1g in raw data, always preset in Z so remove it using code)
    float gyroScale = 131.0;    // For ±250 °/s

    float ignoreDegreesChange = 0.1; // ignore values less then 0.1deg/s
    float ignoreAccelerationChange = 0.1; // ignore values less then 0.1g of change (0.98ms2)
      // Calibration Offsets
    long accX_offset = 0, accY_offset = 0, accZ_offset = 0;
    long gyroX_offset = 0, gyroY_offset = 0, gyroZ_offset = 0;

    float ax, ay, az;
    float gx, gy, gz;

    float relativeX, relativeY, relativeZ;
    float relativeAngX, relativeAngY, relativeAngZ;

    bool isUpsideDown = false;

    void printOutput() {
      // -------- Print Results --------
      Serial.print("Acc(g): ");
      Serial.print(this->ax); Serial.print(", ");
      Serial.print(this->ay); Serial.print(", ");
      Serial.println(this->az);

      Serial.print("Gyro(dps): ");
      Serial.print(this->gx); Serial.print(", ");
      Serial.print(this->gy); Serial.print(", ");
      Serial.println(this->gz);

      Serial.println("---------------------");
    }

    void printCalibrationOutput() {
      // -------- Print Results --------
      Serial.println("Calibration offsets: ");
      Serial.print("Acc(g): ");
      Serial.print(this->accX_offset); Serial.print(", ");
      Serial.print(this->accY_offset); Serial.print(", ");
      Serial.println(this->accZ_offset);

      Serial.print("Gyro(dps): ");
      Serial.print(this->gyroX_offset); Serial.print(", ");
      Serial.print(this->gyroY_offset); Serial.print(", ");
      Serial.println(this->gyroZ_offset);

      Serial.println("---------------------");
    }

    void printRelativeAngleOutput() {
      // -------- Print Results --------
      Serial.println("Relative Angle: ");
      Serial.print(this->relativeAngX); Serial.print(", ");
      Serial.print(this->relativeAngY); Serial.print(", ");
      Serial.println(this->relativeAngZ);

      Serial.println("---------------------");
    }

    void getMpuModuleData() {
      int16_t rawAccX, rawAccY, rawAccZ;
      int16_t rawGyroX, rawGyroY, rawGyroZ;

      // ------ Read Accelerometer ------
      Wire.beginTransmission(MPU6050_ADDR);
      Wire.write(ACCEL_XOUT_H);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU6050_ADDR, 6, true);

      rawAccX = (Wire.read() << 8) | Wire.read();
      rawAccY = (Wire.read() << 8) | Wire.read();
      rawAccZ = (Wire.read() << 8) | Wire.read();

      // Apply offsets
      rawAccX -= this->accX_offset;
      rawAccY -= this->accY_offset;
      rawAccZ -= this->accZ_offset;

      // Convert to g
      this->ax = rawAccX / this->accScale;
      this->ay = rawAccY / this->accScale;
      this->az = rawAccZ / this->accScale - 1;

      // ------ Read Gyroscope ------
      Wire.beginTransmission(MPU6050_ADDR);
      Wire.write(GYRO_XOUT_H);
      Wire.endTransmission(false);
      Wire.requestFrom(MPU6050_ADDR, 6, true);

      rawGyroX = (Wire.read() << 8) | Wire.read();
      rawGyroY = (Wire.read() << 8) | Wire.read();
      rawGyroZ = (Wire.read() << 8) | Wire.read();

      // Apply offsets
      rawGyroX -= this->gyroX_offset;
      rawGyroY -= this->gyroY_offset;
      rawGyroZ -= this->gyroZ_offset;

      // Convert to degrees/sec
      this->gx = rawGyroX / this->gyroScale;
      this->gy = rawGyroY / this->gyroScale;
      this->gz = rawGyroZ / this->gyroScale;

      // MpuModuleData output = {ax, ay, az, gx, gy, gz};
      // calculate relative angles using G forces and triganometry
      this->relativeAngX = asin(this->ax) * (180.0 / PI); // 0 is inline, 1 is at an angle, flat to the ground
      this->relativeAngY = asin(this->ay) * (180.0 / PI);
      this->relativeAngZ = asin(this->az) * (180.0 / PI);

      if (isnan(this->relativeAngX) || isnan(this->relativeAngY) || isnan(this->relativeAngZ)) {
        this->isUpsideDown = true;
      }
      else {
        this->isUpsideDown = false;
      }


      return;
    }

    // ---------------- Calibration Functions ---------------- //

    void calibrateAccelerometer() {
      long sumX = 0, sumY = 0, sumZ = 0;

      Serial.println("Calibrating Accelerometer... Do not move!");

      for (int i = 0; i < 200; i++) {
        Wire.beginTransmission(MPU6050_ADDR);
        Wire.write(ACCEL_XOUT_H);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU6050_ADDR, 6, true);

        sumX += (Wire.read() << 8) | Wire.read();
        sumY += (Wire.read() << 8) | Wire.read();
        sumZ += (Wire.read() << 8) | Wire.read();

        delay(5);
      }

      this->accX_offset = sumX / 200;
      this->accY_offset = sumY / 200;
      this->accZ_offset = (sumZ / 200) - 16384; // 1g adjustment

      Serial.println("Accelerometer Calibration Done.");
    }

    void calibrateGyroscope() {
      long sumX = 0, sumY = 0, sumZ = 0;

      Serial.println("Calibrating Gyroscope... Do not move!");

      for (int i = 0; i < 200; i++) {
        Wire.beginTransmission(MPU6050_ADDR);
        Wire.write(GYRO_XOUT_H);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU6050_ADDR, 6, true);

        sumX += (Wire.read() << 8) | Wire.read();
        sumY += (Wire.read() << 8) | Wire.read();
        sumZ += (Wire.read() << 8) | Wire.read();

        delay(5);
      }

      this->gyroX_offset = sumX / 200;
      this->gyroY_offset = sumY / 200;
      this->gyroZ_offset = sumZ / 200;

      Serial.println("Gyroscope Calibration Done.");
    }
};




// 




MpuModule* mpu = new MpuModule();

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Wake up MPU6050 – write 0 to power management register
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);
  Wire.write(0); 
  Wire.endTransmission();

  delay(1000);

  mpu->calibrateAccelerometer();
  mpu->calibrateGyroscope();

  mpu->printCalibrationOutput();

  // calibrateAccelerometer();
  // calibrateGyroscope();

  Serial.println("\nStarting Final Reading...\n");

  // LED for status and button for calibration reset
  pinMode(BUTTON_PIN, INPUT_PULLDOWN); // Configure button pin as input with internal pull-up resistor
  pinMode(LED_PIN, OUTPUT); // Configure LED pin as output
  digitalWrite(LED_PIN, LOW); // Turn off LED
}

void loop() {
  // TODO: make sure the calibration function only runs once on this button press.
  int buttonStateWaitEnd = false;
  int buttonState = digitalRead(BUTTON_PIN); // Read the button state
  if (buttonState == HIGH && buttonStateWaitEnd == false) { // Button pressed (LOW due to pull-up)
    digitalWrite(LED_PIN, HIGH); // Turn off LED
    mpu->calibrateAccelerometer();
    mpu->calibrateGyroscope();
  } 
  else {
    buttonStateWaitEnd = false;
  }
  digitalWrite(LED_PIN, LOW); // Turn off LED
  mpu->getMpuModuleData();
  mpu->printOutput();
  mpu->printRelativeAngleOutput();
  delay(1000);
}



