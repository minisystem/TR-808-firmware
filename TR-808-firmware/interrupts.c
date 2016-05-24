/*
 * interrupts.c
 * JR-808 firmware ATMEGA328PB
 * minisystem
 * system79.com
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "drums.h"
#include "spi.h"
#include "clock.h"
#include "sequencer.h"



ISR (TIMER0_COMPA_vect) {
	
	TCCR0B = 0; //turn off timer
	TIMSK0 &= ~(1<<OCIE0A); //turn off output compare 
	
	//if (sequencer.mode == MANUAL_PLAY) { 
		//spi_data[drum_hit[current_drum_hit].spi_byte_num] &= ~(drum_hit[current_drum_hit].trig_bit);
	//} else {
		//
		//spi_data[8] = 0;
	//}
	////toggle(drum_hit[current_drum_hit].led_index);
	////toggle(ACCENT_1_LED);
	//update_spi(); //should set flag here and update SPI from main loop. SPI should take about 10 microseconds
	sequencer.trigger_finished = 1;
	
}

ISR (TIMER1_COMPA_vect) { //output compare match for internal clock
	
	
	if (sequencer.START) { 
		
		if (internal_clock.ppqn_counter == internal_clock.divider >> 1) { //50% step width, sort of - this is going to get long and complicated fast - need to set flag and handle in main loop refresh function
			
			spi_data[1] = sequencer.step_led_mask[sequencer.current_inst];
			spi_data[0] = sequencer.step_led_mask[sequencer.current_inst] >> 8;
			spi_data[5] &= ~(led[BASIC_VAR_A_LED].spi_bit | led[BASIC_VAR_B_LED].spi_bit); //this clears basic variation LEDs
			spi_data[5] |= sequencer.var_led_mask; 
			turn_off_all_inst_leds();
			turn_on(drum_hit[sequencer.current_inst].led_index);
		}

	} else {
		spi_data[1] = 0; //need to put this somewhere else - this code is executing way too many times
		spi_data[0] = 0;
		spi_data[5] &= ~(led[BASIC_VAR_A_LED].spi_bit | led[BASIC_VAR_B_LED].spi_bit); //this clears basic variation LEDs
		switch (sequencer.variation_mode) {
					
			case VAR_A: case VAR_AB:
			sequencer.var_led_mask = led[BASIC_VAR_A_LED].spi_bit;
			break;
					
			case VAR_B:
			sequencer.var_led_mask = led[BASIC_VAR_B_LED].spi_bit;
			break;
					
		}
		if (internal_clock.beat_counter <2) { //1/8 note, regardless of scale (based on original 808 behavior) - don't take this as gospel. may need to adjust with different pre-scales
			

			if (sequencer.variation_mode == VAR_AB) sequencer.var_led_mask |= led[BASIC_VAR_B_LED].spi_bit;	
					

			
			turn_on(STEP_1_LED); //eventually need to turn on current pattern LED in pattern mode - other modes will require different behavior to be coded
		}
		spi_data[5] |= sequencer.var_led_mask;
		
	} 
	
	if (++internal_clock.ppqn_counter == internal_clock.divider)
	{
		sequencer.next_step_flag = 1;
		internal_clock.beat_counter++; //overflows every 4 beats
		internal_clock.ppqn_counter = 0;
		if (sequencer.current_step++ == sequencer.step_num) { //end of measure
			
			sequencer.current_step = 0;
			if (sequencer.var_change || sequencer.variation_mode == VAR_AB) {
				sequencer.var_change = 0;	
				sequencer.variation ^= 1<<0;
			}
			//sequencer.current_measure++;
		}
	}
	
}