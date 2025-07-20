#include "Port_Cfg.h"

const Port_PinConfigType PortCfg_Pins[Pincount] = {
    {
        .PortID = 2, // port c
        .PinID = 45,// chân 13
        .PinMode = PORT_PIN_MODE_DIO,
        .Direction = PORT_PIN_OUT,
        .Speed = GPIO_Speed_2MHz,
        .Pull = PULL_UP,
        .Level = PORT_PIN_LEVEL_HIGH,
        .DirectionChangeable = 0,
        .ModeChangeable = 0
    },
        {
        .PortID = 1, // port B
        .PinID = 24,// chân 8
        .PinMode = PORT_PIN_MODE_DIO,
        .Direction = PORT_PIN_IN,
        .Speed = GPIO_Speed_2MHz,
        .Pull = PULL_UP,
        .Level = PORT_PIN_LEVEL_LOW,
        .DirectionChangeable = 0,
        .ModeChangeable = 0
    },
     /* PA0 - TIM2_CH1 - PWM Output */
    {
        .PortID = 0, // port A
        .PinID = 3,// chân 3
        .PinMode = PORT_PIN_MODE_PWM,
        .Direction = PORT_PIN_OUT,
        .Speed = GPIO_Speed_2MHz,
        .Pull = 0,
        .Level = 0,
        .DirectionChangeable = 0,
        .ModeChangeable = 0
    },
     /* PA6 - TIM3_CH1 - PWM Output */
    {
        .PortID = 0, // port A
        .PinID = 6,// chân 6
        .PinMode = PORT_PIN_MODE_PWM,
        .Direction = PORT_PIN_OUT,
        .Speed = GPIO_Speed_2MHz,
        .Pull = 0,
        .Level = 0,
        .DirectionChangeable = 0,
        .ModeChangeable = 0
    },
    // ...
};