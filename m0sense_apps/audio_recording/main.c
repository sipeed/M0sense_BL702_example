/* BL702 Driver*/
#include <bflb_platform.h>
#include <bl702_glb.h>
#include <hal_adc.h>
#include <hal_dma.h>
#include <hal_gpio.h>
/* USB STDIO */
#include <usb_stdio.h>

#include "io_def.h"

#define SAMPLE_COUNT (2 * 512)
static adc_data_parse_t data_parse;
static bool pingpong_idx = false;
static int16_t raw_adc_buffer[2][SAMPLE_COUNT * (sizeof(uint32_t) / sizeof(int16_t))];

void dma_ch0_irq_callback(struct device* dev, void* args, uint32_t size, uint32_t state)
{
    int16_t* adc_buffer = raw_adc_buffer[pingpong_idx];
    pingpong_idx = !pingpong_idx;
    device_read(device_find("adc"), 0, raw_adc_buffer[pingpong_idx],
                sizeof(raw_adc_buffer[0]) / sizeof(uint8_t)); /* size need convert to uint8_t*/

    /* min: 6339, max: 9165 mid: 7752 */
    uint32_t res_all = 0;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        adc_buffer[i] = (((uint32_t*)adc_buffer)[i] & 0xffff) >> 2;
        res_all += (uint32_t)adc_buffer[i];
    }

    for (int i = 0; i < SAMPLE_COUNT; i++) {
        adc_buffer[i] -= res_all / SAMPLE_COUNT;
        adc_buffer[i] <<= 6;
        printf("%5d", adc_buffer[i]);
        printf(i % 16 == 15 ? "\r\n" : ", ");
    }
}

int main(void)
{
    bflb_platform_init(0);
    MSG_DBG(
        "Now can use MSG_xxx, LOG_xxx and bflb_platform_printf on uart.\r\n");  // just appear on uart unless use printf

#ifdef M0SENSE_USE_USBSTDIO
    usb_stdio_init();                                        // MUST be called before any call to printf or puts
    printf("Now can use printf, puts on usb_cdc_acm.\r\n");  // on usb, ttyACMx on Linux or COMx on Windows.
#endif

    { /* configure the pll for ADC */
        uint32_t tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
        tmpVal &= (~(1 << BL_AHB_SLAVE1_GPIP));
        BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

        /* ROOT_CLOCK_SOURCE_AUPLL_24576000_HZ / 24 = 1024000 HZ */
        PDS_Set_Audio_PLL_Freq(AUDIO_PLL_24576000_HZ);
        GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_AUDIO_PLL, 23);

        tmpVal |= (1 << BL_AHB_SLAVE1_GPIP);
        BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);
    }

    GLB_GPIO_Type gpios[] = {MIC_IN_PIN};
    GLB_GPIO_Func_Init(GPIO_FUN_ANALOG, gpios, sizeof(gpios) / sizeof(GLB_GPIO_Type));
    printf("[init] goio set mode complete.\r\n");
    /* initialize ADC0 */
    adc_register(ADC0_INDEX, "adc");
    struct device_t* mic_adc = device_find("adc");
    if (!mic_adc) {
        printf("[init] ADC initial failed!\r\n");
        goto _exit;
    }
    /* 1024000 HZ / 4 = 256000 HZ */
    ADC_DEV(mic_adc)->clk_div = ADC_CLOCK_DIV_4;
    /* 256000 HZ / 16 = 16000 HZ (16k) */
    ADC_DEV(mic_adc)->data_width = ADC_DATA_WIDTH_14B_WITH_16_AVERAGE;
    ADC_DEV(mic_adc)->continuous_conv_mode = ENABLE;
    ADC_DEV(mic_adc)->vref = ADC_VREF_2V;
    ADC_DEV(mic_adc)->fifo_threshold = ADC_FIFO_THRESHOLD_16BYTE;
    ADC_DEV(mic_adc)->gain = ADC_GAIN_2;
    device_open(mic_adc, DEVICE_OFLAG_DMA_RX);

    dma_register(DMA0_CH0_INDEX, "dma_ch0");
    struct device* dma_ch0 = device_find("dma_ch0");
    if (!dma_ch0) {
        printf("[init] DMA ch0 for ADC initial failed!\r\n");
        goto _exit;
    }
    DMA_DEV(dma_ch0)->direction = DMA_PERIPH_TO_MEMORY;
    DMA_DEV(dma_ch0)->transfer_mode = DMA_LLI_ONCE_MODE;
    DMA_DEV(dma_ch0)->src_req = DMA_REQUEST_ADC0;
    DMA_DEV(dma_ch0)->dst_req = DMA_REQUEST_NONE;
    DMA_DEV(dma_ch0)->src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    DMA_DEV(dma_ch0)->dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    DMA_DEV(dma_ch0)->src_burst_size = DMA_BURST_INCR1;
    DMA_DEV(dma_ch0)->dst_burst_size = DMA_BURST_INCR1;
    DMA_DEV(dma_ch0)->src_width = DMA_TRANSFER_WIDTH_32BIT;
    DMA_DEV(dma_ch0)->dst_width = DMA_TRANSFER_WIDTH_32BIT;
    device_open(dma_ch0, 0);
    device_set_callback(dma_ch0, dma_ch0_irq_callback);
    device_control(dma_ch0, DEVICE_CTRL_SET_INT, NULL);

    /* connect dac device and dma device */
    device_control(mic_adc, DEVICE_CTRL_ATTACH_RX_DMA, dma_ch0);
    printf("[init] ADC initial success!\r\n");

    adc_channel_t posChList[] = {ADC_CHANNEL2};
    adc_channel_t negChList[] = {ADC_CHANNEL_GND};
    adc_channel_cfg_t adc_channel_cfg = {
        .pos_channel = posChList,
        .neg_channel = negChList,
        .num = sizeof(posChList) / sizeof(posChList[0]),
    };
    while (adc_channel_config(mic_adc, &adc_channel_cfg) != SUCCESS) {
        printf("[micrec] ADC channel config error\r\n");
        mtimer_delay_ms(1000);
    }
    printf("[micrec] start success\r\n");

    adc_channel_start(mic_adc);
    device_read(mic_adc, 0, raw_adc_buffer[pingpong_idx],
                sizeof(raw_adc_buffer[0]) / sizeof(uint8_t)); /* size need convert to uint8_t*/
    while (1) {
        __WFI();
        mtimer_delay_ms(100);
    }
    adc_channel_stop(mic_adc);

_exit:
    while (1) {
        __WFI();
    }
}
