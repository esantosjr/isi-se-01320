/*
 * Main application using FreeRTOS.
 *
 * Author: Elço João dos Santos Junior
 */

/*Libs to generate random includes*/
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 

/* Microkernel includes */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "console.h"
#include "queue.h"

/* Task prototypes */
static void prvADCRead(void *pvParameters);
static void prvSerialInterface(void *pvParameters);
static void prvProcessing(void *pvParameters);
static void prvStats(void *pvParameters);

#define PI 3.141592
#define BUFFER_SIZE 1000

/* Task Priorities */
#define prioADCRead         (tskIDLE_PRIORITY + 4) // High Priority
#define prioProcessing      (tskIDLE_PRIORITY + 3) // Low Priority
#define prioSerialInterface (tskIDLE_PRIORITY + 3) // Low Priority
#define prioStats           (tskIDLE_PRIORITY + 3) // Low Priority

/* Some definitions */
#define pdTICKS_TO_MS( xTicks ) ( ( xTicks * 1000 ) / configTICK_RATE_HZ )

/* Define if tasks are periodic */
#define pTaskADCRead pdMS_TO_TICKS(1);
#define pTaskADCProc pdMS_TO_TICKS(100);
#define pTaskStats   pdMS_TO_TICKS(3000);
#define pTaskSerialInterface pdMS_TO_TICKS(1);

/* Handle for Tasks (optional for Linux) */
TaskHandle_t xHandleADCRead = NULL;
TaskHandle_t xHandleProcessing = NULL;
TaskHandle_t xHandleSerialInterface = NULL;
TaskHandle_t xHandleStats = NULL;

float processed_adc_values[BUFFER_SIZE];
uint32_t proc_current_position = 0;

int main_app()
{
    QueueHandle_t xQueue;

    /* Initializing console */
    console_init();
    
    /* Seed random numbers */
    srand(time(0));

    /* Creating queue */
    console_print("Creating Queues... \n");
    xQueue = xQueueCreate(BUFFER_SIZE, BUFFER_SIZE);
    
    if (xQueue == NULL)
    {
        console_print("Failed on create queue (memory), the program has stopped. Ctrl + C to finish. \n"); 
        for (;;);
    } 
    else
    {
        /* Setting name to Queue */
        vQueueAddToRegistry( xQueue, "Queue-01" );
        console_print("Queue created... \n");
    }
    
    /* Creating tasks */
    xTaskCreate(prvADCRead,                      /* Task Function */
                "ADCRead",                       /* Name of task (for debugging propose only) */
                configMINIMAL_STACK_SIZE * 10,   /* Memory Stack */
                xQueue,                          /* Used to pass a parameter to the task */
                prioADCRead,                     /* Priority of Task */
                &xHandleADCRead);                /* Microcontroller use a Task Handle (optional on Linux) */

    xTaskCreate(prvProcessing,                   /* Task Function */
                "Processing",                    /* Name of task (for debugging propose only) */
                configMINIMAL_STACK_SIZE * 10,   /* Memory Stack */
                xQueue,                          /* Used to pass a parameter to the task */
                prioProcessing,                  /* Priority of Task */
                &xHandleProcessing);             /* Microcontroller use a Task Handle (optional on Linux) */

    xTaskCreate(prvSerialInterface,              /* Task Function */
                "SerialInterface",               /* Name of task (for debugging propose only) */
                configMINIMAL_STACK_SIZE * 10,   /* Memory Stack */
                (void *)1,                       /* Used to pass a parameter to the task */
                prioSerialInterface,             /* Priority of Task */
                &xHandleSerialInterface);        /* Microcontroller use a Task Handle (optional on Linux) */

    xTaskCreate(prvStats,                        /* Task Function */
                "Stats",                         /* Name of task (for debugging propose only) */
                configMINIMAL_STACK_SIZE * 10,   /* Memory Stack */
                (void *)1,                       /* Used to pass a parameter to the task */
                prioStats,                       /* Priority of Task */
                &xHandleStats);                  /* Microcontroller use a Task Handle (optional on Linux) */

    console_print("Starting scheduling, use Ctrl + C on any moment to finish ... \n");

    /* Initializing Scheduler */
    vTaskStartScheduler();

    console_print("Failed to start the scheduler. Ctrl + C to finish. \n");
    for(;;);
}

static void prvADCRead(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pTaskADCRead;
    xLastWakeTime = xTaskGetTickCount();

    /* Queue variables */
    QueueHandle_t xQueue = NULL;

    /* Struct message for queue */
    uint8_t raw_adc_values[BUFFER_SIZE];
    uint32_t raw_current_position = 0;

    /* Restoring xQueue from higher context. */
    xQueue = (QueueHandle_t) pvParameters;

    /* Checking parameter passed to task */
    configASSERT(xQueue != NULL);

    /* Inform tasks information */
    console_print("\n******* %s STATS *******", pcTaskGetName(xHandleADCRead));
    console_print("\n Task Priority: %d", uxTaskPriorityGet(xHandleADCRead));
    console_print("\n Queue Name: %s", pcQueueGetName(xQueue));
    console_print("\n Queue Space Used: %d", uxQueueMessagesWaiting(xQueue));
    console_print("\n Queue Space Avaliable: %d", uxQueueSpacesAvailable(xQueue));
    console_print("\n******************************\n");

    for (;;)
    {   
        /* Gets time since last execution */
        xLastWakeTime = xTaskGetTickCount();

        /* Executes every 1 ms */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        /*
        console_print("\n\n");
        console_print("[ADCRead] running at %lld ms after vTaskStartScheduler() called. \n", pdTICKS_TO_MS(xTaskGetTickCount()));
        */
        
        /* Read ADC value - 10 bits resolution is considered */
        raw_adc_values[raw_current_position] = rand() % 1023;
        
        /*
        console_print("-Sending- ");
        console_print(" Message ID : [%d]", raw_current_position);
        console_print(" ADC value  : [%d]", raw_adc_values[raw_current_position]);
        */

        /* Send the message */
        if(xQueueSend(xQueue, ( void * ) &raw_adc_values, pdMS_TO_TICKS(0)) == pdTRUE && raw_current_position < BUFFER_SIZE)
        {
            /* Increments current vector position thus the next reading does not overwrite information */
            raw_current_position++;
        } else {
            console_print("\n-ADC vector is full-\n");

            /* Suspend current task. No new ADC information is read until users types "zerar" */
            vTaskSuspend( xHandleADCRead );

            /* Deinit current position */
            raw_current_position = 0;
        }
    }
}

static void prvProcessing(void *pvParameters)
{
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pTaskADCProc;

    /* Queue variables */
    QueueHandle_t xQueue = NULL;
    uint8_t received_adc_values[BUFFER_SIZE];
        
    /* Restoring xQueue from higher context. */
    xQueue = (QueueHandle_t) pvParameters;
        
    /* Checking parameter passed to  task */
    configASSERT(xQueue != NULL);

    /* Inform tasks information */
    console_print("\n******* %s STATS *******", pcTaskGetName(xHandleProcessing));
    console_print("\n Task Priority: %d", uxTaskPriorityGet(xHandleProcessing));
    console_print("\n Queue Name: %s", pcQueueGetName(xQueue));
    console_print("\n Queue Space Used: %d", uxQueueMessagesWaiting(xQueue));
    console_print("\n Queue Space Avaliable: %d", uxQueueSpacesAvailable(xQueue));
    console_print("\n******************************\n");

    for (;;)
    {
        /* Gets time since last execution */
        xLastWakeTime = xTaskGetTickCount();

        /* Executes every 100 ms */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        /*
        console_print("\n\n");
        console_print("[Processing] running at %lld ms after vTaskStartScheduler() called. \n", pdTICKS_TO_MS(xTaskGetTickCount()));
        */

        if(xQueueReceive( xQueue,( void * ) &received_adc_values, pdMS_TO_TICKS(0)) == pdTRUE){
            /*
            console_print("-Received- ");
            console_print(" Message ID : [%d]", proc_current_position);
            console_print(" ADC value : [%d]", received_adc_values[proc_current_position]);
            */
            
            processed_adc_values[proc_current_position] = received_adc_values[proc_current_position] * PI;
            /* console_print(" Processed value : [%.2f]", processed_adc_values[proc_current_position]); */
            proc_current_position++;
        } else {
            console_print("-All ADC values were processed- ");
            /* Back to initial position */ 
            proc_current_position = 0;

            /* Suspend current task. No new ADC information is processed until users types "zerar" */
            vTaskSuspend( xHandleProcessing );
        }
    }
}

void prvSerialInterface(void *pvParameters)
{
    char msg[20];
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pTaskSerialInterface;

    console_print("\n******* %s STATS *******", pcTaskGetName(xHandleSerialInterface));
    console_print("\n Task Priority: %d", uxTaskPriorityGet(xHandleSerialInterface));
    console_print("\n******************************\n");

    for (;;) 
    {
        /* Polling serial interface to check if user insert some information */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        /* Read serial interface */
        gets( msg );

        /* Verify if some command from the pre-defined protocol was written */
        if(strcmp(msg,"obter") == 0) 
        {
            /* print current ADC processed values */
            console_print("[ ");

            for(int loop = 0; loop < BUFFER_SIZE; loop++)
              console_print("%.2f ", processed_adc_values[loop]);

            console_print("]\n\n");

            /* Deinit information read from serial interface */
            memset(msg, 0, sizeof(msg));
        }
        else if (strcmp(msg,"zerar") == 0) 
        {
            /* Deinit ADC reading and processing tasks */
            vTaskSuspend( xHandleADCRead );
            vTaskResume( xHandleADCRead );

            vTaskSuspend( xHandleProcessing );
            vTaskResume( xHandleProcessing );
            
            /* Deinit information read from serial interface */
            memset(msg, 0, sizeof(msg));
            /* Deinit processed values vector */
            memset(processed_adc_values, 0, sizeof(processed_adc_values));
            /* Back to initial position */
            proc_current_position = 0;
        }
    }
}

void prvStats(void *pvParameters)
{
    char msg[256];
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pTaskStats;

    console_print("\n******* %s STATS *******", pcTaskGetName(xHandleStats));
    console_print("\n Task Priority: %d", uxTaskPriorityGet(xHandleStats));
    console_print("\n******************************\n\n");
 
    for (;;) 
    {
        /* Print process consumption every 3 seconds */
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        vTaskGetRunTimeStats( ( char * ) msg );
        console_print("\n\nvTaskGetRunTimeStats INFO: \n%s\n", msg);
    }
}
