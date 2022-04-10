#include <task.h>

void setup() {
  vTaskInit();
  // LCD_showALetter('A');
  // showTime(9);
  for (;;) Serial.println("infinity loop...");;
}

void loop() {}