// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings

#define BLYNK_TEMPLATE_ID "TMPL3v_TPjlE"
#define BLYNK_DEVICE_NAME "Control Poultry Cage"
#define BLYNK_AUTH_TOKEN "6luXhSVMPLKZj0rXkf-7FBIlL0qixAQi"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include<SoftwareSerial.h> 

SoftwareSerial mySerial (D1, D2); //Rx, TX pins

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Sufyan";
char pass[] = "123456789";

BlynkTimer timer; 

void setup()
{
  // Debug console
  Serial.begin(115200);
  mySerial.begin(9600);
  
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}
// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}

void loop()
{
  Blynk.run();
  timer.run();
  
  String msg = mySerial.readStringUntil('\r');

  //Extract values from the string and store them in variables
  String values[5]; //create an array to store the values
  int i = 0;
  for (int j = 0; j < msg.length(); j++) {
    if (msg[j] == ' ' || msg[j] == '\r') { //check for a space or a newline character
      i++;
    } 
    else {
      values[i] += msg[j]; //add the current character to the current value string
    }
  }
  
  float temperature = values[0].toFloat();
  float humidity = values[1].toFloat();
  int waterlevel = values[2].toFloat();
  int feedlevel = values[3].toFloat();
  float NH4 = values[4].toFloat();

  if (NH4 != 0){
    // Print the temperature and humidity to the serial monitor("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Water Level: ");
    Serial.print(waterlevel);
    Serial.println(" %");
    Serial.print("Feed Level: ");
    Serial.print(feedlevel);
    Serial.println(" %"); 
    Serial.print("NH4 PPM: ");
    Serial.print(NH4);
    Serial.println(" ppm");
    
    // send the sensor data to virtual pin V0
    Blynk.virtualWrite(V4, waterlevel);
    Blynk.virtualWrite(V5, humidity);
    Blynk.virtualWrite(V6, temperature);
    Blynk.virtualWrite(V7, feedlevel);
    Blynk.virtualWrite(V8, NH4);
  }
  else{   
    Serial.println("Wait...");
  }
  delay(6000);
}
