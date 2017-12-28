/*
 * isr.c
 *
 *  Created on: 20.05.2017
 *      Author: washed
 */

#include "stm32f0xx_hal.h"
#include "tim.h"

#include "MAX31865.h"
#include "sevseg.h"

volatile uint8_t freeze_display = 0;

extern uint32_t start_time;

void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef* htim )
{
  if ( htim == &htim2 )
  {
	  if ( 0 == freeze_display )
	  {
	    setSevSegValue( averaged_RTD_temp );
	  }
    handleSevSeg( 1 );
  }
}

void HAL_TIM_PWM_PulseFinishedCallback( TIM_HandleTypeDef* htim )
{
  if ( htim == &htim2 )
  {
    handleSevSeg( 0 );
  }
}

void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{
  if ( GPIO_Pin == MAX31865_0_DR_PIN )
  {
    MAX31865_DEVICES_SAMPLE_READY[ 0 ] = 1;
  }
  else if ( GPIO_PIN_1 == GPIO_Pin )
  {
	  if ( start_time != 0 )
	  {
		  if ( 0 == freeze_display )
		  {
			  freeze_display = 1;
		  }
		  else if ( 1 == freeze_display )
		  {
			  freeze_display = 0;
		  }
	  }

  }
}

