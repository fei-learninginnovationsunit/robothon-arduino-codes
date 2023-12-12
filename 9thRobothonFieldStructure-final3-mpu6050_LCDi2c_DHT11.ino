#include <Wire.h>
#include <MPU6050_light.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Define pin configurations for DHT11 and ultrasonic sensor
#define DHTPIN 9       // DHT11 sensor data pin
#define DHTTYPE DHT11  // DHT11 sensor type
#define TRIGGER_PIN 10  // Ultrasonic sensor trigger pin
#define ECHO_PIN 11     // Ultrasonic sensor echo pin

// Create instances of DHT and LiquidCrystal_I2C
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Create an instance of MPU6050
MPU6050 mpu(Wire);

void setup() {
  // Initialize I2C communication and LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("Loading Sensors");

  // Set up pins for ultrasonic sensor
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize MPU6050 and display status on LCD
  byte status = mpu.begin();
  lcd.setCursor(0, 8);
  lcd.print(status);

  // Stop everything if could not connect to MPU6050
  while (status != 0) {}
  delay(500);

  // Calculate offsets for gyro and accelerometer
  mpu.calcOffsets();

  // Display "Done!" on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Done!");
  delay(500);
}

void loop() {
  // Wait for an object within 5 cm using ultrasonic sensor
  while (getDistance() > 5) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("9th Robothon");
    lcd.setCursor(0, 1);
    lcd.print("Weather Station");
    delay(500);
  }

  // Display "Object detected!" on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Object detected!");
  delay(500);
  lcd.clear();
  lcd.setCursor(0, 0);

  // Collect temperature and humidity data from DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Display temperature and humidity on LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("°C");
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print("%");
  delay(500);

  // Activate MPU6050 and start the loop
  while (true) {
    mpu.update();

    static int prevAngleZ = 0;
    int currentAngleZ = abs(static_cast<int>(mpu.getAngleZ()));

    // Update LCD only if there is a change in the value
    if (currentAngleZ != prevAngleZ) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Degrees:");
      lcd.print(currentAngleZ);
      Serial.println(currentAngleZ);

      // Update the previous value
      prevAngleZ = currentAngleZ;
    }

    delay(250);
  }
}

// Function to get distance from ultrasonic sensor
float getDistance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;

  return distance;
}
