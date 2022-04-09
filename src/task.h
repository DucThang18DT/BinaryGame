#pragma once

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <Port.h>
#include <LCD/LCD16x2.h>
#include <buzzer/Buzzer.h>
#include <led7seg/Led7Seg.h>

#define QUEUE_SIZE  10
#define ONE         1
#define ZERO        0
#define TIME_COUNTDOWN  10

QueueHandle_t xQueue;
// static int xReceivedValue;
static bool isCounting;
static bool isWinner;
static int randomSequence[QUEUE_SIZE];
TaskHandle_t countDown;
TaskHandle_t readButton1;
TaskHandle_t readButton2;
TaskHandle_t receiveFromQueue;
TaskHandle_t endGame;

inline static void vTaskInit();
static void vGenerateSequence();
static void vCountDown(void *pvParameters);
static void vReadButton1(void *pvParameters);
static void vReadButton2(void *pvParameters);
static void vEndGame(void *pvParameters);
static void vSendToQueue(int xValue);
static void vReceiveFromQueue(void *pvParameters);

/*-----------------------------------------------------------*/

inline void vTaskInit(){
    Serial.begin(9600);
    Serial.println("Task Init");
    isCounting = true;
    isWinner = false;
    xQueue = xQueueCreate(QUEUE_SIZE, sizeof(int));
    
    pinMode(BUTTON1, INPUT);
    pinMode(BUTTON2, INPUT);
    pinMode(LED_1_SOURCE, OUTPUT);
    pinMode(LED_2_SOURCE, OUTPUT);
    pinMode(LED_DS_PIN, OUTPUT);
    pinMode(LED_ST_PIN, OUTPUT);
    pinMode(LED_SH_PIN, OUTPUT);
    pinMode(LCD_RS_PIN, OUTPUT);
    pinMode(HC_ST_PIN, OUTPUT);
    pinMode(HC_SH_PIN, OUTPUT);
    pinMode(HC_DS_PIN, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    digitalWrite(LED_1_SOURCE, LOW);
    digitalWrite(LED_2_SOURCE, LOW);
    digitalWrite(HC_DS_PIN, LOW);
    digitalWrite(HC_ST_PIN, LOW);
    digitalWrite(HC_SH_PIN, LOW);
    digitalWrite(LED_DS_PIN, LOW);
    digitalWrite(LED_SH_PIN, LOW);
    digitalWrite(LED_ST_PIN, LOW);
    digitalWrite(BUZZER, LOW);

    LCD_init();
    vGenerateSequence();

    if (xQueue != NULL){
        // xTaskCreate(vReadButton1, "readButton1", 200, NULL, 2, &readButton1 );
        // Serial.println("readbutton1 OK");
        // xTaskCreate(vReadButton2, "readButton2", 200, NULL, 2, &readButton2);
        // Serial.println("readbutton2 OK");
        // xTaskCreate(vReceiveFromQueue, "Receiver", 100, NULL, 3, &receiveFromQueue);
        // Serial.println("receive from queue OK");
        // xTaskCreate(vCountDown, "CountDown", 100, NULL, 2, &countDown);
        // Serial.println("count down OK");
        // xTaskCreate(vEndGame, "EndGame", 200, NULL, 1, &endGame);
        // Serial.println("endgame OK");
        // // vTaskStartScheduler();
        // Serial.println("end schedule");
    }
    Serial.println("end Task Init");
}

void vGenerateSequence(){
    Serial.println("vGenerateSequence");
    LCD_clear();
    for (int i = 0; i < QUEUE_SIZE; i++){
        randomSequence[i] = random(0,1);
        LCD_goto(0, i);
        LCD_showALetter(randomSequence[i] + 48);
    }
    LCD_goto(1,0); // begin of line 2
    Serial.println("end vGenerateSequence");
}

void vSendToQueue(int xValue){
    bool xStatus;
    Serial.println("Send to queue");
    xStatus = xQueueSendToBack(xQueue, &xValue, 0);
    if (!xStatus){
        Serial.println("Could not send to the Queue!\r\n");
    }
}

void vReceiveFromQueue(void *pvParameters){
    int xValue;
    bool xStatus;
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS;
    int index = 0;
    for(;;){
        Serial.println("Receice from Queue");
        if (!isCounting){
            break;
        }
        xStatus = xQueueReceive(xQueue, &xValue, xTicksToWait);
        if (xStatus){
            LCD_showALetter(xValue + 48);
            Serial.println("Received");
            Serial.println(xValue);
            if (index >= QUEUE_SIZE){
                isCounting = false;
                isWinner = true;
                winnerSound();
            }
            if (randomSequence[index] != xValue){
                isCounting = false;
                isWinner = false;
            }
        }
        vTaskDelay(20);
    }
    vTaskDelete(receiveFromQueue);
}

void vReadButton1(void *pvParameters){
    bool xValue;
    for (;;){
        Serial.println("read button 1");
        if (!isCounting){
            break;
        }
        xValue = (bool)digitalRead(BUTTON1);
        if (xValue){
            vSendToQueue(ZERO);
        }
        vTaskDelay(20);
        // taskYIELD();
    }
    vTaskDelete(readButton1);
}

void vReadButton2(void *pvParameters){
    bool xValue;
    for (;;){
        Serial.println("read button 2");
        if (!isCounting){
            break;
        }
        xValue = (bool)digitalRead(BUTTON2);
        if (xValue){
            vSendToQueue(ONE);
        }
        vTaskDelay(20);
        // taskYIELD();
    }
    vTaskDelete(readButton2);
} 

void vCountDown(void *pvParameters){
    // unsigned long start = millis();
    int time = TIME_COUNTDOWN;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (isCounting){
        Serial.println("Count Down");
        // if (millis() - start >= 60){
        //     start = millis();
        //     --time;
        // }
        --time;
        showTime(time);
        if (!time){
            isCounting = false;
            loserSound();
        }
        vTaskDelayUntil(&xLastWakeTime, 1000/ portTICK_PERIOD_MS);
    }
    vTaskDelete(countDown);
}

void vEndGame(void *pvParameters){
    for (;;){
        Serial.println("End game");
        LCD_clear();
        LCD_goto(0,0);
        if (isWinner){
            LCD_showString("You Win", 0);
        }
        else{
            LCD_showString("You Loss", 0);
        }
    }   
}