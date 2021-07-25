#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>

void log(char *fmt, ... );
void sendReadingToKeyboard( );
void displayTask( void * parameter );
void batteryTask(void * parameter);
void buttonTask(void * parameter) ;
void drawBatteryBar(float level, int rawvalue);
void drawKeyboardConnectionState();
void drawBrightnessBar();
void drawCaliperValue( int32_t value);
void tft_init();
void button_init();
void setup();
void setupKeyboard();

#endif