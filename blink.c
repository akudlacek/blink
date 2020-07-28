/*
 * blink.c
 *
 * Created: 11/30/2018 4:53:30 PM
 *  Author: akudlacek
 */ 


#include "blink.h"


/**************************************************************************************************
*                                            FUNCTIONS
*************************************************^************************************************/
/******************************************************************************
*  \brief Blink get config defaults
*
*  \note
******************************************************************************/
void blink_get_confg_defaults(blink_conf_t * const conf)
{
	conf->tick_ptr             = 0;
	conf->ticks_per_time_unit  = 0;
	conf->blink_separation     = 0;
	conf->idle_value           = 0;
}

/******************************************************************************
*  \brief Blink init
*
*  \note
******************************************************************************/
void blink_init(blink_inst_t * const inst, blink_conf_t const conf)
{
	inst->conf          = conf;
	
	inst->state         = GET_NEXT_SEQ;
	
	inst->blink_bit     = 0;
	inst->bit_number    = 0;
	
	inst->out           = 0;
	inst->count         = 0;
	
	inst->last_tick     = *inst->conf.tick_ptr;
}

/******************************************************************************
*  \brief Blink task
*
*  \note return value is blink output
******************************************************************************/
uint8_t blink_task(blink_inst_t * const inst)
{
	//if no blink bit set and done blinking go to idle value
	if((inst->blink_bit == 0) && (inst->state == GET_NEXT_SEQ))
	{
		inst->out = 0;
		return (inst->conf.idle_value & 1u);
	}
	//run state machine
	else
	{
		switch(inst->state)
		{
			case GET_NEXT_SEQ:
			//if no blink bit set in this bit position, increment
			if(((inst->blink_bit >> inst->bit_number) & 1u) == 0)
			{
				inst->bit_number++;
				
				//wrap
				if(inst->bit_number > BLINKS_MAX)
				{
					inst->bit_number = 0;
				}
				
				inst->out = 0;
				
				/*stay in GET_NEXT_SEQ state*/
			}
			//live bit found
			else
			{
				inst->count     = 0;                      //reset count
				inst->out       = 1;                      //start first blink
				inst->last_tick = *inst->conf.tick_ptr;   //reset tick time
				inst->state     = BLINKING;               //go to blink state
			}
			break;
			
			case BLINKING:
			if((*inst->conf.tick_ptr - inst->last_tick) >= (inst->conf.ticks_per_time_unit))
			{
				inst->count += inst->out; //adds to count if out is 1
				inst->out   ^= 1;         //toggles output
				
				//if hit number of blinks
				if(inst->count > inst->bit_number)
				{
					inst->bit_number++; //Advance to the next sequence, so it does not get stuck running same bit
					
					//wrap
					if(inst->bit_number > BLINKS_MAX)
					{
						inst->bit_number = 0;
					}
					
					//if no separation go to GET_NEXT_SEQ state
					if(inst->conf.blink_separation == 0)
					{
						inst->state = GET_NEXT_SEQ;
					}
					else
					{
						/*Separation between sequences is 1 or larger*/
						inst->count = 0;
						inst->out   = 0;
						inst->state = SEQ_SEPARATION;
					}
				}
				
				inst->last_tick = *inst->conf.tick_ptr;
			}
			break;
				
			case SEQ_SEPARATION:
			if((*inst->conf.tick_ptr - inst->last_tick) >= (inst->conf.ticks_per_time_unit))
			{
				inst->count++;
				
				if(inst->count >= inst->conf.blink_separation)
				{
					inst->state = GET_NEXT_SEQ;
				}
				
				inst->last_tick = *inst->conf.tick_ptr;
			}
			break;
			
			default:
			//something bad happened
			inst->state         = GET_NEXT_SEQ;
			inst->blink_bit     = 0;
			inst->bit_number    = 0;
			inst->out           = 0;
			inst->count         = 0;
			inst->last_tick     = *inst->conf.tick_ptr;
			return (inst->conf.idle_value & 1u);
			break;
		} /* switch */
	}
	
	return ((inst->out ^ inst->conf.idle_value) & 1u);
}

/******************************************************************************
*  \brief Blink set
*
*  \note sets num of blinks by bit position num_blinks = 0 turns all off
******************************************************************************/
void blink_set(blink_inst_t * const inst, uint8_t num_blinks, uint8_t const on_off)
{
	if(num_blinks == 0)
	{
		inst->blink_bit = 0;
		
		return;
	}
	
	num_blinks--;
	
	if(on_off == 1)
	{
		inst->blink_bit |= (1 << num_blinks);
	}
	else if(on_off == 0)
	{
		inst->blink_bit &= ~(1 << num_blinks);
	}
}
