/*
 * bsp_strutils.h
 *
 *  Created on: 11.04.2016
 *      Author: mfreudenberg
 */

#ifndef INC_BSP_STRUTILS_H_
#define INC_BSP_STRUTILS_H_

#include <stdint.h>

#define MAX_WORKING_STRLEN 128
#define STRING_FLOAT_FACTOR 100

#define MINUTES_IN_HOUR 60UL
#define SECONDS_IN_MINUTE 60UL
#define SECONDS_IN_HOUR ( MINUTES_IN_HOUR * SECONDS_IN_MINUTE )

void appendDecimalToString( char* str, int32_t n, char delimiter );
void convertSecondsToTimeString( char* str, int32_t time_s, char delimiter );
void convertFloatToString( char* str, float* float_handle, char decimalSign );
void intToHexString( char* str, int32_t int_num );
int32_t hexStringToInt( char* str );
uint16_t getNumPlaces( int32_t n );
uint8_t _isDigit( char character );
#ifdef NO_STD_ITOA
uint8_t* itoa( int32_t num, uint8_t* str, int32_t base );
void reverse( uint8_t s[] );
#endif

#endif /* INC_BSP_STRUTILS_H_ */
