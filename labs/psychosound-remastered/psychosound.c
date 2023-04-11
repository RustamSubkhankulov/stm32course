#include <stdint.h>

//---------------------------------------------------------

#include "seg7.h"
#include "button.h"
#include "inc/modregs.h"
#include "inc/gpio.h"
#include "inc/rcc.h"
#include "inc/systick.h"
#include "buzzer.h"

//=========================================================

#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
#define REF_FREQUENCY_DIV 8 // SysTick reference clock frequency: 6MHz

#define BLUE_LED_GPIOC_PIN  8U
#define GREEN_LED_GPIOC_PIN 9U

#define FREQ_STEP 1U
#define BUTTON_CHECK_RATE 50U

#define SYSTICK_PERIOD_US 10U
#define SYSTICK_FREQ (1000000U / SYSTICK_PERIOD_US)

//=========================================================

static void board_clocking_init(void);
static void gpio_init_led(void);
static int enable_psychosound(void);
static void systick_init(uint32_t period_us);

//=========================================================

static struct Buzzer Buzzer = { 0 };
static struct Button Up     = { 0 };
static struct Button Down   = { 0 };
static struct Seg7Display Seg7 = { 0 };

//=========================================================

int main(void)
{
    board_clocking_init();
    gpio_init_led();
    systick_init(SYSTICK_PERIOD_US);
    return enable_psychosound();
}

//---------------------------------------------------------

static void gpio_init_led(void)
{
    // (1) Enable GPIOC clocking:
    SET_BIT(REG_RCC_AHBENR, REG_RCC_AHBENR_IOPCEN);

    // (2) Configure PC9 mode:
    SET_GPIO_IOMODE(GPIOC, GREEN_LED_GPIOC_PIN, GPIO_IOMODE_GEN_PURPOSE_OUTPUT);

    // (3) Configure PC9 type - output push/pull 
    SET_GPIO_OTYPE(GPIOC, GREEN_LED_GPIOC_PIN, GPIO_OTYPE_PUSH_PULL);

    return;
}

//---------------------------------------------------------

static void board_clocking_init(void)
{
    // (1) Clock HSE and wait for oscillations to setup.
    SET_BIT(REG_RCC_CR, REG_RCC_CR_HSEON);
    while (CHECK_BIT(REG_RCC_CR, REG_RCC_CR_HSERDY))
        continue;

    // (2) Configure PLL:
    // PREDIV output: HSE/2 = 4 MHz
    SET_REG_RCC_CFGR2_PREDIV(2);

    // (3) Select PREDIV output as PLL input (4 MHz):
    SET_REG_RCC_CFGR_PLLSRC(REG_RCC_CFGR_PLLSRC_HSE_PREDIV);

    // (4) Set PLLMUL to 12:
    // SYSCLK frequency = 48 MHz
    SET_REG_RCC_CFGR_PLLMUL(12);

    // (5) Enable PLL:
    SET_BIT(REG_RCC_CR, REG_RCC_CR_PLLON);
    while(CHECK_BIT(REG_RCC_CR, REG_RCC_CR_PLLRDY))
        continue;

    // (6) Configure AHB frequency to 48 MHz:
    SET_REG_RCC_CFGR_HPRE_NOT_DIV();

    // (7) Select PLL as SYSCLK source:
    SET_REG_RCC_CFGR_SW(REG_RCC_CFGR_SW_PLL);

    while(GET_REG_RCC_CFGR_SWS() != REG_RCC_CFGR_SWS_PLL)
        continue;

    // (8) Set APB frequency to 24 MHz
    SET_REG_RCC_CFGR_PPRE(REG_RCC_CFGR_PPRE_DIV_2);
}

//---------------------------------------------------------

static void systick_init(uint32_t period_us)
{
    uint32_t systick_src_freq = CPU_FREQENCY;
    bool ref_freq_avail = false;

    if (!SYSTICK_GET_NOREF())
    {
        systick_src_freq /= REF_FREQUENCY_DIV;
        ref_freq_avail = true;
    }

    uint32_t reload_value = 0;

    if (!SYSTICK_GET_SKEW())
    {
        // TENMS value is exact

        /*
            NOTE: 
            The SysTick calibration value is set to 6000, which 
            gives a reference time base of 1 ms with
            the SysTick clock set to 6 MHz (max fHCLK/8).
        */

        uint32_t calib_value  = SYSTICK_GET_SKEW();
        reload_value = (ref_freq_avail)? calib_value * period_us :
                                         calib_value * period_us * REF_FREQUENCY_DIV;
    }
    else 
    {
        // 1 = TENMS value is inexact, or not given
        reload_value = period_us * (systick_src_freq / 1000000U);
    }

    // Program the reload value:
    // SYSTICK_SET_RELOAD((reload_value - 1U));
    *SYSTICK_RVR = (reload_value - 1U);

    // // Clear the current value:
    // SYSTICK_SET_CURRENT(0);
    *SYSTICK_CVR = 0;

    // // Program the CSR:

    if (ref_freq_avail == true)
        SYSTICK_SET_SRC_REF();
    else 
        SYSTICK_SET_SRC_CPU();

    SYSTICK_EXC_ENABLE();
    SYSTICK_ENABLE();
}

//---------------------------------------------------------

void systick_handler(void)
{
    static unsigned handler_ticks = 0U;
    handler_ticks += 1U;
    static bool led_is_on = false;

    button_update(&Up);
    button_update(&Down);

    if ((handler_ticks % SYSTICK_FREQ) == 0)
    {
        if (!led_is_on)
        {
            GPIO_BSRR_SET_PIN(GPIOC, GREEN_LED_GPIOC_PIN);
            led_is_on = true;
        }
        else 
        {
            GPIO_BRR_RESET_PIN(GPIOC, GREEN_LED_GPIOC_PIN);
            led_is_on = false;
        }
    }

    if ((handler_ticks % BUTTON_CHECK_RATE) == 0)
    {
        if (button_is_pressed(&Up))
        {
            if (Buzzer.freq < BUZZER_MAX_FREQ - FREQ_STEP)
                Buzzer.freq += FREQ_STEP;
        }

        if (button_is_pressed(&Down))
        {
            if (Buzzer.freq >= FREQ_STEP)
                Buzzer.freq -= FREQ_STEP;
        }
    }

    buzzer_do_routine(&Buzzer, handler_ticks);

    static unsigned iter = 0;

    if ((handler_ticks % 2) == 0)
    {   
        Seg7.number = Buzzer.freq;
        seg7_select_digit(&Seg7, (iter % 4));
        seg7_push_display_state_to_mc(&Seg7);

        iter += 1;
    }
}

//---------------------------------------------------------

static int enable_psychosound(void)
{
    int err = seg7_setup(&Seg7, GPIOA);
    if (err < 0) return err;

    unsigned freq = 0;

    err = buzzer_setup(&Buzzer, GPIOC, 10, freq, SYSTICK_FREQ);
    if (err < 0) return err;

    err = button_setup(&Up, GPIOB, 6);
    if (err < 0) return err;

    err = button_setup(&Down, GPIOD, 2);
    if (err < 0) return err;

    while (1)
        continue; // TODO WFI

    return 0;
}