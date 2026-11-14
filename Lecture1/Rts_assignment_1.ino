#define RGB_BUILTIN 38

int stepDelay = 300;

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("Starting custom RGB pattern");
}

void loop() {
  // Cyan
  neopixelWrite(RGB_BUILTIN, 0, 30, 30);
  delay(stepDelay);

  // Purple
  neopixelWrite(RGB_BUILTIN, 50, 0, 50);
  delay(600);

  // Yellow-orange
  neopixelWrite(RGB_BUILTIN, 70, 40, 0);
  delay(250);

  // Red flash
  neopixelWrite(RGB_BUILTIN, 60, 0, 0);
  delay(150);

  // Off
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);
  delay(800);
}
