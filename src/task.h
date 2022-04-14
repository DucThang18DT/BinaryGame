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

static QueueHandle_t xQueue;
// static int xReceivedValue;
static bool isCounting;
static bool isWinner;
static int remainingTime;
static int randomSequence[QUEUE_SIZE];
static TaskHandle_t countDown;
static TaskHandle_t readButton1;
static TaskHandle_t readButton2;
static TaskHandle_t receiveFromQueue;
// static TaskHandle_t endGame;

inline static void vTaskInit();
inline static void vPortInit();
static void vGenerateSequence();
static void vShowTime(void *pvParameters);
static void vCountDown(void *pvParameters);
static void vReadButton1(void *pvParameters);
static void vReadButton2(void *pvParameters);
static void vEndGame();
static void vSendToQueue(int xValue);
static void vReceiveFromQueue(void *pvParameters);

/*-----------------------------------------------------------*/

inline void vTaskInit(){
    Serial.begin(9600);
    // Serial.println("Task Init");
    isCounting = true;
    isWinner = false;
    remainingTime = TIME_COUNTDOWN;
    xQueue = xQueueCreate(QUEUE_SIZE, sizeof(int));
    
    vPortInit();
    LCD_init();
    vGenerateSequence();

    if (xQueue != NULL){
        // xTaskCreate(vReadButton1, "readButton1", 80, NULL, 2, &readButton1);
        // xTaskCreate(vReadButton2, "readButton2", 80, NULL, 2, &readButton2);
        xTaskCreate(vShowTime, "ShowTime", 128, NULL, 2, NULL);
        xTaskCreate(vReceiveFromQueue, "Receiver", 128, NULL, 3, &receiveFromQueue);
        xTaskCreate(vCountDown, "CountDown", 80, NULL, 4, &countDown);
        vTaskStartScheduler();
        Serial.println("end schedule");
    }
    Serial.println("end Task Init");
}

inline void vPortInit(){
    pinMode(BUTTON1, INPUT);
    pinMode(BUTTON2, INPUT);
    pinMode(LED_1_SOURCE, OUTPUT);
    pinMode(LED_2_SOURCE, OUTPUT);
    pinMode(LED_DS_PIN, OUTPUT);
    pinMode(LED_ST_PIN, OUTPUT);
    pinMode(LED_SH_PIN, OUTPUT);
    pinMode(LCD_RS_PIN, OUTPUT);
    pinMode(LCD_E_PIN, OUTPUT);
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
    digitalWrite(LCD_RS_PIN, LOW);
    digitalWrite(LCD_E_PIN, LOW);
    digitalWrite(BUZZER, LOW);
}

void vGenerateSequence(){
    Serial.println("vGenerateSequence");
    LCD_clear();
    for (int i = 0; i < QUEUE_SIZE; i++){
        // randomSeed(analogRead(RANDOM_PIN));
        randomSequence[i] = random(0, 120) > 60? 1:0;
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
        // Serial.println("RQ");
        if (!isCounting){
            break;
        }
        xStatus = xQueueReceive(xQueue, &xValue, xTicksToWait);
        if (xStatus){
            LCD_showALetter(xValue + 48);
            Serial.println(xValue);
            if (randomSequence[index] != xValue){
                isCounting = false;
                isWinner = false;
                vEndGame();
            }
            ++index;
            if (index >= QUEUE_SIZE){
                isCounting = false;
                isWinner = true;
                vEndGame();
            }
        }
        vTaskDelay(200/ portTICK_PERIOD_MS);
    }
    Serial.println("Delete Receive from queue");
    vTaskDelete(receiveFromQueue);
}

void vReadButton1(void *pvParameters){
    bool xValue;
    // Serial.println("r1");
    for (;;){
        // Serial.println("R b1");
        if (!isCounting){
            break;
        }
        xValue = (bool)digitalRead(BUTTON1);
        if (xValue){
            vSendToQueue(ZERO);
        }
        vTaskDelay(120/ portTICK_PERIOD_MS);
        // taskYIELD();
    }
    Serial.println("Delete read button 1");
    vTaskDelete(readButton1);
}

void vReadButton2(void *pvParameters){
    bool xValue;
    // Serial.println("r2");
    for (;;){
        // Serial.println("R b2");
        if (!isCounting){
            break;
        }
        xValue = (bool)digitalRead(BUTTON2);
        if (xValue){
            vSendToQueue(ONE);
        }
        vTaskDelay(120/ portTICK_PERIOD_MS);
        // taskYIELD();
    }
    Serial.println("Delete read button 2");
    vTaskDelete(readButton2);
} 

void vCountDown(void *pvParameters){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (isCounting){
        --remainingTime;
        vTaskDelayUntil(&xLastWakeTime, 1000/ portTICK_PERIOD_MS);
    }
    Serial.println("Delete count down");
    vTaskDelete(countDown);
}

void vShowTime(void *pvParameters){
    for (;;){
        // Serial.println("ST");
        showTime(remainingTime);
        if (!remainingTime){
            isCounting = false;
            vEndGame();
        }
        vTaskDelay(100/ portTICK_PERIOD_MS);
    }
}

void vEndGame(){
    // Serial.println("EG");
    for (;;){
        // Serial.println("EG");
        LCD_clear();
        LCD_goto(0,0);
        if (isWinner){
            LCD_showString("You Win", 0);
            winnerSound();
        }
        else{
            LCD_showString("You Loss", 0);
            loserSound();
        }
    }  
    Serial.println("Delete end game");
}