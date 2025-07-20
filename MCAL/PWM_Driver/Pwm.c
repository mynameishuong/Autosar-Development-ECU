/**********************************************************
 * @file    Pwm.c
 * @brief   Trình điều khiển PWM (Pulse Width Modulation)
 * @details Cài đặt các API PWM theo chuẩn AUTOSAR cho STM32F103, sử dụng SPL (Standard Peripheral Library).
 *          Driver này chỉ quản lý chức năng PWM, không cấu hình GPIO.
 *
 * @version 1.0
 **********************************************************/

#include "Pwm.h"
#include "stm32f10x_tim.h"
#include "misc.h"
#include "Pwm_cfg.h"
/* ===============================
 *     Biến và hằng cục bộ
 * =============================== */

/* Lưu trữ con trỏ đến cấu hình hiện tại của PWM driver */
static const Pwm_ConfigType* Pwm_CurrentConfigPtr = NULL_PTR;

/* Biến trạng thái khởi tạo driver PWM */
static uint8 Pwm_IsInitialized = 0;

/* ===============================
 *      Định nghĩa hàm chức năng
 * =============================== */

/**********************************************************
 * @brief   Khởi tạo PWM driver với cấu hình được truyền vào
 * @details Bật clock, cấu hình timer (prescaler, period, mode)
 *
 * @param[in] ConfigPtr Con trỏ tới cấu hình PWM
 **********************************************************/
void Pwm_Init(const Pwm_ConfigType* ConfigPtr)
{
    if (Pwm_IsInitialized || ConfigPtr == NULL_PTR) return;

    Pwm_CurrentConfigPtr = ConfigPtr;

    for (uint8 i = 0; i < ConfigPtr->NumChannels; i++)
    {
        if (ConfigPtr->Channels[i].TIMx == TIM1)
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
        else if (ConfigPtr->Channels[i].TIMx == TIM2)
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        else if (ConfigPtr->Channels[i].TIMx == TIM3)
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
        else if (ConfigPtr->Channels[i].TIMx == TIM4)
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

        TIM_TimeBaseInitTypeDef tim;
        tim.TIM_ClockDivision = TIM_CKD_DIV1;
        tim.TIM_CounterMode = TIM_CounterMode_Up;
        tim.TIM_Period = ConfigPtr->Channels[i].defaultPeriod - 1;
        tim.TIM_Prescaler = 8 - 1; // sử dụng thạch anh 8MHz

        TIM_TimeBaseInit(ConfigPtr->Channels[i].TIMx, &tim);

        TIM_OCInitTypeDef oc;
        oc.TIM_OCMode = TIM_OCMode_PWM1;
        oc.TIM_OutputState = TIM_OutputState_Enable;
        oc.TIM_Pulse = ConfigPtr->Channels[i].CompareVal;
        oc.TIM_OCPolarity = TIM_OCPolarity_High;

        switch (ConfigPtr->Channels[i].channel)
        {
            case 1: TIM_OC1Init(ConfigPtr->Channels[i].TIMx, &oc); break;
            case 2: TIM_OC2Init(ConfigPtr->Channels[i].TIMx, &oc); break;
            case 3: TIM_OC3Init(ConfigPtr->Channels[i].TIMx, &oc); break;
            case 4: TIM_OC4Init(ConfigPtr->Channels[i].TIMx, &oc); break;
            default: break;
        }

        TIM_Cmd(ConfigPtr->Channels[i].TIMx, ENABLE);
    }

    Pwm_IsInitialized = 1;
}

/**********************************************************
 * @brief   Giải phóng tài nguyên và dừng các kênh PWM
 **********************************************************/
void Pwm_DeInit(void)
{
    if (!Pwm_IsInitialized) return;

    for (uint8 i = 0; i < Pwm_CurrentConfigPtr->NumChannels; i++)
    {
        const Pwm_ChannelConfigType* ch = &Pwm_CurrentConfigPtr->Channels[i];
        TIM_Cmd(ch->TIMx, DISABLE);
        if (ch->TIMx == TIM1) TIM_CtrlPWMOutputs(TIM1, DISABLE);
    }

    Pwm_IsInitialized = 0;
}

/**********************************************************
 * @brief   Đặt duty cycle cho kênh PWM
 * @param[in] ChannelNumber Số thứ tự kênh
 * @param[in] DutyCycle     Giá trị duty (0 - 0x8000 tương ứng 0-100%)
 **********************************************************/
void Pwm_SetDutyCycle(Pwm_ChannelType ChannelNumber, uint16 DutyCycle)
{
    if (!Pwm_IsInitialized || ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels) return;

    const Pwm_ChannelConfigType* ch = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];
    uint16_t period = ch->TIMx->ARR;
    uint16_t compare = ((uint32_t)period * DutyCycle) >> 15;

    switch (ch->channel)
    {
        case 1: ch->TIMx->CCR1 = compare; break;
        case 2: ch->TIMx->CCR2 = compare; break;
        case 3: ch->TIMx->CCR3 = compare; break;
        case 4: ch->TIMx->CCR4 = compare; break;
        default: break;
    }
}

/**********************************************************
 * @brief   Đặt period và duty cho PWM nếu hỗ trợ
 **********************************************************/
void Pwm_SetPeriodAndDuty(Pwm_ChannelType ChannelNumber, Pwm_PeriodType Period, uint16 DutyCycle)
{
    if (!Pwm_IsInitialized || ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels) return;
    const Pwm_ChannelConfigType* ch = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];
    if (ch->classType != PWM_VARIABLE_PERIOD) return;

    ch->TIMx->ARR = Period;
    uint16_t compare = ((uint32_t)Period * DutyCycle) >> 15;

    switch (ch->channel)
    {
        case 1: ch->TIMx->CCR1 = compare; break;
        case 2: ch->TIMx->CCR2 = compare; break;
        case 3: ch->TIMx->CCR3 = compare; break;
        case 4: ch->TIMx->CCR4 = compare; break;
        default: break;
    }
}

/**********************************************************
 * @brief   Đưa PWM về trạng thái tắt output
 **********************************************************/
void Pwm_SetOutputToIdle(Pwm_ChannelType ChannelNumber)
{
    if (!Pwm_IsInitialized || ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels) return;
    const Pwm_ChannelConfigType* ch = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    switch (ch->channel)
    {
        case 1: ch->TIMx->CCR1 = 0; break;
        case 2: ch->TIMx->CCR2 = 0; break;
        case 3: ch->TIMx->CCR3 = 0; break;
        case 4: ch->TIMx->CCR4 = 0; break;
        default: break;
    }
}

/**********************************************************
 * @brief   Đọc trạng thái đầu ra PWM
 **********************************************************/
Pwm_OutputStateType Pwm_GetOutputState(Pwm_ChannelType ChannelNumber)
{
    if (!Pwm_IsInitialized || ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels) return PWM_LOW;
    const Pwm_ChannelConfigType* ch = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    uint16_t enabled = 0;
    switch (ch->channel)
    {
        case 1: enabled = ch->TIMx->CCER & TIM_CCER_CC1E; break;
        case 2: enabled = ch->TIMx->CCER & TIM_CCER_CC2E; break;
        case 3: enabled = ch->TIMx->CCER & TIM_CCER_CC3E; break;
        case 4: enabled = ch->TIMx->CCER & TIM_CCER_CC4E; break;
        default: break;
    }

    return (enabled ? PWM_HIGH : PWM_LOW);
}

/**********************************************************
 * @brief   Tắt ngắt cho PWM channel
 **********************************************************/
void Pwm_DisableNotification(Pwm_ChannelType ChannelNumber)
{
    if (!Pwm_IsInitialized || ChannelNumber >= Pwm_CurrentConfigPtr->NumChannels) return;
    const Pwm_ChannelConfigType* ch = &Pwm_CurrentConfigPtr->Channels[ChannelNumber];

    switch (ch->channel)
    {
        case 1: TIM_ITConfig(ch->TIMx, TIM_IT_CC1, DISABLE); break;
        case 2: TIM_ITConfig(ch->TIMx, TIM_IT_CC2, DISABLE); break;
        case 3: TIM_ITConfig(ch->TIMx, TIM_IT_CC3, DISABLE); break;
        case 4: TIM_ITConfig(ch->TIMx, TIM_IT_CC4, DISABLE); break;
        default: break;
    }
}

/**
 * @brief Kích hoạt ngắt (notification) cho kênh PWM tương ứng
 *
 * @param channelId       Chỉ số của kênh PWM
 * @param notification    Loại cạnh dùng để kích hoạt ngắt (PWM_RISING_EDGE, PWM_FALLING_EDGE, hoặc cả hai)
 */
void Pwm_EnableNotification(uint8_t channelId, Pwm_EdgeNotificationType notification)
{
    // Kiểm tra nếu channelId vượt quá số kênh cho phép thì thoát
    if (channelId >= PWM_NUM_CHANNELS)
        return;

    // Trỏ đến cấu hình kênh PWM cụ thể
    Pwm_ChannelConfigType *cfg = (Pwm_ChannelConfigType *) &pwmChannelscfg[channelId];

    // Lấy bộ timer tương ứng với kênh này
    TIM_TypeDef *TIMx = cfg->TIMx;

    // Tạo bitmask của cờ Capture/Compare tương ứng với kênh (CC1, CC2, CC3, CC4)
    uint16_t cc_flag = TIM_IT_CC1 << (cfg->channel - 1);

    // Đánh dấu rằng kênh này đã bật notification
    cfg->notificationEnable = 1;

    // Nếu yêu cầu ngắt theo cạnh lên (rising edge)
    if (notification & PWM_RISING_EDGE)
    {
        TIM_ITConfig(TIMx, cc_flag, ENABLE); // Bật ngắt theo Capture Compare
    }

    // Nếu yêu cầu ngắt theo cạnh xuống (falling edge)
    if (notification & PWM_FALLING_EDGE)
    {
        TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE); // Bật ngắt theo sự kiện Update
    }

    // Xác định đúng IRQn tương ứng với timer đang dùng
    IRQn_Type irq =
        (TIMx == TIM2) ? TIM2_IRQn :
        (TIMx == TIM3) ? TIM3_IRQn :
        (TIMx == TIM4) ? TIM4_IRQn :
        (IRQn_Type)0xFF; // Không hỗ trợ các timer khác (ví dụ TIM1)

    // Nếu xác định được IRQ hợp lệ
    if (irq != (IRQn_Type)0xFF)
    {
        NVIC_InitTypeDef n;
        n.NVIC_IRQChannel = irq;                // Gán IRQ tương ứng
        n.NVIC_IRQChannelPreemptionPriority = 1;// Độ ưu tiên ngắt
        n.NVIC_IRQChannelSubPriority = 0;       // Sub-priority
        n.NVIC_IRQChannelCmd = ENABLE;          // Cho phép ngắt
        NVIC_Init(&n);                          // Khởi tạo NVIC
    }
}


/**********************************************************
 * @brief   Trả về thông tin phiên bản phần mềm của driver PWM
 **********************************************************/
void Pwm_GetVersionInfo(Std_VersionInfoType* versioninfo)
{
    if (versioninfo == NULL_PTR) return;

    versioninfo->vendorID = 0x1234;
    versioninfo->moduleID = 0xABCD;
    versioninfo->sw_major_version = 1;
    versioninfo->sw_minor_version = 0;
    versioninfo->sw_patch_version = 0;
}
