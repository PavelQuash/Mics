// DS1302
#include "DS1302.h" // Only for 3.3v https://www.youtube.com/watch?v=jG0sbB4tD04

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
DS1302 rtc(4, 3, 2); // RST (CE, Chip Enable) pin, DATA (I/O, Input/Output) pin, CLK (SCLK, Serial Clock) pin


void setup() {
  logSetup();
  tempMonHumSetup();
  rtcSetup();
  log("Start");
}

void loop() {
  //tempMonHumCycle();
  rtcCycle();
}



void tempMonHumSetup() { dht.begin(); }
void rtcSetup() { 
  // Set the clock to run-mode, and disable the write protection
  //rtc.halt(false);
  //rtc.writeProtect(false);
  // The following lines can be commented out to use the values already stored in the DS1302
  //rtc.setDOW(FRIDAY);        // Set Day-of-Week to FRIDAY
  //rtc.setTime(13, 35, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(6, 11, 2017);   // Set the date to August 6th, 2010
}



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
  // Send Day-of-Week
  Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTimeStr());
  
  // Wait one second before repeating :)
  delay (1000);
  delay(2000);
}



void logSetup() { Serial.begin(9600); }
void log(String message) { Serial.println(message); }


