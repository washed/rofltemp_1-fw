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

void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef* htim )
{
  if ( htim == &htim2 )
  {
    setSevSegValue( averaged_RTD_temp );
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
}
