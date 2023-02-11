// Include the library
#include <SoftwareSerial.h>
#include <DHT.h>  
#include <Wire.h>  
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <NewPing.h>
#include <MQ135.h>
#include <SPI.h>

#define TRIGGER_PIN 5
#define ECHO_PIN 4
#define MAX_DISTANCE 100 // Maximum distance (in cm) that the ultrasonic sensor can measure
#define SERVO_PIN 10

Servo servo;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

#define DHT_TYPE DHT11   // Define the type of the DHT sensor
const int DHT_PIN = 2;   // Pin where the DHT sensor is connected
const int FAN_PIN = 13;  // Pin where the fan is connected
const int BULB_PIN = 12; // Pin where the bulb is connected
int sensorPin = A0;      // select the input pin for the water level sensor
int motorPin = 11;       // select the output pin for the motor

#define MQ135_PIN 3     // Pin for MQ135 sensor
#define RLOAD 1.0
#define r0Air 1
#define scaleFactorNH4 102.694
#define exponentNH4 2.48818
#define atmNH4 15

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int TEMP_THRESHOLD_HIGH = 30;  // Threshold temperature for turning on the fan
const int TEMP_THRESHOLD_LOW = 18;   // Threshold temperature for turning on the bulb

DHT dht(DHT_PIN, DHT_TYPE);  // Create an instance of the DHT library
MQ135 gasSensor = MQ135(MQ135_PIN, RLOAD, r0Air);

void setup() {
  Serial.begin(9600);       // Initialize serial communication
  lcd.begin();                // initialize the lcd 
  lcd.backlight();            // turn on the backlight
  pinMode(FAN_PIN, OUTPUT);   // Set the fan pin as an output
  pinMode(BULB_PIN, OUTPUT);  // Set the bulb pin as an output
  pinMode(sensorPin, INPUT);  // set the sensor pin as an input
  pinMode(motorPin, OUTPUT);  // set the motor pin as an output
  servo.attach(SERVO_PIN);    //connect servo motor
  dht.begin();                // Initialize the DHT sensor
}

void loop() {
  // Read the temperature and humidity from the DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // read the water level sensor value
  int sensorValue = analogRead(sensorPin);
  
  // Measure the distance of feed box
  int feedlevel =  sonar.ping_cm();

  // Measure the feedlevel in percentage
  if (feedlevel <= 9) {
    feedlevel = ((11-feedlevel*0)/11)*100;
  }
  else if (feedlevel == 10) {
    feedlevel = ((11-feedlevel*0.5)/11)*100;
  }
  else if (feedlevel >= 11) {
    feedlevel = ((11-feedlevel)/11)*100;      
  }

  // Measure NH4 levels
  int val1 = analogRead(MQ135_PIN);
  int val = 74;  
  float resistance = ((1024/(float)val) * 5 - 1)* RLOAD; 
  float r0NH4 = resistance * pow((atmNH4/scaleFactorNH4), (1./exponentNH4));
  float NH4=scaleFactorNH4 * pow((resistance/r0NH4), -exponentNH4);

  // convert the sensor value to a water level percentage
  int waterlevel = map(sensorValue, 0, 1023, 0, 100);

  // Print the values to the serial monitor
  Serial.print("\n");
  Serial.print(temperature);
  Serial.print(" ");
  Serial.print(humidity);
  Serial.print(" ");
  Serial.print(waterlevel);
  Serial.print(" ");
  Serial.print(feedlevel);
  Serial.print(" ");
  Serial.print(NH4);
  Serial.print(" ");

  // Display the values on the LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("*C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");
  delay(3000);
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Water Level: ");
  lcd.print(waterlevel);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Feed Level: ");
  lcd.print(feedlevel);
  lcd.print("%");  
  delay(3000);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("NH4: ");
  lcd.print(NH4);
  lcd.print("ppm");

  // Control the fan based on the temperature
  if (temperature > TEMP_THRESHOLD_HIGH) 
  {
    digitalWrite(FAN_PIN, HIGH);  // Turn on the fan
  } 
  else
  {
    digitalWrite(FAN_PIN, LOW);  // Turn off the fan
  }

  // Control the bulb based on the temperature
  if (temperature < TEMP_THRESHOLD_LOW) {
    digitalWrite(BULB_PIN, HIGH);  // Turn on the bulb
  } 
  else
  {
    digitalWrite(BULB_PIN, LOW);
  }

  // start the motor if the water level is less than 20%
  if (waterlevel <= 20) {
    digitalWrite(motorPin, HIGH);
  } 
  else {
    digitalWrite(motorPin, LOW);
  }

  // start the servo motor if the feed level is less than 20%
  if (feedlevel <= 20) { 
    servo.write(20); // Rotate the servo to the 90 degree position
    delay(3000);
    servo.write(160); // Rotate the servo back to the 0 degree position
  }
  else{
    servo.write(160); // Rotate the servo back to the 0 degree position
  }
  delay(1000);
}
