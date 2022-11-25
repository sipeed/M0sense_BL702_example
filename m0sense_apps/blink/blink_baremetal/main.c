/* BL808 Driver*/
#include <bflb_platform.h>
#include <hal_gpio.h>

#include "io_def.h"

int main(void)
{
    bflb_platform_init(0);
    MSG_DBG("Now can use printf or anything other like.\r\n");

    gpio_set_mode(LED_B_PIN, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(LED_G_PIN, GPIO_OUTPUT_PP_MODE);
    gpio_set_mode(LED_R_PIN, GPIO_OUTPUT_PP_MODE);
    MSG_DBG("[init] goio set mode complete.\r\n");

    enum {
        BLACK = 0,
        BLUE = 0b1,
        GREEN = 0b10,
        RED = 0b100,
        WHITE = RED | GREEN | BLUE,
        COLOR_MASK = WHITE,
    };

    uint8_t led_state = BLACK;

    MSG_DBG("[main] start loop\r\n");
    while (1) {
        led_state = (led_state + 1) & COLOR_MASK;
        MSG_DBG("\r\nincrease 1 so led_state is %x\r\n", led_state);
        MSG_DBG("rgb(%u,%u,%u)\r\n", !!(led_state & RED), !!(led_state & GREEN), !!(led_state & BLUE));

        gpio_write(LED_B_PIN, !(led_state & BLUE));
        gpio_write(LED_G_PIN, !(led_state & GREEN));
        gpio_write(LED_R_PIN, !(led_state & RED));
        mtimer_delay_ms(10);
    }
}
