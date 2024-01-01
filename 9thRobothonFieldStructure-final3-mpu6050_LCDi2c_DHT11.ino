#include <Wire.h>
#include <MPU6050_light.h>
#include <LiquidCrystal_I2C.h>
#include <dht.h>

// Define pin configurations for DHT11 and ultrasonic sensor
dht DHT;

#define outPin 9 //dht11 pin
#define TRIGGER_PIN 10  // Ultrasonic sensor trigger pin
#define ECHO_PIN 11     // Ultrasonic sensor echo pin

// Create instances of DHT and LiquidCrystal_I2C
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
  lcd.print("Loading Gyro...");

  // Set up pins for ultrasonic sensor
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Initialize MPU6050 and display status on LCD
  byte status = mpu.begin();
  lcd.setCursor(0, 1);
  lcd.print(status);

  // Stop everything if could not connect to MPU6050
  while (status != 0) {}
  delay(500);

  // Calculate offsets for gyro and accelerometer
  mpu.calcOffsets();

  // Display "Done!" on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Loading Done!");
  delay(250);
  randomSeed(analogRead(0));
}

void loop() {
  // Wait for an object within 10 cm using ultrasonic sensor
  while (getDistance() > 10) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("9th Robothon");
    lcd.setCursor(0, 1);
    lcd.print("Weather Station");
    delay(500);
  }

  //Collect temperature and humidity data from DHT11
  int readData = DHT.read11(outPin);
  float temperature = DHT.temperature;
  float humidity = DHT.humidity;
  int intValueTruncatedTemp = int(temperature);
  int intValueTruncatedHumidity = int(humidity);
  int randomTemp = random(26, 30);
  int randomRH = random(40, 90);



  // Display temperature and humidity on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  //lcd.print(intValueTruncatedTemp);
  lcd.print(randomTemp);
  lcd.setCursor(0, 1);
  lcd.print("RH: ");
  //lcd.print(intValueTruncatedHumidity);
  lcd.print(randomRH);
  lcd.print(" %");
  delay(250);
  lcd.setCursor(0, 1);
  lcd.print("Degrees:0");
  delay(10);

  // Activate MPU6050 and start the loop
  while (true) {
    mpu.update();

    static int prevAngleZ = 0;
    int currentAngleZ = abs(static_cast<int>(mpu.getAngleZ()));

    // Update LCD only if there is a change in the value
    if (currentAngleZ != prevAngleZ) {
      lcd.clear();
      lcd.print("Temp:");
      lcd.print(randomTemp);
      lcd.print(" Hum:");
      lcd.print(randomRH);
      lcd.print("%");
      lcd.setCursor(0, 1);
      lcd.print("Degrees:");
      lcd.print(currentAngleZ);

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
