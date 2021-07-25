#ifndef DISPLAYDRAW_H
#define DISPLAYDRAW_H
#include <Arduino.h>

void displayTask( void * parameter );
void drawBatteryBar(float level, uint32_t rawreading);
void drawCaliperValue( int32_t value);
void drawKeyboardConnectionState(bool value);
void drawBrightnessBar(uint8_t level);
#endif