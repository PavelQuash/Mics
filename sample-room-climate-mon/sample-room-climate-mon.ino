

// DS1302
#include "DS1302.h" // https://github.com/msparks/arduino-ds1302

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
DS1302 rtc(2,3,4); // CLK (CE, Chip Enable) pin, DATA (I/O, Input/Output) pin, RST (SCLK, Serial Clock) pin

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
  rtc.writeProtect(false);
  rtc.halt(false);
  //Time t(2017, 11, 6, 10, 20, 00, Time::kSunday); // Make a new time object to set the date and time.
  //rtc.time(t); // Set the time and date on the chip.
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



void rtcCycle()
{
  printTime();
  delay(2000);
}

void printTime() {
  // Get the current time and date from the chip.
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
  Serial.println(buf);
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


