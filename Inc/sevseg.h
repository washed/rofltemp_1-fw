/*
 * sevseg.h
 *
 *  Created on: 17.04.2017
 *      Author: washed
 */

#ifndef SEVSEG_H_
#define SEVSEG_H_

#define SEVSEG_SYMBOLS 11
#define SEVSEG_DIGITS 3

#define SEVSEG_GPIO_CAT_MASK ( CA_7SEG_A_Pin | CA_7SEG_B_Pin | CA_7SEG_C_Pin | CA_7SEG_D_Pin | CA_7SEG_E_Pin | CA_7SEG_F_Pin | CA_7SEG_G_Pin | CA_7SEG_DP_Pin )
#define SEVSEG_GPIO_AN_MASK (AN_7SEG_0_Pin | AN_7SEG_1_Pin | AN_7SEG_2_Pin)
#define SEVSEG_GPIO_CAT_PORT GPIOA
#define SEVSEG_GPIO_AN_PORT GPIOB

#define SEVSEG_DIG_0 0
#define SEVSEG_DIG_1 1
#define SEVSEG_DIG_2 2

#define SEVSEG_TOP 0
#define SEVSEG_MID 6
#define SEVSEG_BOT 3
#define SEVSEG_TOP_LEFT 5
#define SEVSEG_TOP_RIGHT 1
#define SEVSEG_BOT_LEFT 4
#define SEVSEG_BOT_RIGHT 2
#define SEVSEG_DP 7

__IO uint32_t* AnodeDutyCycles[3];

static const uint16_t CathodePins[8];

void initSevSeg();
void handleSevSeg(uint8_t set);
void
setSevSegDP (int32_t position);
void
setSevSegValue (uint16_t value);

#endif /* SEVSEG_H_ */
