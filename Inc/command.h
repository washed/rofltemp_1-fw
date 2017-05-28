/*
 * command.h
 *
 *  Created on: 25.05.2017
 *      Author: washed
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#define MAX_TOKENS 5

#define MAX_ID_TOKEN_PAIRS 50

#define __conststr ( const char* )
#define ID_INVALID -1

/**
 * Command tokens
 */

#define CMD_TOKEN_SET "set"
#define CMD_TOKEN_GET "get"
#define CMD_TOKEN_HELP "help"

/**
 * Command IDs
 */

#define CMD_ID_GET 0
#define CMD_ID_SET 1

/**
 * Parameter tokens
 */

#define PARAM_TOKEN_BRIGHTNESS "brightness"
#define PARAM_TOKEN_TEMPERATURE "temperature"

/**
 * Parameter IDs
 */

#define PARAM_ID_BRIGHTNESS 0
#define PARAM_ID_TEMPERATURE 1

typedef struct ID_Token_Pair_TypeDef
{
  uint32_t id;
  const char* token;
} ID_Token_Pair_TypeDef;

void ReceiveCommand( uint8_t* Buf, uint32_t* Len );

#endif /* COMMAND_H_ */
