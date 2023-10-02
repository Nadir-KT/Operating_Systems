#include <Arduino_FreeRTOS.h>
// Task handles
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;
void setup(){
Serial.begin(9600);
// xTaskCreate(Function to be called, "Name of task", Stack size in words, Arguments(pvParameters to pass), Task priority, Task handle); https://www.freertos.org/a00125.html
// Low priority numbers denote low priority tasks. The idle task has priority zero https://www.freertos.org/RTOS-task-priority.html
xTaskCreate(MyTask1, "Task1", 80, NULL, 1, &task_1);
xTaskCreate(MyTask2, "Task2", 80, NULL, 2, &task_2);
xTaskCreate(MyTask3, "Task3", 80, NULL, 3, NULL);
xTaskCreate(StackSize, "Unused Stack Value", 80, NULL, 4, NULL);
xTaskCreate(MyIdleTask, "IdleTask", 80, NULL, 0, NULL);
// Tasks can be created before or after starting the RTOS scheduler
vTaskStartScheduler(); // Start the real time scheduler. https://www.freertos.org/a00132.html
}

void loop(){
// There is no instruction in the loop section of the code.
// Because each task executes on interrupt after specified time
  // Suspend the higher priority task for some intervals
  for (int i = 0; i < 3; i++) {
    vTaskSuspend(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    vTaskResume(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  // Delete the lower priority task
  if (task_1 != NULL) {
    vTaskDelete(task_1);
    task_1 = NULL;
  }
}

/* Task1 with priority 1 */
static void MyTask1(void* pvParameters){
  while(1){
    Serial.println("Task1");
    vTaskDelay(100/portTICK_PERIOD_MS); //This function sends that particular task into the blocked state for a set amount of Ticks. 
    Serial.println("Delayed Task");
    vTaskDelay(110); //https://www.freertos.org/a00127.html
  }
}

/* Task2 with priority 2 */
static void MyTask2(void* pvParameters){  
while(1){
    Serial.println("Task2");
    vTaskDelay(110/portTICK_PERIOD_MS);
  }
}

/* Task3 with priority 3 */
static void MyTask3(void* pvParameters){  
while(1){
    Serial.println("Task3");
    vTaskDelay(120/portTICK_PERIOD_MS);
  }
}

/* Idle Task with priority Zero */ 
static void MyIdleTask(void* pvParameters){
  while(1){
    Serial.println("Idle state");
    delay(50);
  }
}

void StackSize( void * pvParameters ){ // https://www.freertos.org/uxTaskGetStackHighWaterMark.html
  UBaseType_t uxHighWaterMark;
  /* Inspect our own high water mark on entering the task. */
  uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
  for( ;; ){
    /* Call any function. */
    vTaskDelay(100);
    /* Calling the function will have used some stack space, we would 
    therefore now expect uxTaskGetStackHighWaterMark() to return a 
    value lower than when it was called on entering the task. */
    uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    Serial.print("Unused Stack Value = ");
    Serial.println(uxHighWaterMark);
  }
}