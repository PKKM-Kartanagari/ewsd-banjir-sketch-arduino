// BLynks
#define BLYNK_TEMPLATE_ID "TMPL6CM0uVQ6h"
#define BLYNK_TEMPLATE_NAME "EWSDBanjir"
#define BLYNK_AUTH_TOKEN "iFpr95TA4HL9JHac8CPhTkrB3VFuHKJP"

// Package
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Initiate Port
#define ULTRA_TRIG 2
#define ULTRA_ECHO 4
#define FLOWSENSOR 14
// #define BUZZER 4

// Initiate Static Variable
#define THRESHOLD1 650
#define THRESHOLD2 700
#define THRESHOLD3 800
#define KEDALAMAN_SUNGAI 15
#define LUAS_PENAMPANG_SUNGAI 34

// Varaible for Flowmeter
long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
boolean ledState = LOW;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

// Variable for WiFi
const char* ssid     = "Pondok Sebelah"; // Deklarasi variabel ssid dan password
const char* password = "*N68J./4yszkZ(Zh";     // Sebelum pemanggilan Blynk.begin()

void setup() {
  Serial.begin(115200);

  pinMode(ULTRA_TRIG, OUTPUT);
  pinMode(ULTRA_ECHO, INPUT);

  setupFlowmeter();

  // Mulai koneksi WiFi
    Serial.printf("Connecting to %s\n", ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
}

void loop() {
  Blynk.run();
  ultrasonic();
  flowmeter();
  delay(1000);
}

void ultrasonic()
{
  digitalWrite(ULTRA_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_TRIG, LOW);

  float jarak = pulseIn(ULTRA_ECHO, HIGH);
  float jarak_value = jarak * 0.017 / 10;
  float ketinggian_air = KEDALAMAN_SUNGAI - jarak_value;

  Serial.print("Jarak Air: ");
  Serial.print(jarak_value);
  Serial.println(" m");

  Serial.print("Ketinggian Air: ");
  Serial.print(ketinggian_air);
  Serial.println(" m");

  Serial.println("=================================");

  Blynk.virtualWrite(V0, ketinggian_air);
}

void IRAM_ATTR pulseCounter() // Interrupt function
{
  pulseCount++;
}

void setupFlowmeter()
{
  pinMode(FLOWSENSOR, INPUT_PULLUP);

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  attachInterrupt(digitalPinToInterrupt(FLOWSENSOR), pulseCounter, FALLING);
}

void flowmeter()
{
  currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;

    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;

    float kecepatan_arus = flowRate * 10000 / 60;

    float debit_air = LUAS_PENAMPANG_SUNGAI * kecepatan_arus / 60;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(kecepatan_arus);  // Print the integer part of the variable
    Serial.println(" m/min");

    Serial.print("Debit Air: ");
    Serial.print(debit_air);  // Print the integer part of the variable
    Serial.println(" L/sec");

    Serial.println("=================================");

    Blynk.virtualWrite(V1, kecepatan_arus);
    Blynk.virtualWrite(V2, debit_air);
  }
}

// void buzz(int y){
//   tone(BUZZER, y);
//   delay(100);
//   noTone(BUZZER);
//   delay(10);
// }
