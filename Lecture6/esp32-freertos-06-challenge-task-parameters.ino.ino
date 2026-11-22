#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = 38;
static SemaphoreHandle_t mutex;

void blinkLED(void *parameters) {
  xSemaphoreTake(mutex, portMAX_DELAY);
  int num = *(int *)parameters;
  xSemaphoreGive(mutex);

  Serial.print("Received: ");
  Serial.println(num);

  while (1) {
    neopixelWrite(led_pin, 0, 0, 45);
    vTaskDelay(num / portTICK_PERIOD_MS);
    neopixelWrite(led_pin, 0, 0, 0);
    vTaskDelay(num / portTICK_PERIOD_MS);
  }
}

void setup() {
  int delay_arg;

  Serial.begin(115200);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Mutex Challenge---");
  Serial.println("Enter a number for delay (milliseconds)");

  while (Serial.available() <= 0);

  delay_arg = Serial.parseInt();
  Serial.print("Sending: ");
  Serial.println(delay_arg);

  mutex = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(blinkLED,
                          "Blink LED",
                          2048,
                          (void *)&delay_arg,
                          2,
                          NULL,
                          app_cpu);

  xSemaphoreTake(mutex, portMAX_DELAY);
  xSemaphoreGive(mutex);

  Serial.println("Done!");
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}