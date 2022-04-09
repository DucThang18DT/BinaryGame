#pragma once

#include <Arduino.h>
#include <Port.h>

static uint8_t Num_Anode[] = {0x88, 0xBD, 0xC4, 0xA4, 0xB1, 0xA2, 0x82, 0xBC, 0x80, 0xA0, 0xF0, 0xCA};

static void showTime(int number);

/*-----------------------------------*/

void showTime(int number){
    digitalWrite(LED_1_SOURCE, HIGH);
    digitalWrite(LED_2_SOURCE, LOW);
    shiftOut(LED_DS_PIN, LED_SH_PIN, MSBFIRST, Num_Anode[number/10]);
    delay(2);
    digitalWrite(LED_ST_PIN, HIGH);
    digitalWrite(LED_ST_PIN, LOW);
    delay(10);
    digitalWrite(LED_1_SOURCE, LOW);
    digitalWrite(LED_2_SOURCE, HIGH);
    shiftOut(LED_DS_PIN, LED_SH_PIN, MSBFIRST, Num_Anode[number%10]);
    delay(2);
    digitalWrite(LED_ST_PIN, HIGH);
    digitalWrite(LED_ST_PIN, LOW);
    delay(10);
}