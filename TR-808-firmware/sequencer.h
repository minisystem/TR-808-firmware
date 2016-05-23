/*
 * sequencer.h
 * JR-808 firmware ATMEGA328PB
 * minisystem
 * system79.com
 */



#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "mode.h"
#include "drums.h"

struct pattern { //current pattern is loaded into ram from eeprom. changing pattern will write to eeprom and load next pattern
	
	uint16_t part[32]; 
	uint8_t accent[16]; //this is a memory expensive way to store a single bit of accent data
	uint8_t step_num:4; //1-16 (0-15)
	uint8_t pre_scale:2; //1-4 (0-3)
	
	
};



struct sequencer {
	
	enum global_mode mode;
	uint8_t SHIFT:1; //is SHIFT key being held?
	uint8_t START:1; //is sequencer running or not?
	struct pattern current_pattern;
	uint8_t variation:1; //variation A or variation B
	uint8_t current_step:4; //will need to increase this with sequences >16 steps, or use offset?
	uint8_t next_step_flag:1;
	uint8_t trigger_finished:1;
	uint8_t current_pattern_num:4;
	uint8_t current_measure;
	enum drum current_inst; //this is index of drum_hit struct
	uint16_t step_led_mask[17]; //this may not need to be an array? implemented as array it is a useful shortcut though
	
	};
		

extern struct sequencer sequencer;


#endif 