#include "DisplayDraw.h"
#include "DisplayBoard.h"
#include <TFT_eSPI.h>

extern DisplayBoard displayBoard;
extern TFT_eSPI tft;

void displayTask( void * parameter ) {
  TickType_t xLastWakeTime;
  const TickType_t taskFrequency = 20; // run every 20 ms, refresh rate of 50Hz
 
  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount ();
  for(;;) {
    // Wait for the next cycle.
 	  vTaskDelayUntil( &xLastWakeTime, taskFrequency );

    if( displayBoard.caliperValue.dirty ) {
      drawCaliperValue(displayBoard.caliperValue.value);
      displayBoard.caliperValue.dirty = false;
    }
    if( displayBoard.batteryVoltage.dirty ) {
      drawBatteryBar(displayBoard.batteryVoltage.value,displayBoard.batteryReading.value);
      displayBoard.batteryVoltage.dirty = false;
    }
    if( displayBoard.displayBrightness.dirty ) {
      drawBrightnessBar(displayBoard.displayBrightness.value);
      displayBoard.displayBrightness.dirty = false;
    }
    if( displayBoard.keyboardConnectionState.dirty ) {
      drawKeyboardConnectionState(displayBoard.keyboardConnectionState.value);
      displayBoard.keyboardConnectionState.dirty = false;
    }
  }
}
void drawBatteryBar(float level, uint32_t rawreading) {
  char buf[10];
  sprintf(buf, "%.2fV", level );
  tft.setTextColor(TFT_SKYBLUE);
  tft.fillRect(tft.width()-80,0,100,30, TFT_BLACK);
  tft.setFreeFont(&FreeSansBold9pt7b);
  tft.setCursor(tft.width()-80, 14);
  tft.print(buf);
  sprintf(buf, "%d", rawreading);
  tft.setCursor(tft.width()-80, 29);
  tft.print(buf);
}

void drawCaliperValue( int32_t value) {
  char buf[16];
  sprintf(buf, "%.2f", value/100.0);
  tft.setTextColor(TFT_LIGHTGREY);
  tft.fillRect(0,40,tft.width()-40,46,TFT_BLACK);
  tft.setFreeFont(&FreeSansBold24pt7b);
  tft.setCursor(10, tft.height()/2+10);
  tft.print(buf);
}
void drawKeyboardConnectionState(bool state) {
  tft.setTextColor(state ? TFT_DARKGREEN: TFT_DARKGREY);
  tft.fillRect(0,0,120,17,TFT_BLACK);
  tft.setFreeFont(&FreeSansBold9pt7b);
  tft.setCursor(2, 15);
  tft.print(state ? "connected":"disconnected");
}

void drawBrightnessBar(byte tft_brightness ) {
    tft.fillRect(2,tft.height()-14,35,14,TFT_DARKGREEN);
    for(int i=0;i<tft_brightness+1;i++) {
      tft.fillRect(2+(i*7),tft.height()-12,3,10,TFT_GREEN);
    }
}
