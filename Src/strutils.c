/*
 * bsp_strutils.c
 *
 *  Created on: 11.04.2016
 *      Author: mfreudenberg
 */

#include "strutils.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief	Appends an integer number to a string and optionally adds a
 * delimiter after the number
 * 			Useful for creating strings for comma/tab separated recording
 * etc.
 *
 * @param str			String to append to
 * @param n				Number to convert and append
 * @param delimiter		Delimiter character to add. Pass 0xFF if no
 * delimiter should be added
 */
void appendDecimalToString( char* str, int32_t int_num, char delimiter )
{
  char tempstr[ MAX_WORKING_STRLEN ];
  itoa( int_num, tempstr, 10 );
  if ( (uint8_t)delimiter != 0xFF ) strncat( tempstr, &delimiter, 1 );
  strncat( str, tempstr, 64 );
}

/**
 * @brief	Converts an integer number to a hex string without leading
 * zeroes
 *
 * @param str		Destination string where the hex number is to be stored
 * @param int_num	Integer number to convert
 */
void intToHexString( char* str, int32_t int_num )
{
  int32_t a;
  short i, s;
  uint8_t no_leading_zero;
  i = 0;
  if ( int_num )
  {
    s = 32;
    no_leading_zero = 0;
    do
    {
      s = s - 4;
      a = ( int_num >> s ) & 0xf;
      if ( a ) no_leading_zero = 1;
      if ( no_leading_zero )
      {
        if ( a < 10 )
          str[ i++ ] = a + 48;
        else
          str[ i++ ] = a + 55;
      }
    } while ( s );
  }
  else
    str[ i++ ] = 48;
  str[ i ] = 0;
}

/**
 * @brief		Converts a string containing hex-digits to an integer
 * number
 * 				i.e.: "DEADBEEF" --> -559038737
 * @param str	String containing the hex digits
 *
 * @return		The integer value of the supplied hex string
 */
int32_t hexStringToInt( char* str )
{
  int32_t result, a;
  result = 0;
  for ( ; *str != 0; str++ )
  {
    if ( _isDigit( *str ) )
      a = *str - 48;
    else
      a = *str - 55;
    result = ( result << 4 ) + a;
  }
  return ( result );
}

void convertSecondsToTimeString( char* str, int32_t time_s, char delimiter )
{
  uint32_t hours = 0, minutes = 0, seconds = 0;
  uint8_t temp_str[ 4 ];
  char minus_sign = '-';
  const char* zeroes = "00";

  memset( str, 0, 10 );

  if ( time_s < 0 )
  {
    strncat( str, &minus_sign, 1 );
    time_s = -time_s;
  }

  hours = time_s / SECONDS_IN_HOUR;
  time_s = time_s - ( hours * SECONDS_IN_HOUR );

  minutes = time_s / SECONDS_IN_MINUTE;
  time_s = time_s - ( minutes * SECONDS_IN_MINUTE );

  seconds = time_s;

  if ( hours == 0 )
    strncat( str, zeroes, 2 );
  else if ( hours < 10 )
  {
    strncat( str, &zeroes[ 0 ], 1 );
    itoa( hours, temp_str, 10 );
    strncat( str, temp_str, 1 );
  }
  else
  {
    itoa( hours, temp_str, 10 );
    strncat( str, temp_str, 2 );
  }
  strncat( str, &delimiter, 1 );

  if ( minutes == 0 )
    strncat( str, zeroes, 2 );
  else if ( minutes < 10 )
  {
    strncat( str, &zeroes[ 0 ], 1 );
    itoa( minutes, temp_str, 10 );
    strncat( str, temp_str, 1 );
  }
  else
  {
    itoa( minutes, temp_str, 10 );
    strncat( str, temp_str, 2 );
  }
  strncat( str, &delimiter, 1 );

  if ( seconds == 0 )
    strncat( str, zeroes, 2 );
  else if ( seconds < 10 )
  {
    strncat( str, &zeroes[ 0 ], 1 );
    itoa( seconds, temp_str, 10 );
    strncat( str, temp_str, 1 );
  }
  else
  {
    itoa( seconds, temp_str, 10 );
    strncat( str, temp_str, 2 );
  }
}

/**
 * @brief				Converts a floating point number to a string
 * representation with user selectable decimal sign
 *
 * @param str			Destination string
 * @param float_handle	Pointer to the float number to convert
 * @param decimalSign	Sign to use for the decimal sign. Typically '.' or ','
 */
void convertFloatToString( char* str, float* float_handle, char decimalSign )
{
  int32_t integer_part;
  uint32_t non_integer_part;
  uint16_t digits_integer_part, digits_non_integer_part;
  uint8_t leading_zeroes;
  float remainder;

  if ( str == NULL ) return;

  // Check for value outside of float range
  if ( ( *float_handle > 2147483648 ) || ( *float_handle < -2147483648 ) ) return;

  integer_part = labs( ( int32_t )( *float_handle ) );
  digits_integer_part = getNumPlaces( integer_part );

  // Add - sign if number is negative
  if ( *float_handle < 0 ) *str++ = '-';

  // Subtract integer part to get the remainder
  remainder = fabs( *float_handle - (float)( integer_part ) );

// Check for leading zeros in the remainder:
// This needs to be adapted if STRING_FLOAT_FAKTOR is changed!!!
#if STRING_FLOAT_FACTOR > 1000000
#error "STRING_FLOAT_FACTOR has to be 1000000 for 6 post decimal places, otherwise this function has to be adapted!"
#endif

  if ( remainder >= 0.1 )
    leading_zeroes = 0;
  else if ( remainder <= 0.000001 )
    leading_zeroes = 0;
  else if ( remainder <= 0.00001 )
    leading_zeroes = 5;
  else if ( remainder <= 0.0001 )
    leading_zeroes = 4;
  else if ( remainder <= 0.001 )
    leading_zeroes = 3;
  else if ( remainder <= 0.01 )
    leading_zeroes = 2;
  else if ( remainder <= 0.1 )
    leading_zeroes = 1;
  else
    leading_zeroes = 0;

  // TODO: use proper rounding here?
  non_integer_part = ( uint32_t )( remainder * (float)( STRING_FLOAT_FACTOR ) + 0.5 );

  // The number of digits after the decimal point:
  digits_non_integer_part = getNumPlaces( non_integer_part ) + leading_zeroes;

  // Convert the pre decimal part of the number to characters and add to the
  // string
  itoa( integer_part, str, 10 );

  // Add the decimal sign to the string
  *( str++ + digits_integer_part ) = decimalSign;

  // Add the required amount of leading zeros to the string after the decimal
  // sign:
  memset( ( str + digits_integer_part ), '0', leading_zeroes );

  // Convert the post decimal part of the number to characters
  itoa( non_integer_part, ( str + ( digits_integer_part + leading_zeroes ) ), 10 );
  str[ digits_integer_part + digits_non_integer_part + 1 ] = '\0';
}

/**
 * @brief			Returns the number of decimal places in an integer
 * number
 *
 * @param int_num	Integer number in which to count the places
 * @return			Number of places in number
 */
uint16_t getNumPlaces( int32_t int_num )
{
  if ( int_num < 0 ) int_num = -int_num;
  if ( int_num < 10 ) return ( 1 );
  if ( int_num < 100 ) return ( 2 );
  if ( int_num < 1000 ) return ( 3 );
  if ( int_num < 10000 ) return ( 4 );
  if ( int_num < 100000 ) return ( 5 );
  if ( int_num < 1000000 ) return ( 6 );
  if ( int_num < 10000000 ) return ( 7 );
  if ( int_num < 100000000 ) return ( 8 );
  if ( int_num < 1000000000 ) return ( 9 );
  if ( int_num < 10000000000 ) return ( 10 );
  return ( 11 );
}

/**
 * @brief			Checks whether supplied character is an ASCII numerical
 * digit or not
 *
 * @param character	Character to check
 *
 * @return			TRUE if character is a digit (ie 0-9), otherwise
 * false.
 */
uint8_t _isDigit( char character )
{
  if ( ( character >= '0' ) && ( character <= '9' ) )
    return 1;
  else
    return 0;
}

#ifdef NO_STD_ITOA
uint8_t* itoa( int32_t num, uint8_t* str, int32_t base )
{
  int32_t i = 0;
  int32_t isNegative = 0;

  /* Handle 0 explicitely, otherwise empty string is printed for 0 */
  if ( num == 0 )
  {
    str[ i++ ] = '0';
    str[ i ] = '\0';
    return str;
  }

  // In standard itoa(), negative numbers are handled only with
  // base 10. Otherwise numbers are considered unsigned.
  if ( num < 0 && base == 10 )
  {
    isNegative = 1;
    num = -num;
  }

  // Process individual digits
  while ( num != 0 )
  {
    int rem = num % base;
    str[ i++ ] = ( rem > 9 ) ? ( rem - 10 ) + 'a' : rem + '0';
    num = num / base;
  }

  // If number is negative, append '-'
  if ( isNegative ) str[ i++ ] = '-';

  str[ i ] = '\0';  // Append string terminator

  // Reverse the string
  reverse( str );

  return str;
}

/* A utility function to reverse a string  */
void reverse( uint8_t s[] )
{
  int32_t i, j;
  uint8_t c;

  for ( i = 0, j = strlen( s ) - 1; i < j; i++, j-- )
  {
    c = s[ i ];
    s[ i ] = s[ j ];
    s[ j ] = c;
  }
}
#endif
