#include "Dio.h"
#include "Port.h"
#include "Det.h"
#include "Port_Cfg.h"
#include "timer.h"
#include "Pwm.h"
#include "Pwm_cfg.h"
// Tạo cấu hình Port tổng thể
const Port_ConfigType Port_Config = {
    .PinCfgType = PortCfg_Pins,
    .PortCfg_PinsCount = sizeof(PortCfg_Pins) / sizeof(PortCfg_Pins[0])
};

const Pwm_ConfigType PwmDriverConfig  = {
    .Channels    = pwmChannelscfg,
    .NumChannels =  sizeof(pwmChannelscfg) / sizeof(pwmChannelscfg[0])
};
int main(void)
{
    Port_Init(&Port_Config);  // Khởi tạo tất cả chân theo cấu hình
    Pwm_Init(&PwmDriverConfig);
    Delay_Init();        // Khởi tạo timer delay
    uint16_t dutyQ15 = 0;    // Bắt đầu từ 0% duty
    uint8_t dir = 1;         // Hướng tăng duty
    Pwm_SetDutyCycle(1,dutyQ15);
    // Pwm_SetDutyCycle(0, dutyQ15);
    while (1)
    {
        Delay_ms(10);  // Delay mượt sáng/tối

        if (dir)
        {
            dutyQ15 += 512;  // Tăng dần (~1.5%)
            if (dutyQ15 >= 32767) {
                dutyQ15 = 32767;
                dir = 0;
            }
        }
        else
        {
            if (dutyQ15 >= 512) dutyQ15 -= 512;
            else {
                dutyQ15 = 0;
                dir = 1;
            }
        }
    }

}

