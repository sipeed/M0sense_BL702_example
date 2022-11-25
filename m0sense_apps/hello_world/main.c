/* BL808 Driver*/
#include <bflb_platform.h>

int main(void)
{
    bflb_platform_init(0);
    MSG_DBG("Now can use printf or anything other like.\r\n");

    while (1) {
        printf("hello world.\r\n");
        mtimer_delay_ms(1000);
    }
}
