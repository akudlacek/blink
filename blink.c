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
void blink_get_confg_defaults(blink_conf_t *conf)
{
	conf->tick_ms_ptr          = 0;
	conf->ticks_per_time_unit  = 0;
	conf->blink_separation     = 0;
}

/******************************************************************************
*  \brief Blink init
*
*  \note
******************************************************************************/
void blink_init(blink_inst_t *inst, blink_conf_t conf)
{
	inst->conf          = conf;
	
	inst->state         = GET_NEXT_SEQ;
	
	inst->blink_bit     = 0;
	inst->num_of_blinks = 0;
	
	inst->out           = 0;
	inst->count         = 0;
	
	inst->last_tick_ms  = 0;
}

/******************************************************************************
*  \brief Blink task
*
*  \note
******************************************************************************/
uint8_t blink_task(blink_inst_t *inst)
{
	if((*inst->conf.tick_ms_ptr - inst->last_tick_ms) >= (inst->conf.ticks_per_time_unit))
	{
		switch(inst->state)
		{
			case GET_NEXT_SEQ:
			if(inst->blink_bit > 1)
			{
				/*there is a blink sequence to start*/
				
				/*Find next set bit*/
				while((inst->blink_bit & (1 << inst->num_of_blinks)) == 0)
				{
					inst->num_of_blinks++;
					
					if(inst->num_of_blinks >= 32)
					{
						inst->num_of_blinks = 0;
					}
					
					/*Just in case*/
					if(inst->blink_bit <= 1)
					{
						break;
					}
				}
				
				inst->count = 0;
				inst->out = 1;
				inst->state = BLINKING;
			}
			else
			{
				/*stay in GET_NEXT_SEQ state*/
				inst->out = 0;
			}
			break;
			
			case BLINKING:
			inst->count += inst->out; //adds to count if out is 1
			inst->out ^= 1;           //toggles output
			
			if(inst->count >= inst->num_of_blinks)
			{
				/*Hit the num of blinks*/
				
				/*Advance to the next sequence*/
				inst->num_of_blinks++;
				
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
			break;
			
			case SEQ_SEPARATION:
			inst->count++;
			
			if(inst->count >= inst->conf.blink_separation)
			{
				inst->state = GET_NEXT_SEQ;
			}
			break;
		}
		
		inst->last_tick_ms = *inst->conf.tick_ms_ptr;
	}
	
	return inst->out;
}

/******************************************************************************
*  \brief Blink set
*
*  \note sets num of blinks by bit position num_blinks = 0 turns all off
******************************************************************************/
void blink_set(blink_inst_t *inst, uint8_t num_blinks, uint8_t on_off)
{
	if(num_blinks == 0)
	{
		inst->blink_bit = 0;
		
		return;
	}
	
	if(on_off == 1)
	{
		inst->blink_bit |= (1 << num_blinks);
	}
	else if(on_off == 0)
	{
		inst->blink_bit &= ~(1 << num_blinks);
	}
}

