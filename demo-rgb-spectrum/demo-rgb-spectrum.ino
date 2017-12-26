#define redPin = 3;
#define greenPin = 4;
#define bluePin = 5;

void setup() {
  pinMode(buzzerPin, OUTPUT);
  meow();
  pinMode(buzzerPin, INPUT);
  // Start off with the LED off.
  setColourRgb(0,0,0);
}

void loop() {
  unsigned int rgbColour[3];

  // Start off with red.
  rgbColour[0] = 255;
  rgbColour[1] = 0;
  rgbColour[2] = 0;  

  for(int i = 0; i <= 170; i += 1) {
    rgbColour[0] = i;
    setColourRgb(rgbColour[0], rgbColour[1], rgbColour[2]);
    delay(15);
  }
  for(int i = 170; i >= 0; i -= 1) {
    rgbColour[0] = i;
    setColourRgb(rgbColour[0], rgbColour[1], rgbColour[2]);
    delay(15);
  }
  delay(900);
}

void setColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

