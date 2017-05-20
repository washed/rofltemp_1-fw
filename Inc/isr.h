/*
 * isr.h
 *
 *  Created on: 20.05.2017
 *      Author: washed
 */

#ifndef ISR_H_
#define ISR_H_

void
HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim);
void
HAL_TIM_PWM_PulseFinishedCallback (TIM_HandleTypeDef *htim);

#endif /* ISR_H_ */
