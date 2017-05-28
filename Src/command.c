/*
 * command.c
 *
 *  Created on: 25.05.2017
 *      Author: washed
 */

#include "stm32f0xx_hal.h"

#include "command.h"
#include "sevseg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "usbd_cdc_if.h"

char command_rx_buffer[ 64 ];
char command_tx_buffer[ 64 ];

volatile uint8_t commandssqueued = 0;

const ID_Token_Pair_TypeDef CommandSet[ MAX_ID_TOKEN_PAIRS ] = {
  { CMD_ID_GET, CMD_TOKEN_GET },  //
  { CMD_ID_SET, CMD_TOKEN_SET }   //
};

const ID_Token_Pair_TypeDef ParameterSet[ MAX_ID_TOKEN_PAIRS ] = {
  { PARAM_ID_BRIGHTNESS, PARAM_TOKEN_BRIGHTNESS },   //
  { PARAM_ID_TEMPERATURE, PARAM_TOKEN_TEMPERATURE }  //
};

/**
 *  Private function prototypes
 */
static void SplitCommand();
static uint32_t getIDFromToken( const ID_Token_Pair_TypeDef* ID_Token_Pair, char* token );
static void defaultReply( uint32_t command_Id, uint32_t param_id, uint32_t value, float* fp );

void ReceiveCommand( uint8_t* Buf, uint32_t* Len )
{
  if ( commandssqueued == 0 )
  {
    memcpy( command_rx_buffer, Buf, *Len );
    if ( command_rx_buffer[ *Len ] == 0 ) commandssqueued = 1;
  }
  else
  {
    // Answer: sorry busy!
  }
}

void HandleQueuedCommand()
{
  if ( commandssqueued > 0 )
  {
    SplitCommand();

    commandssqueued = 0;
  }
}

void getTemperature()
{
  /*
itoa( averaged_RTD_temp, tx_buffer, 10 );
strncat( tx_buffer, "\r\n", 2 );
CDC_Transmit_FS( tx_buffer, strlen( tx_buffer ) );
*/
}

static void SplitCommand()
{
  char* tokens[ MAX_TOKENS ];
  uint32_t current_token_index = 0;
  uint32_t temp = 0;

  uint32_t command_id = ID_INVALID;
  ;
  uint32_t parameter_id = ID_INVALID;

  tokens[ current_token_index ] = strtok( command_rx_buffer, " " );
  while ( ( tokens[ current_token_index ] != NULL ) && ( ++current_token_index < MAX_TOKENS ) )
    tokens[ current_token_index ] = strtok( NULL, " " );

  command_id = getIDFromToken( CommandSet, tokens[ 0 ] );
  parameter_id = getIDFromToken( ParameterSet, tokens[ 1 ] );

  switch ( command_id )
  {
    // Get command received
    case CMD_ID_GET:
      switch ( parameter_id )
      {
        case PARAM_ID_BRIGHTNESS:
          defaultReply( command_id, parameter_id, getBrightness(), NULL );
          break;
        case ID_INVALID:
        default:
          // Huh? Say what now?
          break;
      }

      break;

    // Set command received
    case CMD_ID_SET:

      switch ( parameter_id )
      {
        case PARAM_ID_BRIGHTNESS:
          temp = atoi( tokens[ 2 ] );
          if ( ( temp >= 0 ) && ( temp <= 255 ) )
          {
            setBrightness( temp );
            defaultReply( command_id, parameter_id, getBrightness(), NULL );
            return;
          }
          break;

        case ID_INVALID:
        default:
          // Huh? Say what now?
          break;
      }

      break;

    case ID_INVALID:
    default:
      // Huh? Say what now?
      break;
  }
}

static void defaultReply( uint32_t command_Id, uint32_t parameter_id, uint32_t value, float* fp )
{
  char temp_string[ 16 ];
  strncat( command_tx_buffer, ParameterSet[ parameter_id ].token, strlen( ParameterSet[ parameter_id ].token ) );
  strncat( command_tx_buffer, " is ", 4 );

  if ( fp == NULL ) utoa( value, temp_string, 10 );
  // TODO: else float!
  strncat( command_tx_buffer, temp_string, strlen( temp_string ) + 1 );

  CDC_Transmit_FS( (uint8_t*)command_tx_buffer, strlen( command_tx_buffer ) + 1 );
  command_tx_buffer[ 0 ] = 0;
}

static uint32_t getIDFromToken( const ID_Token_Pair_TypeDef* ID_Token_Pair, char* token )
{
  uint32_t id = ID_INVALID;

  for ( uint32_t i = 0; i < MAX_ID_TOKEN_PAIRS; i++ )
  {
    if ( strcmp( ID_Token_Pair[ i ].token, token ) == 0 )
    {
      id = ID_Token_Pair[ i ].id;
      break;
    }
  }

  return id;
}
