#pragma once
#include <Arduino.h>

#define dataPin 26
#define clockPin 27
#define droPowerPin 12

// ADC threshold for 1.5V SPCsignals (at 6dB/11-bit, high comes to around 1570 in analogRead() )
#define ADC_TRESHOLD 1000

// timeout in µs for a bit read 
#define BIT_READ_TIMEOUT 600

// timeout for a packet read (ms)
#define PACKET_READ_TIMEOUT 10

// Packet format: [0 .. 19]=data, 20=sign, [21..22]=unused?, 23=inch
#define PACKET_BITS       24

// minimum reading
#define MIN_RANGE -(1<<20)

// DRO circular buffer entries 
#define DRO_BUFFER_SIZE  10

extern size_t dro_index;

extern int32_t microns;
// DRO circular buffer
extern int32_t dro_buffer[DRO_BUFFER_SIZE];
extern int32_t last_valid_reading;


void readCaliperTask( void * parameter );
