// DS1302
#include "iarduino_RTC.h"
// Info
// https://lesson.iarduino.ru/page/podklyuchenie-rtc-chasy-realnogo-vremeni-ds1302-ds1307-ds3231-k-arduino/
// http://iarduino.ru/file/235.html

// DHT22
#include "Adafruit_Sensor.h"
#include "DHT.h"
// MQ135
// BMP180
//#include "EEPROM.h"

#define DHTPIN 12 // номер пина, к которому подсоединен датчик
#define MQPIN A7 // датчик CO2
#define RELAYPIN 11 // реле

// Инициализация
DHT dht(DHTPIN, DHT22);
iarduino_RTC rtc(RTC_DS1302, 4, 2, 3); // RST (SCLK, Serial Clock) pin, CLK (CE, Chip Enable) pin, DATA (I/O, Input/Output) pin


void setup() {
  logSetup();
  tempMonHumSetup();
  rtcSetup();
  rtc.settime(0, 39, 10, 6, 11, 17, 0); // sec, min, hours, day, month, year, day of week
  log("Start");
}

void loop() {
  //tempMonHumCycle();
  rtcCycle();
}



void tempMonHumSetup() { dht.begin(); }
void rtcSetup() { rtc.begin(); }



void tempMonHumCycle()
{
  delay(2000); // Задержка 2 секунды между измерениями
  float h = dht.readHumidity(); // Влажность
  float t = dht.readTemperature(); // Температура
  if (isnan(h) || isnan(t)) { // Проверка удачно прошло ли считывание
    Serial.println("Can't read sensor data");
    return;
  }
  Serial.println("Hum: " + String(h) + " %\t" + "Temp: " + String(t) + " *C ");
}



void rtcCycle()
{
  Serial.println(rtc.gettime("d-m-Y, H:i:s, D"));
  delay(2000);
}



void logSetup() { Serial.begin(9600); }
void log(String message) { Serial.println(message); }


