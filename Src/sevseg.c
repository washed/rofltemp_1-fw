/*
 * sevseg.c
 *
 *  Created on: 17.04.2017
 *      Author: washed
 */

#include "stm32f0xx_hal.h"
#include "tim.h"
#include "sevseg.h"
#include "MAX31865.h"

static volatile uint8_t display_update = 0;

/*
const uint32_t sevSegSymbolArray[SEVSEG_SYMBOLS] = {
		( ~SEVSEG_GPIO_CAT_MASK | (SEVSEG_GPIO_CAT_MASK >> 16) ),
		( ~(SEVSEG_TOP | SEVSEG_BOT | SEVSEG_TOP_LEFT | SEVSEG_TOP_RIGHT | SEVSEG_BOT_LEFT | SEVSEG_BOT_RIGHT) | SEVSEG_GPIO_CAT_MASK ),
		( ~(SEVSEG_TOP_RIGHT | SEVSEG_BOT_RIGHT) | SEVSEG_GPIO_CAT_MASK ),
		( ~(SEVSEG_TOP | SEVSEG_TOP_RIGHT | SEVSEG_MID | SEVSEG_BOT_LEFT | SEVSEG_BOT) | SEVSEG_GPIO_CAT_MASK ),
		( ~(SEVSEG_TOP | SEVSEG_TOP_RIGHT | SEVSEG_MID | SEVSEG_BOT_RIGHT | SEVSEG_BOT) | SEVSEG_GPIO_CAT_MASK ),
		( ~(SEVSEG_TOP_LEFT | SEVSEG_MID | SEVSEG_TOP_RIGHT | SEVSEG_BOT_RIGHT) | SEVSEG_GPIO_CAT_MASK ),
		( ~(SEVSEG_TOP | SEVSEG_TOP_LEFT | SEVSEG_MID | SEVSEG_BOT_RIGHT | SEVSEG_BOT) | SEVSEG_GPIO_CAT_MASK ),
		( ~(SEVSEG_TOP | SEVSEG_TOP_LEFT | SEVSEG_MID | SEVSEG_BOT_LEFT | SEVSEG_BOT | SEVSEG_BOT_RIGHT) | SEVSEG_GPIO_CAT_MASK ),
		( ~(SEVSEG_TOP | SEVSEG_TOP_RIGHT | SEVSEG_BOT_RIGHT) | SEVSEG_GPIO_CAT_MASK ),
		( ~(SEVSEG_TOP | SEVSEG_MID | SEVSEG_BOT | SEVSEG_TOP_LEFT | SEVSEG_TOP_RIGHT | SEVSEG_BOT_LEFT | SEVSEG_BOT_RIGHT) | SEVSEG_GPIO_CAT_MASK ),
		( ~(SEVSEG_TOP | SEVSEG_MID | SEVSEG_BOT | SEVSEG_TOP_LEFT | SEVSEG_TOP_RIGHT | SEVSEG_BOT_RIGHT) | SEVSEG_GPIO_CAT_MASK )
};
*/

const uint32_t CathodePorts[8] = {
		  CA_7SEG_A_GPIO_Port, // 0
		  CA_7SEG_B_GPIO_Port, // 1
		  CA_7SEG_C_GPIO_Port, // 2
		  CA_7SEG_D_GPIO_Port, // 3
		  CA_7SEG_E_GPIO_Port, // 4
		  CA_7SEG_F_GPIO_Port, // 5
		  CA_7SEG_G_GPIO_Port, // 6
		  CA_7SEG_DP_GPIO_Port, // 7
};

static const uint16_t CathodePins[8] = {
		  CA_7SEG_A_Pin, // 0
		  CA_7SEG_B_Pin, // 1
		  CA_7SEG_C_Pin, // 2
		 CA_7SEG_D_Pin, // 3
		   CA_7SEG_E_Pin, // 4
		  CA_7SEG_F_Pin, // 5
		  CA_7SEG_G_Pin, // 6
		  CA_7SEG_DP_Pin, // 7
};

uint32_t sevSegSymbolArray[SEVSEG_SYMBOLS];

const uint32_t sevSegDigitArray[SEVSEG_DIGITS] = {
		( SEVSEG_DIG_0 | SEVSEG_GPIO_AN_MASK ),
		( SEVSEG_DIG_1 | SEVSEG_GPIO_AN_MASK ),
		( SEVSEG_DIG_2 | SEVSEG_GPIO_AN_MASK )
};

int8_t sevSegValue[SEVSEG_DIGITS] = {
		1,
		2,
		3,
};

static void setSevSegValue(uint16_t value);
static void WriteSevSegDigitFast( uint8_t digit, int8_t number );

void initSevSeg()
{
	sevSegSymbolArray[0] = 0xFFFF << 16;
	sevSegSymbolArray[1] = ( CathodePins[SEVSEG_TOP] | CathodePins[SEVSEG_BOT] | CathodePins[SEVSEG_TOP_LEFT] | CathodePins[SEVSEG_TOP_RIGHT] | CathodePins[SEVSEG_BOT_LEFT] | CathodePins[SEVSEG_BOT_RIGHT] );
	sevSegSymbolArray[2] = ( CathodePins[SEVSEG_TOP_RIGHT] | CathodePins[SEVSEG_BOT_RIGHT] );
	sevSegSymbolArray[3] = ( CathodePins[SEVSEG_TOP] | CathodePins[SEVSEG_TOP_RIGHT] | CathodePins[SEVSEG_MID] | CathodePins[SEVSEG_BOT_LEFT] | CathodePins[SEVSEG_BOT] );
	sevSegSymbolArray[4] = ( CathodePins[SEVSEG_TOP] | CathodePins[SEVSEG_TOP_RIGHT] | CathodePins[SEVSEG_MID] | CathodePins[SEVSEG_BOT_RIGHT] | CathodePins[SEVSEG_BOT] );
	sevSegSymbolArray[5] = ( CathodePins[SEVSEG_TOP_LEFT] | CathodePins[SEVSEG_MID] | CathodePins[SEVSEG_TOP_RIGHT] | CathodePins[SEVSEG_BOT_RIGHT] );
	sevSegSymbolArray[6] = ( CathodePins[SEVSEG_TOP] |  CathodePins[SEVSEG_TOP_LEFT] |  CathodePins[SEVSEG_MID] |  CathodePins[SEVSEG_BOT_RIGHT] |  CathodePins[SEVSEG_BOT] );
	sevSegSymbolArray[7] = ( CathodePins[SEVSEG_TOP] | CathodePins[SEVSEG_TOP_LEFT] | CathodePins[SEVSEG_MID] | CathodePins[SEVSEG_BOT_LEFT] | CathodePins[SEVSEG_BOT] | CathodePins[SEVSEG_BOT_RIGHT] );
	sevSegSymbolArray[8] = ( CathodePins[SEVSEG_TOP] | CathodePins[SEVSEG_TOP_RIGHT] | CathodePins[SEVSEG_BOT_RIGHT] );
	sevSegSymbolArray[9] = ( CathodePins[SEVSEG_TOP] | CathodePins[SEVSEG_MID] | CathodePins[SEVSEG_BOT] | CathodePins[SEVSEG_TOP_LEFT] | CathodePins[SEVSEG_TOP_RIGHT] | CathodePins[SEVSEG_BOT_LEFT] | CathodePins[SEVSEG_BOT_RIGHT] );
	sevSegSymbolArray[10] = ( CathodePins[SEVSEG_TOP] | CathodePins[SEVSEG_MID] | CathodePins[SEVSEG_BOT] | CathodePins[SEVSEG_TOP_LEFT] | CathodePins[SEVSEG_TOP_RIGHT] | CathodePins[SEVSEG_BOT_RIGHT] );

  AnodeDutyCycles[0] = &(htim1.Instance->CCR1);
  AnodeDutyCycles[1] = &(htim1.Instance->CCR2);
  AnodeDutyCycles[2] = &(htim1.Instance->CCR3);

  *AnodeDutyCycles[0] = 0;
  *AnodeDutyCycles[1] = 0;
  *AnodeDutyCycles[2] = 0;

  HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_1 );
  HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_2 );
  HAL_TIM_PWM_Start( &htim1, TIM_CHANNEL_3 );

  HAL_GPIO_WritePin(CA_7SEG_A_GPIO_Port, CA_7SEG_A_Pin, GPIO_PIN_RESET );
  HAL_GPIO_WritePin(CA_7SEG_B_GPIO_Port, CA_7SEG_B_Pin, GPIO_PIN_RESET );
  HAL_GPIO_WritePin(CA_7SEG_C_GPIO_Port, CA_7SEG_C_Pin, GPIO_PIN_RESET );
  HAL_GPIO_WritePin(CA_7SEG_D_GPIO_Port, CA_7SEG_D_Pin, GPIO_PIN_RESET );
  HAL_GPIO_WritePin(CA_7SEG_E_GPIO_Port, CA_7SEG_E_Pin, GPIO_PIN_RESET );
  HAL_GPIO_WritePin(CA_7SEG_F_GPIO_Port, CA_7SEG_F_Pin, GPIO_PIN_RESET );
  HAL_GPIO_WritePin(CA_7SEG_G_GPIO_Port, CA_7SEG_G_Pin, GPIO_PIN_RESET );
  HAL_GPIO_WritePin(CA_7SEG_DP_GPIO_Port, CA_7SEG_DP_Pin, GPIO_PIN_RESET );

  setSevSegValue(0);

  HAL_TIM_Base_Start_IT(&htim14);
}

static void setSevSegValue(uint16_t value)
{
if ( (value >= 0) && (value < 1000) )
{
	if ( value >= 100 )
	{
		sevSegValue[0] = value / 100;
		value = value % 100;
	}
	else
	{
		sevSegValue[0] = -1;
		value = value % 100;
	}

	if ( value >= 10 )
	{
		sevSegValue[1] = value / 10;
		value = value % 10;
	}
	else
	{
		sevSegValue[1] = 0;
		value = value % 10;
	}

	sevSegValue[2] = value;

}
}

void handleSevSeg()
{
  static uint32_t current_digit = 0;

  if ( display_update == 1 )
  {
    display_update = 0;
    //setSevSegValue( averaged_RTD_temp / ( TEMP_INT_FACTOR / 10 ) );
	WriteSevSegDigitFast(current_digit, sevSegValue[current_digit] );
	if( ++current_digit >= SEVSEG_DIGITS) current_digit = 0;
  }
}

static void WriteSevSegDigitFast( uint8_t digit, int8_t number )
{
	static uint8_t last_digit = 0;

	//*AnodeDutyCycles[last_digit] = 0;
	//GPIOB->BSRR = 0xFFFF << 16;
  //HAL_Delay(1);
	if ( (number >= -1) && ( number < SEVSEG_SYMBOLS) && (digit >= 0) && (digit < SEVSEG_DIGITS) )
	  {

		  //GPIOC->BSRR = (uint32_t)sevSegDigitArray[digit];
		  GPIOB->BSRR = (uint32_t)sevSegSymbolArray[number + 1];
		  *AnodeDutyCycles[digit] = 1000;

		  last_digit = digit;
	  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim14)
	{
		display_update = 1;
	}
}
