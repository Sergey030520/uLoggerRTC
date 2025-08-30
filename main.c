#include <stdlib.h>
#include "rcc.h"
#include "led.h"
#include "usart.h"
#include "fpu.h"
#include "SD.h"
#include "IRQ.h"
#include "timer.h"
#include "log.h"
#include "memory_map.h"
#include "board_pins.h"
#include "RTC.h"
#include "button.h"

#define DEFAULT_TIMEOUT 1000

int init_rcc();
int init_board();
void init_uart();
int usart_adapter(const char *data, int length);

void print_clock_frequencies();

static void sw1_pressed_isr();
static void sw2_pressed_isr();
static void sw3_pressed_isr();
void init_buttons_callbacks();

static volatile uint8_t g_evt_set_datetime = 0;
static volatile uint8_t g_evt_set_time = 0;
static volatile uint8_t g_evt_set_date = 0;

void process_button_events()
{
    if (g_evt_set_datetime)
    {
        g_evt_set_datetime = 0;

        RTC_DateTime_Type datetime = {
            .date = {
                .day = 24,
                .month = 2,
                .week = MONDAY,
                .year = 2025,
            },
            .time = {.hour = 10, .minute = 50, .seconds = 0}};

        if (set_datetime_rtc(&datetime) == 0)
        {
            LOG_INFO("[BTN] Set full datetime -> %04d-%02d-%02d (w=%d) %02d:%02d:%02d",
                     datetime.date.year, datetime.date.month, datetime.date.day, datetime.date.week,
                     datetime.time.hour, datetime.time.minute, datetime.time.seconds);
            led_timed_blink(15, 1, 120);
        }
        else
        {
            LOG_INFO("[BTN] Failed to set datetime\r\n");
            led_timed_blink(13, 2, 120);
        }
    }

    if (g_evt_set_time)
    {
        g_evt_set_time = 0;

        RTC_Time_Type time = {.hour = 7, .minute = 20, .seconds = 0};
        if (set_time_rtc(&time) == 0)
        {
            LOG_INFO("[BTN] Set time -> %02d:%02d:%02d", time.hour, time.minute, time.seconds);
            led_timed_blink(15, 1, 120);
        }
        else
        {
            LOG_INFO("[BTN] Failed to set time\r\n");
            led_timed_blink(13, 2, 120);
        }
    }

    if (g_evt_set_date)
    {
        g_evt_set_date = 0;

        RTC_Date_Type date = {.year = 2025, .month = 8, .day = 15, .week = FRIDAY};
        if (set_date_rtc(&date) == 0)
        {
            LOG_INFO("[BTN] Set date -> %04d-%02d-%02d (w=%d)\r\n", date.year, date.month, date.day, date.week);
            led_timed_blink(15, 1, 120);
        }
        else
        {
            LOG_INFO("[BTN] Failed to set date\r\n");
            led_timed_blink(13, 2, 120);
        }
    }
}

int main()
{
    int status = init_board();
    RTC_DateTime_Type cur_datetime;
 
    if (status != 0)
    {
        ledOn(13, 1);
        goto error;
    }
    print_clock_frequencies();
    LOG_INFO("Драйверы запущены\r\n");

    init_buttons();
    init_buttons_callbacks();

    while (1)
    {
         if (get_datetime_rtc(&cur_datetime)) {
            LOG_INFO("RTC read error\r\n");
            goto error;
        }

        LOG_INFO("{{year:%d, month:%d, week:%d, day:%d}; {hour:%02d, minute:%02d, sec:%02d}}",
                 cur_datetime.date.year, cur_datetime.date.month, cur_datetime.date.week, cur_datetime.date.day,
                 cur_datetime.time.hour, cur_datetime.time.minute, cur_datetime.time.seconds);

        process_button_events();

        delay_timer(3000);
    }

error:
    while (1)
        ;
    return 0;
}

int init_rcc()
{
    RCC_PLLConfig pll_config = {0};
    RCC_BusConfig bus_config = {0};
    RCC_SystemConfig system_config = {0};
    RCC_PeripheralClockConfig peripheral_conf = {0};

    int status = 0;

    pll_config.PLLM = 25;
    pll_config.PLLN = 336;
    pll_config.PLLP = RCC_PLLP_DIV2;
    pll_config.PLLQ = 7;

    pll_config.source = RCC_HSE;

    bus_config.ahb_prescaler = RCC_HPRE_NOT_DIV;
    bus_config.apb1_prescaler = RCC_APB_DIV4;
    bus_config.apb2_prescaler = RCC_APB_DIV2;
    bus_config.rtc_prescaler = RCC_RTCPRE_DIV25;

    system_config.source = RCC_PLL;
    system_config.bus_config = &bus_config;
    system_config.pll_config = &pll_config;

    status = setup_system_config_rcc(&system_config);

    if (status != 0)
    {
        pll_config.PLLM = 16;
        pll_config.PLLN = 336;
        pll_config.PLLP = RCC_PLLP_DIV2;
        pll_config.PLLQ = 7;

        pll_config.source = RCC_HSI;

        bus_config.ahb_prescaler = RCC_HPRE_NOT_DIV;
        bus_config.apb1_prescaler = RCC_APB_DIV4;
        bus_config.apb2_prescaler = RCC_APB_DIV2;
        bus_config.rtc_prescaler = RCC_RTCPRE_DIV25;

        status = setup_system_config_rcc(&system_config);
        if (status != 0)
        {
            bus_config.ahb_prescaler = RCC_NOT_DIV;
            bus_config.apb1_prescaler = RCC_NOT_DIV;
            bus_config.apb2_prescaler = RCC_NOT_DIV;

            system_config.source = RCC_HSI;
            system_config.pll_config = NULL;
            status = setup_system_config_rcc(&system_config);
        }
    }

    peripheral_conf.APB1 = RCC_APB1ENR_TIM2EN | RCC_APB1ENR_PWREN;
    peripheral_conf.APB2 = RCC_APB2ENR_USART1EN | RCC_APB2ENR_SYSCFGEN;
    peripheral_conf.AHB1 = RCC_AHB1ENR_DMA2EN | RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_BKPSRAMEN;
    peripheral_conf.AHB2 = 0;
    peripheral_conf.AHB3 = 0;

    enable_gpio_clock_rcc(&peripheral_conf);
    return status;
}

int init_board()
{
    int status = 0;
    RCC_Frequencies rcc_clocks = {0};

    init_fpu();

    init_irq();

    status = init_rcc();

    init_led();

    if (status != 0)
    {
        ledOn(13, 1);
        return -1;
    }

    timer_init();

    init_uart();

    stm_init_log(usart_adapter);

    status = init_rtc();
    if (status != 0)
    {
        return -2;
    }

    return 0;
}

int usart_adapter(const char *data, int length)
{
    send_data_usart((uint8_t *)data, (uint16_t)length);
    return 0;
}

void init_uart()
{
    DMA_Config dma_tx_config = {
        .dma = DMA2,          // DMA2 для USART1_TX
        .stream = STREAM_7,   // Stream 7
        .channel = CHANNEL_4, // канал для USART1_TX
        .direction = DMA_DIR_MEM_TO_PERIPH,
        .mem_size = DMA_MSIZE_8BITS,
        .periph_size = DMA_PSIZE_8BITS,
        .inc_mem = 1,
        .inc_periph = 0,
        .circular = 0};

    GPIO_PinConfig_t tx_pin_config = {
        .gpiox = GPIOA_REG,
        .pin = USART1_TX_PIN,
        .mode = GPIO_MODER_ALTERNATE,
        .speed = GPIO_SPEED_100MHz,
        .pull = GPIO_PULL_NONE,
        .output = GPIO_OUTPUT_PUSHPULL,
        .af = GPIO_AF7,
    };
    GPIO_PinConfig_t rx_pin_config = {
        .gpiox = GPIOA_REG,
        .pin = USART1_RX_PIN,
        .mode = GPIO_MODER_ALTERNATE,
        .speed = GPIO_SPEED_100MHz,
        .pull = GPIO_PULL_NONE,
        .output = GPIO_OUTPUT_PUSHPULL,
        .af = GPIO_AF7,
    };

    // Основная конфигурация UART
    UART_Config_t uart1_config = {
        .usart = USART1_REG,
        .baud_rate = UART_BAUDRATE_115200,
        .tx_mode = UART_MODE_POLLING,
        .rx_mode = UART_MODE_POLLING,
        .dma_tx = dma_tx_config,   // DMA конфигурация для TX
        .dma_rx = {0},             // RX DMA не используется
        .tx_port = &tx_pin_config, // Пин TX
        .rx_port = &rx_pin_config  // Пин RX (можно NULL, если RX отключен)
    };

    RCC_Frequencies rcc_clocks = {0};

    get_clock_frequencies(&rcc_clocks);

    setup_uart(&uart1_config, rcc_clocks.APB2_Freq);
}

int sd_card_init()
{
    int status = init_sd();
    if (status != 0)
    {
        return -1;
    }

    status = init_sd_card();
    if (status != 0)
    {
        return -2;
    }
    return 0;
}

void print_clock_frequencies()
{
    RCC_Frequencies freq = {0};
    get_clock_frequencies(&freq);

    LOG_INFO("SYSCLK: %lu Hz", freq.SYSCLK_Freq);
    LOG_INFO("HCLK  : %lu Hz", freq.HCLK_Freq);
    LOG_INFO("APB1  : %lu Hz", freq.APB1_Freq);
    LOG_INFO("APB2  : %lu Hz", freq.APB2_Freq);
}

static void sw1_pressed_isr(void) { g_evt_set_datetime = 1; }
static void sw2_pressed_isr(void) { g_evt_set_time = 1; }
static void sw3_pressed_isr(void) { g_evt_set_date = 1; }

void init_buttons_callbacks(void)
{
    register_button_callback(BUTTON_SW1_PIN, sw1_pressed_isr);
    register_button_callback(BUTTON_SW2_PIN, sw2_pressed_isr);
    register_button_callback(BUTTON_SW3_PIN, sw3_pressed_isr);
}