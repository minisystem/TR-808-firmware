/*
 *leds.c
 *JR-808 firmware ATMEGA328PB
 *minisystem
 *system79.com
*/

#include <stdio.h>
#include "hardware.h"
#include "spi.h"
#include "leds.h"

struct led led[NUM_LEDS] = {
	
	{	1<<0	,	1	,	0	,	NO_BLINK},
	{	1<<1	,	1	,	0	,	NO_BLINK},
	{	1<<2	,	1	,	0	,	NO_BLINK},
	{	1<<3	,	1	,	0	,	NO_BLINK},
	{	1<<4	,	1	,	0	,	NO_BLINK},
	{	1<<5	,	1	,	0	,	NO_BLINK},
	{	1<<6	,	1	,	0	,	NO_BLINK},
	{	1<<7	,	1	,	0	,	NO_BLINK},
	{	1<<0	,	0	,	0	,	NO_BLINK},
	{	1<<1	,	0	,	0	,	NO_BLINK},
	{	1<<2	,	0	,	0	,	NO_BLINK},
	{	1<<3	,	0	,	0	,	NO_BLINK},
	{	1<<4	,	0	,	0	,	NO_BLINK},
	{	1<<5	,	0	,	0	,	NO_BLINK},
	{	1<<6	,	0	,	0	,	NO_BLINK},
	{	1<<7	,	0	,	0	,	NO_BLINK},
	{	1<<0	,	2	,	0	,	NO_BLINK},
	{	1<<1	,	2	,	0	,	NO_BLINK},
	{	1<<2	,	2	,	0	,	NO_BLINK},
	{	1<<3	,	2	,	0	,	NO_BLINK},
	{	1<<4	,	2	,	0	,	NO_BLINK},
	{	1<<5	,	2	,	0	,	NO_BLINK},
	{	1<<6	,	2	,	0	,	NO_BLINK},
	{	1<<7	,	2	,	0	,	NO_BLINK},
	{	1<<4	,	3	,	0	,	NO_BLINK},
	{	1<<5	,	3	,	0	,	NO_BLINK},
	{	1<<6	,	3	,	0	,	NO_BLINK},
	{	1<<7	,	3	,	0	,	NO_BLINK},
	{	1<<0	,	4	,	0	,	NO_BLINK},
	{	1<<1	,	4	,	0	,	NO_BLINK},
	{	1<<2	,	4	,	0	,	NO_BLINK},
	{	1<<3	,	4	,	0	,	NO_BLINK},
	{	1<<4	,	4	,	0	,	NO_BLINK},
	{	1<<5	,	4	,	0	,	NO_BLINK},
	{	1<<6	,	4	,	0	,	NO_BLINK},
	{	1<<7	,	4	,	0	,	NO_BLINK},
	{	1<<0	,	5	,	0	,	NO_BLINK},
	{	1<<1	,	5	,	0	,	NO_BLINK},
	{	1<<2	,	5	,	0	,	NO_BLINK},
	{	1<<3	,	5	,	0	,	NO_BLINK},
	{	1<<4	,	5	,	0	,	NO_BLINK},
	{	1<<5	,	5	,	0	,	NO_BLINK},
	{	1<<6	,	5	,	0	,	NO_BLINK},
	{	1<<7	,	5	,	0	,	NO_BLINK},
	{	1<<4	,	6	,	0	,	NO_BLINK},
	{	1<<5	,	6	,	0	,	NO_BLINK},
	{	1<<6	,	6	,	0	,	NO_BLINK},
	{	1<<7	,	6	,	0	,	NO_BLINK},
	{	1<<0	,	7	,	0	,	NO_BLINK},
	{	1<<1	,	7	,	0	,	NO_BLINK},
	{	1<<2	,	7	,	0	,	NO_BLINK},
	{	1<<3	,	7	,	0	,	NO_BLINK},
	{	1<<4	,	7	,	0	,	NO_BLINK},
	{	1<<6	,	7	,	0	,	NO_BLINK},
	{	1<<7	,	7	,	0	,	NO_BLINK}
	
};

void turn_on(uint8_t led_index) {
	
	spi_data[led[led_index].spi_byte] |= led[led_index].spi_bit;
	led[led_index].state = 1;

}

void turn_off(uint8_t led_index) {
	
	spi_data[led[led_index].spi_byte] &= ~(led[led_index].spi_bit);
	led[led_index].state = 0;
}

void flash_once(uint8_t led_index) { //need to think about how to flash LED for fixed interval - say ~5ms - work into main spi updating interrupt somehow. or maybe just flash asynchronously?
	
	//or what about a flash flag and if LED is ON and flash flag is ON then turn it off?
	
}

void toggle(uint8_t led_index) {

	//led[led_index].state ^= led[led_index].state;
	//
	//spi_data[led[led_index].spi_byte] ^= (-led[led_index].state^spi_data[led[led_index].spi_byte]) & led[led_index].spi_bit;
		
	if (led[led_index].state) {
		
		turn_off(led_index);
		
	} else { 
		
		turn_on(led_index);
	
	}
}
	