/**********************************************************
 * @file    Pwm_Lcfg.h
 * @brief   PWM Driver Configuration Header File (AUTOSAR)
 * @details Khai báo extern biến cấu hình tổng của PWM Driver cho STM32F103.
 * @version 1.0
 * @date    2024-06-27
 * @author  HALA Academy
 **********************************************************/
#ifndef PWM_CFG_H
#define PWM_CFG_H

#include "Pwm.h"
/**********************************************************
 *
 * @brief   Biến cấu hình tổng cho PWM Driver
 **********************************************************/
#define PinPWM     12     // Tổng số chân của STM32 có trong 4 port

extern const Pwm_ChannelConfigType pwmChannelscfg[PinPWM];

#endif /* PWM_CFG_H */
