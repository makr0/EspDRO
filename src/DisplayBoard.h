#ifndef DISPLAYBOARD_H
#define DISPLAYBOARD_H
#include <Arduino.h>

// struct for different data typesto display and keep track of dirty state
typedef struct
{
  int32_t value;
  bool dirty;
} display_item_int32;
typedef struct
{
  int8_t value;
  bool dirty;
} display_item_int8;

typedef struct
{
  float value;
  bool dirty;
} display_item_float;

class DisplayBoard
{
public:
  uint32_t lastDisplayTime;
  display_item_int32 caliperValue;           // Value to display, assumed to be microns, will be divided by 100 before being displayed 
  display_item_int8 batteryLevel;            // Batterylevel 0-100%
  display_item_float batteryVoltage;         // raw Battery Voltage 3.3 - 4.2V
  display_item_int8 keyboardConnectionState; // 1 or 0, connected/not connected
  display_item_int8 displayBrightness;       // how many bars to display (1-5)

  void SetCaliperValue(uint32_t caliperValue);

  void SetBatteryLevel(uint8_t batteryLevel);

  void SetBatteryVoltage(float batteryVoltage);

  void SetKeyboardConnectionState(uint8_t keyboardConnectionState);

  void SetDisplayBrightness(uint8_t displayBrightness);

  DisplayBoard();
};

#endif // DISPLAYBOARD_H
