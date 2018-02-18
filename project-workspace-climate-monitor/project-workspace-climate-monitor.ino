// Created by Quash (qshzone.ru) 2018
// 350 — 450 ppm    : Нормальный уровень на открытом воздухе.
// less than 600 ppm : Приемлемые уровни. Уровень. рекомендованный для спален, детских садов и школ.
// 600 — 1000 ppm   : Жалобы на несвежий воздух, возможно снижение концентрации внимания.
// 1000 ppm          : Максимальный уровень стандартов ASHRAE (American Society of Heating, Refrigerating and Air-Conditioning Engineers) и OSHA (Occupational Safety & Health Administration).
// 1000 — 2500 ppm  : Общая вялость, снижение концентрации внимания, возможна головная боль.
// 2500 — 5000 ppm  : Возможны нежелательные эффекты на здоровье.
// dht22 
// mh-z19 +/- 50 ppm

#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP085.h>
#include <Adafruit_BME280.h>

// Roadmap
// 1. Noise read value, and score it
// 2. Different threads without common delay
// 3. Updating screen just values and if value is changed
// 4. Delete ppm>m< symbol
// 5. Timer to turn off lcd backlight by button
// 6. Send data to thingspeak service

// Settings
LiquidCrystal_I2C lcd(0x27,20,4);  // Address and size of display
#define DHT_PIN 4
#define MHZ19_RX_PIN 5
#define MHZ19_TX_PIN 6
#define NOISE_PIN 2
#define SOUND_PIN 3
// Limits
#define PERIOD_DHT22 2000 // Not often than 2 secs
#define PERIOD_MHZ19 3000 // Not often than 3 secs
#define PERIOD_BUZZER 12000 // 2 minutes
#define MAX_NOISE_SCORE 5

// Initialize
DHT dht(DHT_PIN, DHT22);
SoftwareSerial mhz19serial(MHZ19_TX_PIN, MHZ19_RX_PIN); // RX, TX Nano
Adafruit_BMP085 bmp;
Adafruit_BME280 bme;
unsigned long dht22LastRead = 0;
unsigned long mhz19LastRead = 0;
unsigned long buzzerLastRead = 0;
int noise_score = 0;
byte noise_symbol[8] = // :(
{
  B00000,
  B01010,
  B01010,
  B00000,
  B01110,
  B10001,
  B00000,
};     

void setup() {
  // Logging
  logSetup();
  log("Start");
  // Display
  lcd.init(); // Initialize the lcd
  lcd.backlight(); // Turn on the backlight 
  lcd.createChar(1, noise_symbol);
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Tmp ");
  lcd.setCursor(0,1); lcd.print("Hum ");
  lcd.setCursor(0,2); lcd.print("CO2 ");
  lcd.setCursor(0,3); lcd.print("Tmp ");
  lcd.setCursor(10,0); lcd.print("Tmp "); 
  lcd.setCursor(10,1); lcd.print("Hum "); 
  lcd.setCursor(12,2); lcd.print("A "); 
  lcd.setCursor(10,3); lcd.print("Prs "); 
  // Sensors
  dht.begin();
  mhz19serial.begin(9600);
  pinMode(NOISE_PIN, INPUT);
  pinMode(SOUND_PIN, OUTPUT);
  
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  if (!bme.begin()) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      while (1);
  }
}

void loop() {
  // Reading data from sensors
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  dht22LastRead = millis();
  int co2 = readCO2();
  mhz19LastRead = millis();
  int noise = 1;//digitalRead(NOISE_PIN);
  buzzerLastRead = millis();

  log("\tTemp(C)\tHum(%)\tPrs(mm)\tAlt(m)\tCO2(ppm)");
  log("dht\t" + String(t,1) + "\t" + String(h,1) + "\t-\t-\t-");
  log("bme\t" + String(bme.readTemperature()) + "\t" + String(bme.readHumidity()) + "\t" + String(bme.readPressure() * 0.007501) + "\t" + String(bme.readAltitude(1013.25)) + "\t-");
  log("bmp\t" + String(bmp.readTemperature()) + "\t-\t" + String(bmp.readPressure() * 0.007501) + "\t" + String(bmp.readAltitude()) + "\t-");
  log("mhz\t-\t-\t-\t-\t" + String(co2));
  log("");
  
  if (noise == 0) {
    if (noise_score == MAX_NOISE_SCORE) { playMelody(); }
    else { noise_score++; }    
  }
  else if (noise_score > 0) { 
    noise_score--;
  }
  //log("Noise sensor " + String(noise)); 

  // Show data
  lcd.setCursor(4,0); 
  if (!isnan(h)) { lcd.print(String(t,1) + "C"); }
  lcd.setCursor(4,1);
  if(!isnan(t)) { lcd.print(String(h,1) + "%"); }
  lcd.setCursor(4,2);
  if (co2 >= 0) { lcd.print(String(co2) + "ppm "); }
  //lcd.setCursor(15,0);
  //for(int i = 0; i < noise_score; i++) { lcd.print("\1"); }

  lcd.setCursor(14,0); lcd.print(String(bme.readTemperature()) + "C");
  lcd.setCursor(14,1); lcd.print(String(bme.readHumidity()) + "%");
  lcd.setCursor(14,2); lcd.print(String(bme.readAltitude(1013.25)));;// + "m ");
  lcd.setCursor(14,3); lcd.print(String(bme.readPressure() * 0.007501));//+ "mm ");

  delay(10000);
}

// MH-Z19 reading co2 value
int readCO2() {
  byte mhz19_command[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; // MH-Z19 comand to get data
  unsigned char mhz19_response[9];
  
  mhz19serial.write(mhz19_command, 9);
  memset(mhz19_response, 0, 9);
  mhz19serial.readBytes(mhz19_response, 9);

  // CRC calculating
  byte crc = 0;
  for (int i = 1; i < 8; i++) crc += mhz19_response[i];
  crc = 255 - crc;
  crc++;
  if (!(mhz19_response[0] == 0xFF && mhz19_response[1] == 0x86 && mhz19_response[8] == crc)) { return -1; }

  // Result value calculating
  unsigned int responseHigh = (unsigned int)mhz19_response[2];
  unsigned int responseLow = (unsigned int)mhz19_response[3];
  
  lcd.setCursor(4,3); lcd.print(String(mhz19_response[4] - 40) + "C");
  
  return 256 * responseHigh + responseLow;
}

const byte COUNT_NOTES = 39; // Notes count
// Notes frequency
int frequences[COUNT_NOTES] = {
  392, 392, 392, 311, 466, 392, 311, 466, 392,
  587, 587, 587, 622, 466, 369, 311, 466, 392,
  784, 392, 392, 784, 739, 698, 659, 622, 659,
  415, 554, 523, 493, 466, 440, 466,
  311, 369, 311, 466, 392
};
// Notes duration
int durations[COUNT_NOTES] = {
  350, 350, 350, 250, 100, 350, 250, 100, 700,
  350, 350, 350, 250, 100, 350, 250, 100, 700,
  350, 250, 100, 350, 250, 100, 100, 100, 450,
  150, 350, 250, 100, 100, 100, 450,
  150, 350, 250, 100, 750
};
void playMelody() {
  for (int i = 0; i <= COUNT_NOTES; i++  ) {
    tone(SOUND_PIN, frequences[i], durations[i] * 2);
    delay(durations[i] * 2);  // Pause
    noTone(SOUND_PIN); // Stop
  }
}

void logSetup() { Serial.begin(9600); }
void log(String message) { Serial.println(message); }

