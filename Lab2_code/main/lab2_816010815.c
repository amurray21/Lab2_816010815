/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "semphr.h"
#include "freertos/queue.h"

#define GPIO_OUTPUT_IO_1    2
#define GPIO_OUTPUT_PIN_SEL  1ULL<<GPIO_OUTPUT_IO_1

static const char *TAG = "main";

SemaphoreHandle_t xMutex;
TaskHandle_t LEDON_TaskHandle = NULL;
TaskHandle_t LEDOFF_TaskHandle = NULL;
TaskHandle_t print_status_TaskHandle = NULL;

static void LED_ON(void *arg)
{       
       if(xSemaphoreTake(xMutex, (TickType_t) 0xFFFFFFFF) == 1)
       {
            for(;;)
            {
                gpio_set_level(GPIO_NUM_2 ,1);
                vTaskDelay(pdMS_TO_TICKS(500));    //500 ms delay

                xSemaphoreGive(xMutex);

                vTaskDelay(pdMS_TO_TICKS(1000));    //1s delay
            }
        }

        vTaskDelay(100);
}

static void LED_OFF(void *arg)
{       
        
       if(xSemaphoreTake(xMutex, (TickType_t)0xFFFFFFFF) == 1)
       {
            for(;;)
            {
                gpio_set_level(GPIO_NUM_2 ,0);
                vTaskDelay(pdMS_TO_TICKS(500));    //500 ms delay

                xSemaphoreGive(xMutex);

                vTaskDelay(pdMS_TO_TICKS(1000));    //1s delay
            }            
       }
       
       vTaskDelay(100);
}

static void print_status(void *arg)
{       
        for(;;)
        {
            if(gpio_get_level(GPIO_NUM_2) == 1)
            {
                printf("Status message: LED is ON\n");  
            } 
            else 
            {
                printf("Status message: LED is OFF\n");  
            }

            vTaskDelay(pdMS_TO_TICKS(1000));    //1s delay
        }
}


void app_main()       
{
    
   gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins GPIO1 and GPIO2
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;

    //configure GPIO with the given settings
    gpio_config(&io_conf);

   

    //xTaskCreate(print_status, "print_status", 2048, NULL, 10, &print_status_TaskHandle);

    xTaskCreate(LED_OFF, "LED_OFF", 2048, NULL, 10, &LEDOFF_TaskHandle);

    xTaskCreate(LED_ON, "LED_ON", 2048, NULL, 10, &LEDON_TaskHandle);
  

    xMutex = xSemaphoreCreateMutex();

    vTaskStartScheduler();

    while(1);
}
