#include <task.h>

void setup() {
  vTaskInit();
  // LCD_showALetter('A');
  // showTime(9);
  // for (int i = 0; i < 10; i++){
  //   showTime(i);
  //   delay(500);
  // }
  for (;;) Serial.println("infinity loop...");;
}

void loop() {
  Serial.println("main loop");
  delay(200);
}