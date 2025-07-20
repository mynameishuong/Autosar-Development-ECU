/**********************************************************
 * @file    Pwm_Lcfg.c
 * @brief   PWM Driver Configuration Source File (AUTOSAR)
 * @details Cấu hình các kênh PWM dùng cho STM32F103 (ví dụ: TIM2_CH1/PA0, TIM3_CH2/PA7)
 * @version 1.0
 **********************************************************/

#include "Pwm_cfg.h"
#include "stm32f10x_gpio.h"
/* ==== Ví dụ hàm callback cho PWM notification ==== */
void TIM2_IRQHandler(void)
{
    static uint8 valcheck = 0;

    // Ví dụ: đặt breakpoint, bật LED, debug, v.v.
    // printf("PWM Channel 0 interrupt/callback!\n");
    //GPIO_ReSetBits(GPIOC, GPIO_Pin_13);
    valcheck++;
    // ngắt quay lại học, nhưng cũng nói qua, thằng này kiểu khi có ngắt xảy ra, cụ thể là hàm
    Pwm_EnableNotification(0, PWM_RISING_EDGE); // hoặc PWM_RISING_EDGE, PWM_FALLING_EDGE
    // sẽ kích hoạt ngắt
}
/* ==== Cấu hình từng kênh PWM ==== */
const Pwm_ChannelConfigType pwmChannelscfg[PinPWM] = {
    /* Channel 0: PA0 - TIM2_CH1, có callback */
    {
        .TIMx             = TIM2,
        .channel          = 4,
        .classType        = PWM_VARIABLE_PERIOD,
        .defaultPeriod    = 999,          // 1ms (72MHz/72/1000)
        .defaultDutyCycle = 0,       // Duty 0%
        .polarity         = PWM_HIGH,
        .idleState        = PWM_LOW,
        .CompareVal       = 500,
        .notificationEnable =  1,
        .NotificationCb   = TIM2_IRQHandler   // Callback không NULL!TIM2_IRQHandler
    },
    /* Channel 1: PA6 - TIM3_CH2, không dùng callback */
    {
        .TIMx             = TIM3,
        .channel          = 1,
        .classType        = PWM_VARIABLE_PERIOD,
        .defaultPeriod    = 999,
        .defaultDutyCycle = 0,
        .polarity         = PWM_HIGH,
        .idleState        = PWM_LOW,
        .CompareVal       = 0,
        .notificationEnable =  0,
        .NotificationCb   = NULL_PTR
    }
};

/* ==== Cấu hình tổng PWM driver ==== */
