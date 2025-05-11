#include <stdio.h>
#include "hardware/pio.h"

#include "ultrasonic_distance.h"
#include "ultrasonic_distance.pio.h"

//*************************************************************************************************

// Uses the HC-SR04 ultrasonic distance sensor which can run on 3.3V

static uint trigger_pin;
static uint echo_pin;

static PIO pio;
static uint sm;
static uint offset;

//*************************************************************************************************

bool ultrasonic_init_bit_bang(uint pin_base)
{
    trigger_pin = pin_base;
    echo_pin = pin_base + 1;

    // configure the output pin, default to low
    gpio_init(trigger_pin);
    gpio_set_dir(trigger_pin, true);
    gpio_put(trigger_pin, false);

    // configure the echo pin
    gpio_init(echo_pin);
    gpio_set_dir(echo_pin, false);
    gpio_disable_pulls(echo_pin);

    return true;
}

//*************************************************************************************************

void ultrasonic_start_measure_bit_bang()
{
    gpio_put(trigger_pin, false);
    sleep_ms(1000);

    gpio_put(trigger_pin, true);
    sleep_us(10);
    gpio_put(trigger_pin, false);
}

//*************************************************************************************************

float ultrasonic_get_distance_bit_bang()
{
    // while echo pin is low
    while (!gpio_get(echo_pin))
    {
        tight_loop_contents();
    }

    absolute_time_t time1 = get_absolute_time();
    printf("echo not low anymore\n");

    // while echo pin is high
    while (gpio_get(echo_pin))
    {
        tight_loop_contents();
    }

    absolute_time_t time2 = get_absolute_time();
    printf("echo not high anymore\n");

    int64_t diff_us = absolute_time_diff_us(time1, time2);

    return diff_us / 58.0f;
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

bool ultrasonic_distance_init_pio(uint pin_base)
{
    trigger_pin = pin_base;
    echo_pin = pin_base + 1;

    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ultrasonic_distance_program, &pio, &sm, &offset, pin_base, 2, true);
    if (!success)
    {
        return false;
    }

    gpio_disable_pulls(echo_pin);

    pio_gpio_init(pio, trigger_pin);
    pio_gpio_init(pio, echo_pin);
    
    pio_sm_set_consecutive_pindirs(pio, sm, trigger_pin, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, echo_pin, 1, false);

    pio_sm_config c = ultrasonic_distance_program_get_default_config(offset);
    
    sm_config_set_set_pins(&c, trigger_pin, 1);

    sm_config_set_jmp_pin(&c, echo_pin);
    sm_config_set_in_pin_base(&c, echo_pin); // the wait instruction uses the in pin base
    sm_config_set_in_pin_count(&c, 1);

    sm_config_set_clkdiv(&c, 1.0f);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_NONE);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    return true;
}

//*************************************************************************************************

void ultrasonic_start_measure_pio()
{
    pio_sm_put(pio, sm, ultrasonic_distance_TRIGGER_PULSE_CYCLES);
}

//*************************************************************************************************

float ultrasonic_get_distance_pio()
{
    uint32_t cycles = pio_sm_get_blocking(pio, sm);
    printf("Rx FIFO had %u (0x%x)\n", cycles, cycles);
    uint64_t ns = (0xFFFFFFFF - cycles) * 16 + 8;
    float us = ns / 1000.0f;
    float cm = us / 58.0f;
    return cm;
}
