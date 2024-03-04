/* pvParameters: A value that is passed as the paramater to the created task.
If pvParameters is set to the address of a variable then the variable must still exist when the created task executes - so it is not valid to pass the address of a stack variable. */

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

// Task function
void vTaskFunction(void *pvParameters) {
    int taskParam = *((int*)pvParameters); // Cast pvParameters to int pointer and dereference
    printf("Task parameter: %d\n", taskParam);
    
    // Task loop
    for (;;) {
        // Task code here
    }
}

int main() {
    int parameter = 123; // Define parameter to be passed to the task

    // Create task with xTaskCreate
    xTaskCreate(vTaskFunction,        // Task function
                "Task",               // Task name (for debugging)
                configMINIMAL_STACK_SIZE, // Stack depth
                &parameter,           // Task parameter
                tskIDLE_PRIORITY + 1, // Priority
                NULL);                // Task handle (not used here)

    // Start the scheduler
    vTaskStartScheduler();

    // Code should never reach here
    return 0;
}