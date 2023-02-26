#include <stdint.h>

//---------------------------------------------------------

#include "seg7.h"
#include "button.h"
#include "inc/modregs.h"
#include "inc/gpio.h"
#include "inc/rcc.h"
#include "inc/sleep.h"

//=========================================================

#define BLUE_LED_GPIOC_PIN  8U
#define GREEN_LED_GPIOC_PIN 9U

#define COOLDOWN_TICKS 10000U

//=========================================================

struct Player
{
    struct Button btn;

    uint32_t led_port;
    uint16_t led_pin;
    
    uint8_t score;

    bool prev;
    bool cur;

    bool led_is_on;
};

//=========================================================

static void board_clocking_init(void);
static void gpio_init_leds(void);
static int play_fingers_game(void);

static int player_setup(struct Player* player, uint32_t led_port, uint16_t led_pin, uint32_t btn_port, uint16_t btn_pin);
static void player_turn_off_led(struct Player* player);
static void player_turn_on_led(struct Player* player);
static void player_switch_led(struct Player* player);
static int player_poll_events(struct Player* player);

//=========================================================

int main(void)
{
    board_clocking_init();
    gpio_init_leds();
    return play_fingers_game();
}

//---------------------------------------------------------

static void gpio_init_leds(void)
{
    // (1) Enable GPIOC clocking:
    SET_BIT(REG_RCC_AHBENR, REG_RCC_AHBENR_IOPCEN);

    // (2) Configure PC8 & PC9 mode:
    SET_GPIO_IOMODE(GPIOC, GREEN_LED_GPIOC_PIN, GPIO_IOMODE_GEN_PURPOSE_OUTPUT);
    SET_GPIO_IOMODE(GPIOC, BLUE_LED_GPIOC_PIN, GPIO_IOMODE_GEN_PURPOSE_OUTPUT);

    // (3) Configure PC8 & PC9 type - output push/pull 
    SET_GPIO_OTYPE(GPIOC, GREEN_LED_GPIOC_PIN, GPIO_OTYPE_PUSH_PULL);
    SET_GPIO_OTYPE(GPIOC, BLUE_LED_GPIOC_PIN, GPIO_OTYPE_PUSH_PULL);

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

static int player_setup(struct Player* player, uint32_t led_port, uint16_t led_pin, uint32_t btn_port, uint16_t btn_pin)
{
    if (player == NULL)
        return -1;

    player->cur  = false;
    player->prev = false;
    player->score = 0;

    player->led_port = led_port;
    player->led_pin = led_pin;

    return button_setup(&(player->btn), btn_port, btn_pin);
}

//---------------------------------------------------------

static void player_switch_led(struct Player* player)
{
    if (player == NULL)
        return;

    if (player->led_is_on == true)
        player_turn_off_led(player);
    else 
        player_turn_on_led(player);

    return;
}

//---------------------------------------------------------

static void player_turn_off_led(struct Player* player)
{
    if (player == NULL)
        return;

    GPIO_BRR_RESET_PIN(player->led_port, player->led_pin);
    player->led_is_on = false;
}

//---------------------------------------------------------

static void player_turn_on_led(struct Player* player)
{
    if (player == NULL)
        return;

    GPIO_BSRR_SET_PIN(player->led_port, player->led_pin);
    player->led_is_on = true;
}

//---------------------------------------------------------

static int player_poll_events(struct Player* player)
{
    if (player == NULL)
        return -1;

    int err = button_update(&(player->btn));
    if (err < 0) return err;   

    player->cur = button_is_pressed(&(player->btn));
    return 0;
}

//---------------------------------------------------------

// TODO divide into functions

static int play_fingers_game(void)
{
    int err = 0;

    struct Seg7Display seg7 = { 0 };
    err = seg7_setup(&seg7, GPIOA);
    if (err < 0) return err;

    struct Player player1 = { 0 };
    err = player_setup(&player1, GPIOC, GREEN_LED_GPIOC_PIN, GPIOC, 10);
    if (err < 0) return err;

    struct Player player2  = { 0 };
    err = player_setup(&player1, GPIOC, BLUE_LED_GPIOC_PIN, GPIOC, 12);
    if (err < 0) return err;

    uint32_t tick = 0U;
    uint32_t cooldown = 0U;
    uint8_t winner = 0U;
    seg7.number = 0U;

    while (1)
    {
        // 1) Update buttons' state and update current players' actions

        if (cooldown == 0U)
        {
            err = player_poll_events(&player1);
            if (err < 0) return err;

            err = player_poll_events(&player2);
            if (err < 0) return err;

            // 2) decide who won

            if (player1.prev == true  && player1.cur == true 
            && player2.prev == false && player2.cur == true)
            {
                // 2nd player won

                winner = 2U;
                player2.score += 1;
                cooldown += COOLDOWN_TICKS;
            }
            else if (player1.prev == false && player1.cur == true
                && player2.prev == true  && player2.cur == true)
            {
                // 1st player won

                winner = 1U;
                player1.score += 1;
                cooldown += COOLDOWN_TICKS;
            }

            player1.prev = player1.cur;
            player2.prev = player2.cur;
        }
        else 
        {
            uint32_t rate1 = 0;
            uint32_t rate2 = 0;

            if (winner == 1U)
            {
                rate1 = 10; // High freq
                rate2 = 100; // Low freq
            }
            else // winner == 2U
            {
                rate1 = 100; // Low freq
                rate2 = 10; // High freq
            }

            if ((tick % rate1) == 0)
                player_switch_led(&player1);

            if ((tick % rate2) == 0)
                player_switch_led(&player2);

            cooldown -= 1;
            if (cooldown == 0)
            {
                player_turn_off_led(&player1);
                player_turn_off_led(&player2);
            }
        }

        // 3) Show current score
        // Seg7: NN:MM, where NN == player1.score and MM == player2.score
        seg7.number = (player1.score % 100) * 100 + (player2.score % 100);

        err = seg7_select_digit(&seg7, (tick % 4));
        if (err < 0) return err;

        err = seg7_push_display_state_to_mc(&seg7);

        to_get_more_accuracy_pay_2202_2013_2410_3805_1ms();
        tick += 1;
    }

    return 0;
}