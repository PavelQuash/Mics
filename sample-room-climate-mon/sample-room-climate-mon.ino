

// DS1302
#include "RTClib.h"

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
DS1302 rtc(2,4,3);

void setup() {
  logSetup();
  log("Start");
  
  tempMonHumSetup();
  rtc.begin();
}

void loop() {
  rtc.adjust(DateTime(__DATE__, __TIME__));
  DateTime now = rtc.now();
  showDate(now);  
  delay(2000);
}

void tempMonHumSetup()
{
  dht.begin();
}

void tempMonHumCycle()
{
  delay(2000); // Задержка 2 секунды между измерениями
  float h = dht.readHumidity(); //Считываем влажность
  float t = dht.readTemperature(); // Считываем температуру
  if (isnan(h) || isnan(t)) { // Проверка удачно прошло ли считывание
    Serial.println("Can't read sensor data");
    return;
  }
  Serial.println("Hum: " + String(h) + " %\t" + "Temp: " + String(t) + " *C ");
}

void logSetup()
{
  Serial.begin(9600);
}

void log(String message)
{
  Serial.println(message);
}

void showDate(const DateTime& dt) {
    Serial.print(dt.year(), DEC);
    Serial.print('/');
    Serial.print(dt.month(), DEC);
    Serial.print('/');
    Serial.print(dt.day(), DEC);
    Serial.print(' ');
    Serial.print(dt.hour(), DEC);
    Serial.print(':');
    Serial.print(dt.minute(), DEC);
    Serial.print(':');
    Serial.print(dt.second(), DEC);
    
    Serial.print(" = ");
    Serial.print(dt.unixtime());
    Serial.println();
}

