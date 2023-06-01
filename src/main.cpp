
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#elif defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#include <FirebaseESP8266.h>
#endif

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>


//inisialisasi sensor
#include "DHT.h"
#define DHTPIN 21
#define DHTTYPE DHT22
#define sensor_pin 35
const int trig = 9;
const int echo = 10;

long duration;
int distance;
    

//inisialisasi aktuator
#define pump 32

#define WIFI_SSID "POCO X3 NFC"
#define WIFI_PASSWORD "hime1382"


#define API_KEY "AIzaSyBAB7k2rxsxoANDh4JQS17NvYb5-i4w8KY"


#define DATABASE_URL "https://miot-dhior-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define USER_EMAIL "ridhoalmorits@gmail.com"
#define USER_PASSWORD "miot1382"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;
DHT dht(DHTPIN, DHTTYPE);

void setup()
{

  Serial.begin(9600);
  dht.begin();
  pinMode(pump, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT); 

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);

 
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

}

void loop()
{


  if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
  {
   int x = random(1,100);
   int y = random (100,200);
   float h = dht.readHumidity();
  float t = dht.readTemperature();
  int baca;
  baca = (analogRead(sensor_pin)/40);
  int lembab = 102-(baca);

  int LEDStatus = Firebase.getInt(fbdo,"/monitor/data/GarageLED");
  Serial.println(LEDStatus);


  if (lembab < 30){
    digitalWrite(pump,HIGH);
    Firebase.setString(fbdo,"/monitor/data/status_pompa","ON");
    Firebase.setInt(fbdo,"/monitor/data/Kelembaban",lembab);
    Firebase.setInt(fbdo,"/monitor/data/Suhu",t);
    Firebase.setInt(fbdo,"/monitor/data/Udara",h);
    Serial.println(lembab);
  }else{
    digitalWrite(pump,LOW);
        Firebase.setString(fbdo,"/monitor/data/status_pompa","OFF");
    Firebase.setInt(fbdo,"/monitor/data/Kelembaban",lembab);
    Firebase.setInt(fbdo,"/monitor/data/Suhu",t);
    Firebase.setInt(fbdo,"/monitor/data/Udara",h);
    Serial.println(lembab);

  }
  }
}

int ultrasonic(){
  digitalWrite(trig, LOW); 
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trig, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance= duration*0.034/2;
  return distance;
}