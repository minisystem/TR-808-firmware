/*
 *main.c
 *JR-808 firmware ATMEGA328PB
 *minisystem
 *system79.com
*/

#include <avr/io.h>
#include <stdio.h>
#define F_CPU 16000000UL

#include <avr/interrupt.h>
#include <util/delay.h>
#include "mode.h"
#include "sequencer.h"
#include "hardware.h"
#include "leds.h"
#include "switches.h"
#include "spi.h"
#include "midi.h"
#include "drums.h"
#include "clock.h"
#include "adc.h"
#include "twi_eeprom.h"
#include "xnormidi-develop/midi.h"
#include "xnormidi-develop/midi_device.h"
#include "xnormidi-develop/bytequeue/bytequeue.h" //this is required for MIDI sending

MidiDevice midi_device;

//pattern_data next_pattern;



void refresh(void) {
	//if (sequencer.SHIFT) update_tempo(); //this analog reading is noisy - need to do it less often, like maybe only when shift is pressed? //meh, doesn't seem to make a huge difference.	
	if (clock.source == INTERNAL) {
		update_tempo(); 	
	}
	check_start_stop_tap();
	read_switches();
	parse_switch_data();
	if (sequencer.mode == MANUAL_PLAY) live_hits(); //live_hits() needs to be updated to work with synchronized spi updating
	update_mode();
	update_fill_mode();
	check_clear_switch();
	check_intro_fill_variation_switch();
	check_variation_switches();
	update_prescale();
	check_inst_switches();
	update_inst_leds();	
	update_step_board();
	process_step();
	update_spi();
	PORTD &= ~(1<<TRIG);
	
}


int main(void)
{
		
    DDRD |= (1<<TRIG); //set PD5, TRIG to output
	
	//setup SPI
	DDRE |= (1<<SPI_MOSI) | (1<<SPI_SS); //set MOSI and SS as outs (SS needs to be set as output or it breaks SPI
	DDRC |= (1<<SPI_CLK) | (1<<SPI_LED_LATCH) | (1<<SPI_SW_LATCH);
	DDRB |= (1<<SPI_EN);
	//DDRB &= ~((1<<TAP) | (1<<START_STOP)); //set start/stop tap pins as inputs
	
	PORTE &= ~(1<<SPI_MOSI );
	PORTC &= ~(1<<SPI_CLK | 1<<SPI_LED_LATCH | 1<<SPI_SW_LATCH);
	PORTB &= ~(1<<SPI_EN); //active low
	
	PORTC |= (1<<SPI_LED_LATCH); //toggle LED LATCH HIGH (disabled)
	
	SPCR1 = (1<<SPE1) | (1<<MSTR1); //Start SPI as MASTER
	SPSR1 |= (1<<SPI2X); //set clock rate to XTAL/2 (8 MHz)
	
	//setup external interrupts
	EICRA |= (1 << ISC11) | (1 << ISC10); //set up DIN sync to trigger on rising edge of DIN clock
	PCMSK2 |= (1 << PCINT20); //set up DIN Run/Stop pin change interrupt
	
	sequencer.current_pattern = sequencer.new_pattern = 0;
	turn_on(sequencer.current_pattern);
	turn_on(MODE_2_FIRST_PART_PART);
	turn_on(FILL_MANUAL);
	
	update_spi();
	
	//setup Timer0 for drum triggering interrupt
	
	TCCR0A |= (1<<WGM01); //clear on compare match A
	OCR0A = 225; //gives period of about 0.9ms
	
	//setup MIDI
	//initialize MIDI device
	midi_device_init(&midi_device);
	
	//register callbacks
	midi_register_noteon_callback(&midi_device, note_on_event);
	midi_register_noteoff_callback(&midi_device, note_off_event);
	midi_register_realtime_callback(&midi_device, real_time_event);
	//midi_register_songposition_callback(&midi_device, song_position_event);
	//setup MIDI USART
	setup_midi_usart();
	
	setup_clock();
	//sequencer.pre_scale = PRE_SCALE_3;
	clock.divider = PRE_SCALE_3;//.pre_scale;; //6 pulses is 1/16th note - this is are default fundamental step
	clock.ppqn_counter = 1;
	clock.source = INTERNAL;
	//clock.rate = 400; //use fixed rate to get clock working
	//update_clock_rate(clock.rate);
	setup_adc();
	trigger_finished = 1;
	flag.pre_scale_change = 0;
	sequencer.START = 0;
	//update_tempo();
	
	//set up default start up state. Eventually this should be recalled from EEPROM
	//sequencer.step_num[FIRST] = 15; //0-15 - default 16 step sequence - will change with pre-scale? and can by dynamically changed while programming pattern
	//sequencer.step_num[SECOND] = NO_STEPS; //default is that second part is not active
	//sequencer.step_num_new = 15;
	sequencer.variation_mode = VAR_A;
	turn_on(BASIC_VAR_A_LED);
	sequencer.intro_fill_var = VAR_A;
	turn_on(IF_VAR_A_LED);
	sequencer.mode = FIRST_PART;
	//sequencer.SLAVE = 0;
	sequencer.sync_mode = MIDI_MASTER;

	sequencer.part_playing = FIRST;
	sequencer.part_editing = FIRST;
	turn_on(FIRST_PART_LED);
	turn_on(SCALE_3_LED);
	sequencer.current_intro_fill = 12;//first of 4 intro/fill patterns
	sequencer.fill_mode = MANUAL;
	eeprom_init();
	//flag.twi_init_error = 0;
	

	
	DDRE |= (1<<PE0); //set PE0, pin 3 as output for diagnostic purposes (currently used for TWI timing measurement)
	//DDRD |= 1 << PD3; //set up PD3 as output 
	sei(); //enable global interrupts	
	//sequencer.pattern[0] = read_pattern(0);
	read_next_pattern(0); //load first pattern
	//update_step_led_mask();
	
	//sequencer.SHUFFLE = 1;
	sequencer.shuffle_amount = 0;
	
    while (1) 
    {
		//PORTD |= (1<< PD3);
		midi_device_process(&midi_device); //this needs to be called 'frequently' in order for MIDI to work
		refresh();
		//PORTD &= ~(1<<PD3);

	}
}

