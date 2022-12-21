/*
 * blink.c
 *
 * Created: 11/30/2018 4:53:30 PM
 *  Author: akudlacek
 */ 


#include "blink.h"
#include "timer.h"


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
	conf->on_time_tick      = 200;
	conf->off_time_tick     = 350;
	conf->sep_time_tick     = 1000;
	conf->prsst_time_tick   = 5000;
	conf->polarity          = 0;
}

/******************************************************************************
*  \brief Blink init
*
*  \note
******************************************************************************/
void blink_init(blink_inst_t * const inst, blink_conf_t const conf)
{
	inst->conf            = conf;
	inst->bit_flg         = 0;
	inst->last_bit_flg    = 0;
	inst->state           = GET_NEXT_SEQ;
	inst->bit_num         = 0;
	inst->out             = 0;
	inst->count           = 0;
	tmrReset(&inst->prev_prsst_tick);
	tmrReset(&inst->prev_tick);
}

/******************************************************************************
*  \brief Blink task
*
*  \note return value is blink output
******************************************************************************/
uint8_t blink_task(blink_inst_t * const inst)
{
	//if no blink bit set and done blinking go to idle value
	if(blink_idle(inst))
	{
		inst->out = 0;
	}
	//run state machine
	else
	{
		//turn on bits
		inst->prsst_bit_flg |= inst->bit_flg;
		
		/*This block of code before the state machine handles keeping the faults blinking for a min amount of time after it is set*/
		//if bit flag changed to 1
		if((inst->bit_flg ^ inst->last_bit_flg) & inst->bit_flg)
		{
			tmrReset(&inst->prev_prsst_tick); //reset persistence timer
		}
		
		//persistence timer elapsed
		if (tmrCheck(&inst->prev_prsst_tick, inst->conf.prsst_time_tick))
		{
			//clear bits
			inst->prsst_bit_flg &= inst->bit_flg;
		}
		
		//record last bit flag
		inst->last_bit_flg = inst->bit_flg;
		
		switch(inst->state)
		{
			case GET_NEXT_SEQ:
			//no live blink, go to next bit
			if(((inst->prsst_bit_flg >> inst->bit_num) & 1u) == 0)
			{
				inst->bit_num++;
				
				//wrap
				if(inst->bit_num > BLINKS_MAX)
				{
					inst->bit_num = 0;
				}
				
				inst->out = 0;
			}
			//live bit found
			else
			{
				inst->count     = 0;
				inst->out       = 1;
				inst->state     = BLINKING;
				tmrReset(&inst->prev_tick);
			}
			break;
			
			case BLINKING:
			//time based on if it on or off because there are 2 different thresholds.
			if (tmrCheckReset(&inst->prev_tick, (inst->out ? inst->conf.on_time_tick : inst->conf.off_time_tick)))
			{
				inst->count++;
				inst->out ^= 1;                //toggles output
				
				//if hit number of blinks
				if((inst->count/2) > inst->bit_num)
				{
					inst->bit_num++; //Advance to the next sequence, so it does not get stuck running same bit
					
					//wrap
					if(inst->bit_num > BLINKS_MAX)
					{
						inst->bit_num = 0;
					}
					
					//if no separation go to GET_NEXT_SEQ state
					if(inst->conf.sep_time_tick == 0)
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
			}
			break;
				
			case SEQ_SEPARATION:
			if (tmrCheckReset(&inst->prev_tick, inst->conf.sep_time_tick))
			{
				inst->state = GET_NEXT_SEQ;
			}
			break;
			
			default:
			//something bad happened
			//this assumes config is valid.
			blink_init(inst, inst->conf);
			break;
		} /* switch */
	}
	
	return ((inst->out ^ inst->conf.polarity) & 1u);
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
		inst->bit_flg = 0;
		
		return;
	}
	
	num_blinks--;
	
	if(on_off == 1)
	{
		inst->bit_flg |= (1 << num_blinks);
	}
	else if(on_off == 0)
	{
		inst->bit_flg &= ~(1 << num_blinks);
	}
}

/******************************************************************************
*  \brief Blink idle?
*
*  \note returns 1 for idle and 0 while not complete with sequence
******************************************************************************/
uint8_t blink_idle(blink_inst_t * const inst)
{
	return (inst->prsst_bit_flg == 0) && (inst->bit_flg == 0) && (inst->state == GET_NEXT_SEQ);
}