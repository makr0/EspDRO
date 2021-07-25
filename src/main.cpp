#include <SPI.h>
#include <TFT_eSPI.h>
#include <driver/adc.h>
#include <BleKeyboard.h>
#include "Button2.h"
#include "caliper.h"
#include "DisplayBoard.h"
#include "main.h"

#define ESPDRO_VERSION "0.2"

const uint8_t vbatPin = 34;

#define BUTTON_1 35
#define BUTTON_2 0
Button2 btn1(BUTTON_1);
Button2 btn2(BUTTON_2);
const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;
const int backlight[5] = {16,32,64,128,255};
byte tft_brightness=4;
extern int32_t last_valid_reading;

int stream_mode = 0;

TFT_eSPI tft = TFT_eSPI();

BleKeyboard bleKeyboard("DIY Caliper", "Unlimited Surprise Systems");
DisplayBoard displayBoard;

void log(char *fmt, ... )
{  
  char buf[128];
  va_list args;
  va_start (args, fmt);
  vsnprintf(buf, 128, fmt, args);
  va_end (args);
  Serial.println(buf);
}

void sendReadingToKeyboard( ) {
  // send current reading via keyboard
  char buf[32];
  float value = last_valid_reading/100.0;
  sprintf(buf, "%s%.2f", value > 0 ? "":"/",fabs(value));
  if(bleKeyboard.isConnected()) {
    bleKeyboard.print(buf);
  }
}

void batteryTask(void * parameter) {
  float VBAT;  // battery voltage from ESP32 ADC read
  int32_t VBAT_RAW;
  TickType_t xLastWakeTime;
  const TickType_t taskFrequency = 2000; // run every 2 seconds
 
  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount ();
  /*
  The ADC value is a 12-bit number, so the maximum value is 4095 (counting from 0).
  To convert the ADC integer value to a real voltage you’ll need to divide it by the maximum value of 4095,
  then double it (note above that Adafruit halves the voltage), then multiply that by the reference voltage of the ESP32 which 
  is 3.3V and then vinally, multiply that again by the ADC Reference Voltage of 1100mV.
  */
  for(;;) {
    digitalWrite(14, HIGH);
    delay(50);
    VBAT_RAW = analogRead(vbatPin);
    delay(50);
    digitalWrite(14, LOW);
    VBAT = map(VBAT_RAW, 480,640,330,420)/100.0;
    displayBoard.SetBatteryVoltage(VBAT);
    // Wait for the next cycle.
 	  vTaskDelayUntil( &xLastWakeTime, taskFrequency );
  }
}
void buttonTask(void * parameter) {
    for(;;) {
      btn1.loop();
      btn2.loop();      
      delay(10);
    }
}

void keyboardTask(void * parameter) {
  TickType_t xLastWakeTime;
  const TickType_t taskFrequency = 2000; // run every 2 seconds
 
  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount ();

  uint8_t fakebattery = 0;
  /*
  The ADC value is a 12-bit number, so the maximum value is 4095 (counting from 0).
  To convert the ADC integer value to a real voltage you’ll need to divide it by the maximum value of 4095,
  then double it (note above that Adafruit halves the voltage), then multiply that by the reference voltage of the ESP32 which 
  is 3.3V and then vinally, multiply that again by the ADC Reference Voltage of 1100mV.
  */
  for(;;) {
    bleKeyboard.setBatteryLevel(fakebattery++);
    if(fakebattery == 100) fakebattery = 1;
    if(displayBoard.keyboardConnectionState.value != bleKeyboard.isConnected()) {
        displayBoard.SetKeyboardConnectionState(bleKeyboard.isConnected());
    }
    // Wait for the next cycle.
 	  vTaskDelayUntil( &xLastWakeTime, taskFrequency );
  }
}
void tft_init() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setTextSize(1);

  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, backlight[tft_brightness]);
}

void button_init()
{
  pinMode(BUTTON_2,INPUT_PULLUP);
  pinMode(BUTTON_1,INPUT);
  btn1.setDebounceTime(100);

//  btn1.setPressedHandler([](Button2 & b){
//    tft_brightness++;
//    if(tft_brightness>=5) tft_brightness=0;
//    ledcWrite(pwmLedChannelTFT, backlight[tft_brightness]);
//    displayBoard.SetDisplayBrightness(tft_brightness);
//  });
  btn2.setPressedHandler([](Button2 & b){
    sendReadingToKeyboard();
  });
  btn1.setClickHandler([](Button2 & btn) {
      tft.fillScreen(TFT_BLACK);
      digitalWrite(TFT_BL, !digitalRead(TFT_BL));
      digitalWrite(droPowerPin,LOW); // power off the DRO
      digitalWrite(14, LOW); // power off ADC
  
      Serial.println("sleeping");
      delay(500);
  
      tft.writecommand(TFT_DISPOFF);
      tft.writecommand(TFT_SLPIN);
      //esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);
      esp_sleep_enable_ext0_wakeup(GPIO_NUM_35, 0);
      delay(100);
      esp_deep_sleep_start();
  });
}


void setup()
{

  pinMode(dataPin, INPUT);     
  pinMode(clockPin, INPUT);
  pinMode(vbatPin, INPUT);
  pinMode(14, OUTPUT);
  pinMode(droPowerPin, OUTPUT);
  digitalWrite(droPowerPin,LOW); // power off the DRO

  button_init();
  tft_init();

  dro_buffer[dro_index] = 0;

  analogReadResolution(11);
  
  analogSetAttenuation(ADC_6db);
  adc1_config_width(ADC_WIDTH_BIT_10);

  bleKeyboard.begin();
  digitalWrite(droPowerPin,HIGH); // power on the DRO
  xTaskCreate(readCaliperTask, "readCaliper", 10000, NULL, 3, NULL); 
  xTaskCreate(buttonTask, "buttons", 10000,NULL, 2, NULL);
  xTaskCreate(batteryTask, "battery", 10000,NULL, 1, NULL);
  xTaskCreate(displayTask, "display", 10000,NULL, 1, NULL);
  xTaskCreate(keyboardTask, "keyboard", 5000,NULL, 1, NULL);
}

 
size_t last_dro_index = 0;
void loop()
{
  if(Serial.available() > 0)
  {
      String serial_cmd = Serial.readStringUntil('\n');
      log("Received: cmd='%s'", serial_cmd.c_str());

      if (serial_cmd.startsWith("stop")) {
        stream_mode = 0;
      } else if (serial_cmd.startsWith("start")) {
        stream_mode = 1;
      }
  }

  // async dump the DRO stream over serial
  if (stream_mode && (last_dro_index != dro_index)) {
      log("{\"axis0\":%d}", dro_buffer[last_dro_index]);
      last_dro_index = (last_dro_index+1) % DRO_BUFFER_SIZE;
  }
  delay(100);
}
