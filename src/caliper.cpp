#include "caliper.h"
#include "DisplayBoard.h"

size_t dro_index = 2;

int32_t microns;
// DRO circular buffer
int32_t dro_buffer[DRO_BUFFER_SIZE] = {};
int32_t last_valid_reading = -100;
extern DisplayBoard displayBoard;

int getBit() {
    int data;

    int readTimeout = micros() + BIT_READ_TIMEOUT;
    while (analogRead(clockPin) > ADC_TRESHOLD) {
      if (micros() > readTimeout)
        return -1; 
    }
    
    while (analogRead(clockPin) < ADC_TRESHOLD) {
      if (micros() > readTimeout)
        return -1;
    }
    
    data = (analogRead(dataPin) > ADC_TRESHOLD)?1:0;
    return data;
}

// read one full packet
long getPacket() 
{
    long packet = 0;
    int readTimeout = millis() + PACKET_READ_TIMEOUT;

    int bitIndex = 0;
    while (bitIndex < PACKET_BITS) {
      int bit = getBit();
      if (bit < 0 ) {
        // bit read timeout: reset packet or bail out
        if (millis() > readTimeout) {
          // packet timeout
          return -1;
        }
        
        bitIndex = 0;
        packet = 0;
        continue;
      }

      packet |= (bit & 1)<<bitIndex;
      bitIndex++;
    }
    
    return packet;
}

// convert a packet to signed microns
long getDisplayValue(long packet)
{
  if (packet < 0)
    return MIN_RANGE;
    
  long data = (packet & 0xFFFFF)*( (packet & 0x100000)?-1:1);

  if (packet & 0x800000) {
        // inch value (this comes sub-sampled) 
        data = data/2.0;
  }

  return data;
}

void readCaliperTask( void * parameter )
{
    uint32_t lastReadTime = millis();

    for(;;) { 
      long packet = getPacket();
      
      if (packet < 0) {
        // read timeout, display?
        if (millis() > lastReadTime + PACKET_READ_TIMEOUT) {
          // advance last read to time-out
          lastReadTime = millis();
          //log("* %d: no SPC data", lastReadTime);
        }
      } else {

        // add to local queue
        //log("* %d: microns=%d raw=0x%08X", millis(), getDisplayValue(packet), packet);
        size_t new_dro_index = (dro_index+1) % DRO_BUFFER_SIZE;
        dro_buffer[new_dro_index] = getDisplayValue(packet);
        dro_index = new_dro_index;

        // set last_valid_reading
        // only if last 3 readings where equal
        if( dro_buffer[new_dro_index] == dro_buffer[new_dro_index-1]
            &&  dro_buffer[new_dro_index] == dro_buffer[new_dro_index-2])
        {
          if(last_valid_reading != dro_buffer[new_dro_index]) {
            last_valid_reading = dro_buffer[new_dro_index];
            displayBoard.SetCaliperValue(dro_buffer[new_dro_index]);
          }
        }
      }
    }
}