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

float ultrasonic_get_distance_bit_bang()
{
    // keep trigger low for a short time to get ready for the next measurement
    gpio_put(trigger_pin, false);
    sleep_ms(100);

    // pulse the trigger pin high for 10us
    gpio_put(trigger_pin, true);
    sleep_us(10);
    gpio_put(trigger_pin, false);

    // while echo pin is low, we are waiting for echo pin to go high
    while (!gpio_get(echo_pin))
    {
        tight_loop_contents();
    }

    // echo pin is now high, grab the time
    absolute_time_t time1 = get_absolute_time();
    //printf("echo not low anymore\n");

    // while echo pin is high, we are waiting for echo pin to go low
    while (gpio_get(echo_pin))
    {
        tight_loop_contents();
    }

    // echo pin is now low, grab the time
    absolute_time_t time2 = get_absolute_time();
    //printf("echo not high anymore\n");

    int64_t diff_us = absolute_time_diff_us(time1, time2);

    // The speed of sound is about 343 m/s.
    // (343 m/s) * (100cm / 1m) * (1s / 1,000,000us) = 0.0343 cm/us
    // diff_us * 0.0343 cm/us gives the total roundtrip distance traveled in cm.
    // We want half of that distance.
    // Thus, diff_us * 0.0343 / 2 gives the distance in cm.
    return diff_us * 0.01715;
}

//*************************************************************************************************
//*************************************************************************************************
//*************************************************************************************************

bool ultrasonic_distance_init_pio(uint pin_base)
{
    // trigger and echo GPIOs should be consecutive
    trigger_pin = pin_base;
    echo_pin = pin_base + 1;

    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ultrasonic_distance_program, &pio, &sm, &offset, pin_base, 2, true);
    if (!success)
    {
        return false;
    }

    // don't want pull-up or pull-down on the input, datasheet for sensor says it is TTL
    gpio_disable_pulls(echo_pin);

    pio_gpio_init(pio, trigger_pin);
    pio_gpio_init(pio, echo_pin);
    
    pio_sm_set_consecutive_pindirs(pio, sm, trigger_pin, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, echo_pin, 1, false);

    pio_sm_config c = ultrasonic_distance_program_get_default_config(offset);
    
    // we use the set assembly instruction on the trigger pin
    sm_config_set_set_pins(&c, trigger_pin, 1);

    // we use the jmp and wait instructions on the echo pin
    sm_config_set_jmp_pin(&c, echo_pin);
    sm_config_set_in_pin_base(&c, echo_pin); // the wait instruction uses the in pin base
    sm_config_set_in_pin_count(&c, 1);

    // no clock divider, so one instruction is 8ns (125MHz)
    sm_config_set_clkdiv(&c, 1.0f);

    // keep size of 4 for each Rx and Tx buffers
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_NONE);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    return true;
}

//*************************************************************************************************

void ultrasonic_start_measure_pio()
{
    // initialize the loop counter that the PIO uses, this is defined in the .pio file
    pio_sm_put(pio, sm, ultrasonic_distance_TRIGGER_PULSE_CYCLES);
}

//*************************************************************************************************

float ultrasonic_get_distance_pio()
{
    uint32_t loop_counter = pio_sm_get_blocking(pio, sm);
    printf("Rx FIFO had %u (0x%x)\n", loop_counter, loop_counter);

    // The PIO counted down from 0xFFFFFFFF while it was looking at the echo pulse.
    // The difference between 0xFFFFFFFF and loop_counter is the number of times it ran
    // through the loop. Each loop is 2 assembly instructions (each instruction is 8ns).
    // We will add an additional 1 instruction (8ns) for the wait instruction before the loop.
    uint64_t ns = (0xFFFFFFFF - loop_counter) * 16 + 8;
    float us = ns / 1000.0f;

    // See notes above in the Bit Bang API for the conversion formula.
    float cm = us * 0.01715;
    return cm;
}
