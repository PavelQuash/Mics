//#include "EEPROM.h"

// DS1302
#include <DS1302.h> // https://github.com/msparks/arduino-ds1302 (работает только на 3.3В)
// DHT22
#include <Adafruit_Sensor.h>
#include <DHT.h>
// MQ135
// BMP180


#define DHTPIN 12 // номер пина, к которому подсоединен датчик
#define MQPIN A7 // датчик CO2
#define RELAYPIN 11 // реле

// Инициализация
DHT dht(DHTPIN, DHT22);
DS1302 rtc(4, 3, 2); // RST (CE, Chip Enable) pin, DATA (I/O, Input/Output) pin, CLK (SCLK, Serial Clock) pin


void setup() {
  dht.begin(); // temp&hum
  // co2
  
  // log
  logSetup();
  log("Start");
}

void loop() {
  delay(2000); // Задержка между измерениями
  
  // rtc
  String date = getCurrentTime();
  
  // temp&hum
  String tempAndHum;
  float h = dht.readHumidity(); // Влажность
  float t = dht.readTemperature(); // Температура
  if (isnan(h) || isnan(t)) { tempAndHum = "Can't read sensor data"; }
  else { tempAndHum = "Hum: " + String(h) + " %\t" + "Temp: " + String(t) + " *C "; }

  // co2
  String co2 = "?";
  
  Serial.println(date + "\t" + tempAndHum + "\t" + co2 + " ppm" + "\t");
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


