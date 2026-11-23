#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define DEMO_MODE 0

SemaphoreHandle_t mutexA;
SemaphoreHandle_t mutexB;

struct LockPlan {
  const char*       taskName;
  SemaphoreHandle_t first;
  const char*       firstName;
  SemaphoreHandle_t second;
  const char*       secondName;
  volatile uint32_t rounds;
};

LockPlan plan1;
LockPlan plan2;

void initPlan(LockPlan &p, const char* name,
              SemaphoreHandle_t f, const char* fName,
              SemaphoreHandle_t s, const char* sName) {
  p.taskName = name;
  p.first = f;   p.firstName = fName;
  p.second = s;  p.secondName = sName;
  p.rounds = 0;
}

void workerTask(void* pv) {
  LockPlan* p = (LockPlan*)pv;
  for (;;) {
    Serial.printf("[%s] taking %s ...\n", p->taskName, p->firstName);
    xSemaphoreTake(p->first, portMAX_DELAY);
    Serial.printf("[%s] has %s, now needs %s\n", p->taskName, p->firstName, p->secondName);

    vTaskDelay(pdMS_TO_TICKS(100));   // simulated work: makes the bad timing certain

#if DEMO_MODE == 1
    TickType_t waitTime = pdMS_TO_TICKS(500);   // finite timeout
#else
    TickType_t waitTime = portMAX_DELAY;        // wait forever
#endif

    if (xSemaphoreTake(p->second, waitTime) == pdTRUE) {
      p->rounds++;
      Serial.printf("[%s] has BOTH locks -> work done (round %lu)\n",
                    p->taskName, (unsigned long)p->rounds);
      vTaskDelay(pdMS_TO_TICKS(50));
      xSemaphoreGive(p->second);      // release in reverse order
      xSemaphoreGive(p->first);
      vTaskDelay(pdMS_TO_TICKS(100));
    } else {
      Serial.printf("[%s] TIMEOUT waiting for %s -> release %s, back off\n",
                    p->taskName, p->secondName, p->firstName);
      xSemaphoreGive(p->first);
      vTaskDelay(pdMS_TO_TICKS(random(100, 400)));   // random backoff
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.printf("\n=== Demo 1: Deadlock | DEMO_MODE %d ===\n", DEMO_MODE);

  mutexA = xSemaphoreCreateMutex();
  mutexB = xSemaphoreCreateMutex();

  initPlan(plan1, "Task_1", mutexA, "A", mutexB, "B");
#if DEMO_MODE == 2
  initPlan(plan2, "Task_2", mutexA, "A", mutexB, "B");   // same order as Task_1
#else
  initPlan(plan2, "Task_2", mutexA, "A", mutexB, "B");
#endif

  xTaskCreatePinnedToCore(workerTask, "Task_1", 8192, &plan1, 2, NULL, 1);
  xTaskCreatePinnedToCore(workerTask, "Task_2", 8192, &plan2, 2, NULL, 1);
}

// loop() is the heartbeat: if the rounds counters stop increasing, nothing progresses.
void loop() {
  delay(1000);
  Serial.printf("[HEARTBEAT] t=%lu s | rounds: Task_1=%lu Task_2=%lu\n",
                (unsigned long)(millis() / 1000),
                (unsigned long)plan1.rounds, (unsigned long)plan2.rounds);
}
