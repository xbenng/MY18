#ifndef _PINS_H
#define _PINS_H

#define PIN_OLED_RS     3, 1
#define PIN_OLED_CS     3, 3

#define PIN_OLED_SCLK   1, 0
#define PIN_OLED_SDI    1, 1
#define PIN_OLED_SDO    1, 2 

#define IOCON_PIN_OLED_SCLK IOCON_PIO1_0
#define IOCON_PIN_OLED_SDI  IOCON_PIO1_1
#define IOCON_PIN_OLED_SDO  IOCON_PIO1_2

#define PIN_LED_HV      0, 8
#define PIN_LED_RTD     0, 9
#define PIN_LED_IMD     2, 10
#define PIN_LED_AMS     2, 2

#define PIN_BUTTON1     1, 3
#define PIN_BUTTON2     1, 5 

#define IOCON_BUTTON1 IOCON_PIO1_3
#define IOCON_BUTTON2 IOCON_PIO1_5

#endif
