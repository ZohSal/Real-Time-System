QueueHandle_t timerQueue;
hw_timer_t *timer = NULL;

volatile uint32_t seqNum = 0;
int taskDelayMs = 500;

void ARDUINO_ISR_ATTR onTimer() {
  seqNum++;
  uint32_t count = seqNum;

  BaseType_t woken = pdFALSE;
  xQueueSendFromISR(timerQueue, &count, &woken);

  if (woken == pdTRUE) {
    portYIELD_FROM_ISR();
  }
}

void consumerTask(void *parameter) {
  uint32_t val = 0;

  while (1) {
    if (xQueueReceive(timerQueue, &val, portMAX_DELAY) == pdTRUE) {
      Serial.println(val);
      vTaskDelay(pdMS_TO_TICKS(taskDelayMs));
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  timerQueue = xQueueCreate(4, sizeof(uint32_t));
  xTaskCreatePinnedToCore(consumerTask, "Consumer", 2048, NULL, 1, NULL, 1);

  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, 100000, true, 0);
}

void loop() {
}