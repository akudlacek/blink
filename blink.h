/*
 * blink.h
 *
 * Created: 11/30/2018 4:53:40 PM
 *  Author: akudlacek
 */ 


#ifndef BLINK_H_
#define BLINK_H_


#include <stdint.h>


/**************************************************************************************************
*                                             DEFINES
*************************************************^************************************************/
typedef enum blink_state_t
{
	GET_NEXT_SEQ,
	BLINKING,
	SEQ_SEPARATION
} blink_state_t;

typedef struct blink_conf_t
{
	const volatile uint32_t *tick_ms_ptr;
	uint32_t ticks_per_time_unit;  //One blink is on for time unit and off for another
	uint32_t blink_separation;     //Blink separation is 1 time unit long and provides some dead time between blinks
} blink_conf_t;

typedef struct blink_inst_t
{
	blink_conf_t conf;
	blink_state_t state;
	
	uint32_t blink_bit;
	uint8_t num_of_blinks;
	
	uint8_t out;
	uint32_t count;
	
	uint32_t last_tick_ms;
} blink_inst_t;


/**************************************************************************************************
*                                            PROTOTYPES
*************************************************^************************************************/
void blink_get_confg_defaults(blink_conf_t * const conf);
void blink_init(blink_inst_t * const inst, blink_conf_t const conf);
uint8_t blink_task(blink_inst_t * const inst);
void blink_set(blink_inst_t * const inst, uint8_t const num_blinks, uint8_t const on_off);


#endif /* BLINK_H_ */
