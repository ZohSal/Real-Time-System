#define RGB_BUILTIN 38

int brightness = 45;

void taskA(void *parameter) {
  while (1) {
    Serial.println("Task A alive");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void taskB(void *parameter) {
  while (1) {
    // Blue on
    neopixelWrite(RGB_BUILTIN, 0, 0, brightness);
    vTaskDelay(pdMS_TO_TICKS(500));

    // Off
    neopixelWrite(RGB_BUILTIN, 0, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  xTaskCreatePinnedToCore(taskA, "Task A", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskB, "Task B", 2048, NULL, 1, NULL, 1);
}

void loop() {
}