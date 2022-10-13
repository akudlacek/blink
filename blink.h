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
//User set max number of blinks
#ifndef BLINKS_MAX
#define BLINKS_MAX 16
#endif

#if (BLINKS_MAX > 8) && (BLINKS_MAX <= 16)
typedef uint16_t blink_bit_t;
#elif (BLINKS_MAX > 16) && (BLINKS_MAX <= 32)
typedef uint32_t blink_bit_t;
#elif (BLINKS_MAX > 32)
#error "unsupported blink max"
#else
typedef uint8_t blink_bit_t;
#endif

//User set blink tick type
#ifndef BLINK_TICK_TYPE
#define BLINK_TICK_TYPE uint32_t
#endif

typedef BLINK_TICK_TYPE blink_tick_t;

typedef enum blink_state_t
{
	GET_NEXT_SEQ,
	BLINKING,
	SEQ_SEPARATION
} blink_state_t;

typedef struct blink_conf_t
{
	const volatile blink_tick_t * tick_ptr;
	blink_tick_t ticks_per_time_unit;          //One blink is on for time unit and off for another
	uint16_t blink_separation;             //Blink separation is 1 time unit long and provides some dead time between blinks
	uint8_t idle_value;                    //Value of output when there is no blink
} blink_conf_t;

typedef struct blink_inst_t
{
	blink_conf_t conf;
	blink_state_t state;
	
	blink_bit_t blink_bit;
	uint8_t bit_number;
	
	uint8_t out;
	uint8_t count;
	
	blink_tick_t last_tick;
} blink_inst_t;


/**************************************************************************************************
*                                            PROTOTYPES
*************************************************^************************************************/
void    blink_get_confg_defaults(blink_conf_t * const conf);
void    blink_init              (blink_inst_t * const inst, blink_conf_t const conf);
uint8_t blink_task              (blink_inst_t * const inst);
void    blink_set               (blink_inst_t * const inst, uint8_t num_blinks, uint8_t const on_off);
uint8_t blink_idle           (blink_inst_t * const inst);

#endif /* BLINK_H_ */
