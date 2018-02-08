#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SoftwareSerial.h>

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
#define MAX_NOISE_SCORE 10

// Initialize
DHT dht(DHT_PIN, DHT22);
SoftwareSerial mhz19serial(MHZ19_TX_PIN, MHZ19_RX_PIN); // RX, TX Nano
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
  // Display
  lcd.init(); // Initialize the lcd
  lcd.backlight(); // Turn on the backlight 
  lcd.createChar(1, noise_symbol);
  // Sensors
  dht.begin();
  mhz19serial.begin(9600);
  pinMode(NOISE_PIN, INPUT);
  pinMode(SOUND_PIN, OUTPUT);
  // Logging
  logSetup();
  log("Start");
}

void loop() {
  // Reading data from sensors
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  dht22LastRead = millis();
  int co2 = readCO2();
  mhz19LastRead = millis();
  int noise = digitalRead(NOISE_PIN);
  buzzerLastRead = millis();
  
  if (noise == 0) {
    if (noise_score == MAX_NOISE_SCORE) { playMelody(); }
    else { noise_score++; }    
  }
  else if (noise_score > 0) { 
    noise_score--;
  }
  log("Nse " + String(noise)); 

  // Show data
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Tmp ");
  if (!isnan(h)) { lcd.print(String(t) + "C"); }
  lcd.setCursor(0,1);
  lcd.print("Hum ");
  if(!isnan(t)) { lcd.print(String(h) + "%"); }
  lcd.setCursor(0,2);
  lcd.print("CO2 ");
  if (co2 >= 0) { lcd.print(String(co2) + "ppm"); }
  lcd.setCursor(0,3);
  lcd.print("Nse "); 
  for(int i = 0; i < noise_score; i++) { lcd.print("\1"); }

  delay(10000);
}

// MH-Z19 reading co2 value
byte mhz19_command[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79}; // MH-Z19 comand to get data
unsigned char mhz19_response[9];
int readCO2() {
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

