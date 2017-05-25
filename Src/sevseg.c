/*
 * sevseg.c
 *
 *  Created on: 17.04.2017
 *      Author: washed
 */

#include "sevseg.h"
#include "MAX31865.h"
#include "stm32f0xx_hal.h"
#include "tim.h"

const uint32_t AnodePins[ 3 ] = { AN_7SEG_0_Pin, AN_7SEG_1_Pin, AN_7SEG_2_Pin };

static const uint16_t CathodePins[ 8 ] = {
  CA_7SEG_A_Pin,   // 0
  CA_7SEG_B_Pin,   // 1
  CA_7SEG_C_Pin,   // 2
  CA_7SEG_D_Pin,   // 3
  CA_7SEG_E_Pin,   // 4
  CA_7SEG_F_Pin,   // 5
  CA_7SEG_G_Pin,   // 6
  CA_7SEG_DP_Pin,  // 7
};

uint32_t sevSegSymbolArray[ SEVSEG_SYMBOLS ];

int8_t sevSegValue[ SEVSEG_DIGITS ] = { -1, -1, -1 };

int32_t sevSegDPPosition = -1;

static void WriteSevSegDigitFast( uint8_t digit, int8_t number );

void initSevSeg()
{
  sevSegSymbolArray[ 0 ] = SEVSEG_GPIO_CAT_MASK << 16;
  sevSegSymbolArray[ 1 ] =
      ( CathodePins[ SEVSEG_TOP ] | CathodePins[ SEVSEG_BOT ] | CathodePins[ SEVSEG_TOP_LEFT ] |
        CathodePins[ SEVSEG_TOP_RIGHT ] | CathodePins[ SEVSEG_BOT_LEFT ] | CathodePins[ SEVSEG_BOT_RIGHT ] );
  sevSegSymbolArray[ 2 ] = ( CathodePins[ SEVSEG_TOP_RIGHT ] | CathodePins[ SEVSEG_BOT_RIGHT ] );
  sevSegSymbolArray[ 3 ] = ( CathodePins[ SEVSEG_TOP ] | CathodePins[ SEVSEG_TOP_RIGHT ] | CathodePins[ SEVSEG_MID ] |
                             CathodePins[ SEVSEG_BOT_LEFT ] | CathodePins[ SEVSEG_BOT ] );
  sevSegSymbolArray[ 4 ] = ( CathodePins[ SEVSEG_TOP ] | CathodePins[ SEVSEG_TOP_RIGHT ] | CathodePins[ SEVSEG_MID ] |
                             CathodePins[ SEVSEG_BOT_RIGHT ] | CathodePins[ SEVSEG_BOT ] );
  sevSegSymbolArray[ 5 ] = ( CathodePins[ SEVSEG_TOP_LEFT ] | CathodePins[ SEVSEG_MID ] |
                             CathodePins[ SEVSEG_TOP_RIGHT ] | CathodePins[ SEVSEG_BOT_RIGHT ] );
  sevSegSymbolArray[ 6 ] = ( CathodePins[ SEVSEG_TOP ] | CathodePins[ SEVSEG_TOP_LEFT ] | CathodePins[ SEVSEG_MID ] |
                             CathodePins[ SEVSEG_BOT_RIGHT ] | CathodePins[ SEVSEG_BOT ] );
  sevSegSymbolArray[ 7 ] =
      ( CathodePins[ SEVSEG_TOP ] | CathodePins[ SEVSEG_TOP_LEFT ] | CathodePins[ SEVSEG_MID ] |
        CathodePins[ SEVSEG_BOT_LEFT ] | CathodePins[ SEVSEG_BOT ] | CathodePins[ SEVSEG_BOT_RIGHT ] );
  sevSegSymbolArray[ 8 ] =
      ( CathodePins[ SEVSEG_TOP ] | CathodePins[ SEVSEG_TOP_RIGHT ] | CathodePins[ SEVSEG_BOT_RIGHT ] );
  sevSegSymbolArray[ 9 ] = ( CathodePins[ SEVSEG_TOP ] | CathodePins[ SEVSEG_MID ] | CathodePins[ SEVSEG_BOT ] |
                             CathodePins[ SEVSEG_TOP_LEFT ] | CathodePins[ SEVSEG_TOP_RIGHT ] |
                             CathodePins[ SEVSEG_BOT_LEFT ] | CathodePins[ SEVSEG_BOT_RIGHT ] );
  sevSegSymbolArray[ 10 ] =
      ( CathodePins[ SEVSEG_TOP ] | CathodePins[ SEVSEG_MID ] | CathodePins[ SEVSEG_BOT ] |
        CathodePins[ SEVSEG_TOP_LEFT ] | CathodePins[ SEVSEG_TOP_RIGHT ] | CathodePins[ SEVSEG_BOT_RIGHT ] );

  // Anodes off
  GPIOA->BSRR = SEVSEG_GPIO_AN_MASK;

  // Cathodes off
  GPIOB->BSRR = sevSegSymbolArray[ 0 ];

  // Display value 0
  setSevSegValue( 0 );

  // Set brightness/duty cycle (0-1000)
  htim2.Instance->CCR1 = 500;

  // Set fixed decimal point
  sevSegDPPosition = 1;

  HAL_TIM_Base_Start_IT( &htim14 );

  // Start Timer 2 ("digit-clock")
  HAL_TIM_Base_Start_IT( &htim2 );
  HAL_TIM_PWM_Start_IT( &htim2, TIM_CHANNEL_1 );
}

void handleSevSeg( uint8_t set )
{
  static uint32_t current_digit = 0;
  if ( set == 1 )
  {
    WriteSevSegDigitFast( current_digit, sevSegValue[ current_digit ] );
    if ( ++current_digit >= SEVSEG_DIGITS ) current_digit = 0;
  }
  else
  {
    WriteSevSegDigitFast( current_digit, -1 );
  }
}

void setSevSegDP( int32_t position )
{
  sevSegDPPosition = position;
}

void toggleSevSegDP( int32_t position )
{
  static uint8_t toggle = 0;
  if ( toggle == 0 )
  {
    sevSegDPPosition = -1;
    toggle = 1;
  }
  else
  {
    sevSegDPPosition = position;
    toggle = 0;
  }
}

void setSevSegValue( uint16_t value )
{
  if ( ( value >= 0 ) && ( value < 1000 ) )
  {
    if ( value >= 100 )
    {
      sevSegValue[ 0 ] = value / 100;
      value = value % 100;
    }
    else
    {
      sevSegValue[ 0 ] = -1;
      value = value % 100;
    }

    if ( value >= 10 )
    {
      sevSegValue[ 1 ] = value / 10;
      value = value % 10;
    }
    else
    {
      sevSegValue[ 1 ] = 0;
      value = value % 10;
    }

    sevSegValue[ 2 ] = value;
  }
}

static void WriteSevSegDigitFast( uint8_t digit, int8_t number )
{
  uint32_t dp = 0;
  ;

  if ( sevSegDPPosition == -1 )
    dp = 0;
  else if ( digit == sevSegDPPosition )
    dp = CathodePins[ SEVSEG_DP ];

  if ( ( number >= -1 ) && ( number < SEVSEG_SYMBOLS ) && ( digit >= 0 ) && ( digit < SEVSEG_DIGITS ) )
  {
    GPIOB->BSRR = sevSegSymbolArray[ number + 1 ] | dp;
    GPIOA->BSRR = ( SEVSEG_GPIO_AN_MASK ^ AnodePins[ digit ] ) | ( AnodePins[ digit ] << 16 );
  }
}
