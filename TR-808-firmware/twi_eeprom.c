/*
 * twi_eeprom.c
 *
 * Created: 2016-06-12 12:18:57 PM
 *  Author: jeff
 */ 
// hacked from: http://www.nerdkits.com/forum/thread/1423/

#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "twi_eeprom.h"
#include "twi.h"
#include "sequencer.h"


// prototype local functiosn
typedef struct pattern PATTERN;
//PATTERN PATTERN;
// -----------------------------------------------------------------


// Define eeprom commands according to 24CXXX datasheet
typedef struct {
	uint8_t     high_byte;
	uint8_t     low_byte;
	PATTERN		pattern_data;
} WRITE_PATTERN;

typedef struct {
	uint8_t     high_byte;
	uint8_t     low_byte;
} SET_PATTERN_ADDRESS;


// Create structure pointers for the TWI/I2C buffer
WRITE_PATTERN            *p_write_pattern;
SET_PATTERN_ADDRESS      *p_set_pattern_address;
PATTERN             *p_read_pattern;

// Create TWI/I2C buffer, size to largest command
char    TWI_buffer[sizeof(WRITE_PATTERN)];

void eeprom_init(){
	// Specify startup parameters for the TWI/I2C driver
	TWI_init(   F_CPU,           // clock frequency
	800000L,                    // desired TWI/IC2 bitrate
	TWI_buffer,                 // pointer to comm buffer
	sizeof(TWI_buffer),         // size of comm buffer
	&handle_TWI_result          // pointer to callback function
	);
	
	// Enable interrupts
	//sei();
	
	// Set our structure pointers to the TWI/I2C buffer
	p_write_pattern = (WRITE_PATTERN *)TWI_buffer;
	p_set_pattern_address = (SET_PATTERN_ADDRESS *)TWI_buffer;
	p_read_pattern = (PATTERN *)TWI_buffer;
	
}

PATTERN read_pattern(uint16_t memory_address){
	// send 'set memory address' command to eeprom and then read data
	while(TWI_busy);
	p_set_pattern_address->high_byte = (memory_address >> 8);
	p_set_pattern_address->low_byte = memory_address;
	TWI_master_start_write_then_read(   EEPROM_DEVICE_ID,               // device address of eeprom chip
	sizeof(SET_PATTERN_ADDRESS),     // number of bytes to write
	sizeof(PATTERN)             // number of bytes to read
	);
	
	// nothing else to do - wait for the data
	while(TWI_busy);
	// return the data

	return(*p_read_pattern);
}


void write_pattern(uint16_t memory_address, PATTERN *w_data){
	while(TWI_busy);
	int num_pages = sizeof(PATTERN) / PAGE_SIZE;
	for (int i = 0; i < num_pages; ++i) {
		// we can only write up to 32 bytes at a time, so to simplify the code, we simply write 1 byte at a time (plus the 2 address bytes at each write)
		p_write_pattern->high_byte = (memory_address >> 8);
		p_write_pattern->low_byte = memory_address;
		// this is directly putting 1 byte of the input PATTERN w_data into the TWI_buffer *after* the address bytes (hence the +2)
		// NOTE: p_write_pattern is a pointer to TWI_BUFFER
		memcpy(TWI_buffer+2, (char *)w_data + i*PAGE_SIZE, PAGE_SIZE);
		TWI_master_start_write(     EEPROM_DEVICE_ID,       // device address of eeprom chip
		2 + PAGE_SIZE
		);
		memory_address+= PAGE_SIZE;
		while(TWI_busy);
	}
	
	int remaining = sizeof(PATTERN) % PAGE_SIZE;
	p_write_pattern->high_byte = (memory_address >> 8);
	p_write_pattern->low_byte = memory_address;
	memcpy(TWI_buffer+2, (char *)w_data + num_pages*PAGE_SIZE, remaining);
	TWI_master_start_write(     EEPROM_DEVICE_ID,       // device address of eeprom chip
	2 + remaining
	);
	
	while(TWI_busy);
}


// optional callback function for TWI/I2C driver
void handle_TWI_result(uint8_t return_code){
	flag.twi_init_error = 0;
	if(return_code!=TWI_success){
		flag.twi_init_error = 1;
		//turn_on(IF_VAR_B_LED);
	}
}

