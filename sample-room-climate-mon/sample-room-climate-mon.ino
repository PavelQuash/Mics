//#include "EEPROM.h"

// DS1302
#include <DS1302.h> // https://github.com/msparks/arduino-ds1302 (работает только на 3.3В)
// DHT22
#include <Adafruit_Sensor.h>
#include <DHT.h>
// BMP180


#define DHT_PIN 12 // номер пина, к которому подсоединен датчик
#define RTC_RST_PIN 4 // часы реального времени
#define RTC_DATA_PIN 3
#define RTC_CLK_PIN 2
#define CO2_PIN A7 // датчик CO2
// реле
#define RELAY_PIN 11
#define RELAY_TURN_ON LOW
#define RELAY_TUNRN_OFF HIGH

// Инициализация
DHT dht(DHT_PIN, DHT22);
DS1302 rtc(RTC_RST_PIN, RTC_DATA_PIN, RTC_CLK_PIN); // RST (CE, Chip Enable) pin, DATA (I/O, Input/Output) pin, CLK (SCLK, Serial Clock) pin


void setup() {
  dht.begin(); // temp&hum
  pinMode(CO2_PIN, INPUT); // co2
  // relay
  pinMode(RELAY_PIN, OUTPUT); 
  digitalWrite(RELAY_PIN, HIGH);
  
  // log
  logSetup();
  log("Start");
}

void loop() {
  delay(15000); // Задержка между измерениями
  
  // rtc
  String date = getCurrentTime();
  
  // temp&hum
  String tempAndHum;
  float h = dht.readHumidity(); // Влажность
  float t = dht.readTemperature(); // Температура
  if (isnan(h) || isnan(t)) { tempAndHum = "Can't read sensor data"; }
  else { tempAndHum = "Hum: " + String(h) + " %\t" + "Temp: " + String(t) + " *C "; }

  float co2 = analogRead(CO2_PIN); // co2

  // relay
  bool relayStatus = !digitalRead(RELAY_PIN);
  if (!relayStatus and (t < 25)) // if (!relayStatus and (h < 40))
  {
    digitalWrite(RELAY_PIN, RELAY_TURN_ON);
    digitalWrite(13, HIGH);
  }
  else if (relayStatus and (t > 25)) // if (relayStatus and (h > 60))
  {
    digitalWrite(RELAY_PIN, RELAY_TUNRN_OFF);
    digitalWrite(13, LOW);
  } 
  
  Serial.println(date + "\t" + tempAndHum + "\t" + String(co2) + " ppm?" + "\t");
}

String getCurrentTime()
{
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);
  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
  return buf;
}

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}

void logSetup() { Serial.begin(9600); }
void log(String message) { Serial.println(message); }


