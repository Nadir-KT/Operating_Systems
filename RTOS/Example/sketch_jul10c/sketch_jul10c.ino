#include <Arduino_FreeRTOS.h>

void setup() {
Serial.begin(9600);
Serial.println(F("In Setup function"));
pinMode(8,OUTPUT);
pinMode(9,OUTPUT);
pinMode(10,OUTPUT);
pinMode(11,OUTPUT);
xTaskCreate(MyTask1, "Task1", 100, NULL, 1, NULL);
xTaskCreate(MyTask2, "Task2", 100, NULL, 2, NULL);
xTaskCreate(MyTask3, "Task3", 100, NULL, 3, NULL);
xTaskCreate(MyIdleTask, "IdleTask", 100, NULL, 0, NULL);
}

void loop() {
}

static void MyTask1(void* pvParameters)
{
  while(1)
  { 
    digitalWrite(8,LOW);
    digitalWrite(9,HIGH); 
    digitalWrite(10,HIGH);
    digitalWrite(11, HIGH); 
    Serial.println(F("Task1"));
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

static void MyTask2(void* pvParameters)
{  
while(1)
  { digitalWrite(8,HIGH);
    digitalWrite(9,LOW); 
    digitalWrite(10,HIGH);
    digitalWrite(11,HIGH);   
    Serial.println(F("Task2"));
    vTaskDelay(110/portTICK_PERIOD_MS);
  }
}

static void MyTask3(void* pvParameters)
{ 
while(1)
  { 
   digitalWrite(8,HIGH);
   digitalWrite(9,HIGH); 
   digitalWrite(10,LOW);
   digitalWrite(11,HIGH);
   Serial.println(F("Task3"));
   vTaskDelay(120/portTICK_PERIOD_MS);
  }
}

static void MyIdleTask(void* pvParameters)
{
  while(1)
   { 
    digitalWrite(8,HIGH);
    digitalWrite(9,HIGH); 
    digitalWrite(10,HIGH);
    digitalWrite(11,LOW);
    Serial.println(F("Idle state"));
    delay(50);
  }  
}