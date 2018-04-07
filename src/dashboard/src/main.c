#include "board.h"
#include "config.h"
#include "NHD_US2066.h"
#include "CANlib.h"

#include "led.h"
#include "dispatch.h"

#include <string.h>
#include <math.h>

int main(void) {
    Board_Chip_Init();
    Board_GPIO_Init();
    // need a delay after init or the display
    // will sometimes be messed up
    Delay(100);

    init_can0_dash();

    Pin_Write(PIN_OLED_CS, 0);
    Pin_Write(PIN_OLED_RS, 1);
    Pin_Write(PIN_OLED_SCLK, 1);
    Pin_Write(PIN_OLED_SDI, 1);

    LED_HV_off();
    LED_RTD_off();
    LED_IMD_off();
    LED_AMS_off();

    dispatch_init();

    while (1) {
        dispatch_update();
    }

    return 0;
}