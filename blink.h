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
typedef uint16_t blink_bit_flg_t;
#elif (BLINKS_MAX > 16) && (BLINKS_MAX <= 32)
typedef uint32_t blink_bit_flg_t;
#elif (BLINKS_MAX > 32)
#error "unsupported blink max"
#else
typedef uint8_t blink_bit_flg_t;
#endif

typedef enum blink_state_t
{
	GET_NEXT_SEQ,
	BLINKING,
	SEQ_SEPARATION
} blink_state_t;

typedef struct blink_conf_t
{
	TICK_TYPE                     on_time_tick; //Blink on time
	TICK_TYPE                    off_time_tick; //Blink off time
	TICK_TYPE                    sep_time_tick; //Blink separation
	TICK_TYPE                  prsst_time_tick; //Minimum amount of time a blink code will persist after all clear
	uint8_t                           polarity; //Inverts output when 1, normal when 0;
} blink_conf_t;

typedef struct blink_inst_t
{
	blink_conf_t                          conf; //Configuration struct
	
	blink_bit_flg_t                    bit_flg; //bit flag indicating the number of blinks in the sequence, 0=off 1=on.
	blink_bit_flg_t               last_bit_flg; //last state of bit_flg
	blink_bit_flg_t              prsst_bit_flg; //persistent bit flag for holding the blink pattern on for a min amount of time.
	
	blink_state_t                        state; //State machine
	uint8_t                            bit_num; //Holds number of bit active
	
	uint8_t                                out; //Holds output value
	uint8_t                              count; //Holds current blink count
	
	TICK_TYPE                  prev_prsst_tick; //Last time bit was enabled, for persistence of blink
	TICK_TYPE                        prev_tick; //Last time the state machine advanced
} blink_inst_t;


/**************************************************************************************************
*                                            PROTOTYPES
*************************************************^************************************************/
void    blink_get_confg_defaults(blink_conf_t * const conf);
void    blink_init              (blink_inst_t * const inst, blink_conf_t const conf);
uint8_t blink_task              (blink_inst_t * const inst);
void    blink_set               (blink_inst_t * const inst, uint8_t num_blinks, uint8_t const on_off);
uint8_t blink_idle              (blink_inst_t * const inst);

#endif /* BLINK_H_ */
