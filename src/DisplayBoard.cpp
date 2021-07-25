#include "DisplayBoard.h"

void DisplayBoard::SetCaliperValue(uint32_t caliperValue)
{
    this->caliperValue.value = caliperValue;
    this->caliperValue.dirty = true;
}

void DisplayBoard::SetBatteryLevel(uint8_t batteryLevel)
{
    this->batteryLevel.value = batteryLevel;
    this->batteryLevel.dirty = true;
}

void DisplayBoard::SetBatteryVoltage(float batteryVoltage)
{
    this->batteryVoltage.value = batteryVoltage;
    this->batteryVoltage.dirty = true;
}

void DisplayBoard::SetKeyboardConnectionState(uint8_t keyboardConnectionState)
{
    this->keyboardConnectionState.value = keyboardConnectionState;
    this->keyboardConnectionState.dirty = true;
}

void DisplayBoard::SetDisplayBrightness(uint8_t displayBrightness)
{
    this->displayBrightness.value = displayBrightness;
    this->displayBrightness.dirty = true;
}

DisplayBoard::DisplayBoard() {
    this->SetBatteryLevel(0);
    this->SetBatteryVoltage(0);
    this->SetCaliperValue(0);
    this->SetDisplayBrightness(4);
    this->SetKeyboardConnectionState(false);
}
