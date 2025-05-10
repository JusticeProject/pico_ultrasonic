#include "hardware/pio.h"

#include "ultrasonic_distance.h"
#include "ultrasonic_distance.pio.h"

//*************************************************************************************************

// Uses the HC-SR04 ultrasonic distance sensor

static PIO pio;
static uint sm;
static uint offset;

//*************************************************************************************************

bool ultrasonic_distance_init(uint trigger_pin, uint echo_pin)
{
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ultrasonic_distance_program, &pio, &sm, &offset, trigger_pin, 2, true);
    if (!success)
    {
        return false;
    }

    // configure the output pin, default to low
    gpio_init(trigger_pin);
    gpio_set_dir(trigger_pin, true); // TODO: needed?
    gpio_put(trigger_pin, false);

    // configure the echo pin
    gpio_init(echo_pin);
    gpio_set_dir(echo_pin, false); // TODO: needed?
    gpio_disable_pulls(echo_pin);

    pio_gpio_init(pio, trigger_pin); // only needed for outputs

    pio_sm_set_consecutive_pindirs(pio, sm, trigger_pin, 1, true);
    pio_sm_set_consecutive_pindirs(pio, sm, echo_pin, 1, false);

    pio_sm_config c = ultrasonic_distance_program_get_default_config(offset);
    sm_config_set_set_pins(&c, trigger_pin, 1);
    sm_config_set_jmp_pin(&c, echo_pin);
    sm_config_set_clkdiv(&c, 125.0f);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_NONE);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    return true;
}

//*************************************************************************************************

void ultrasonic_start_measure()
{
    pio_sm_put(pio, sm, ultrasonic_distance_TRIGGER_PULSE_CYCLES);
}

//*************************************************************************************************

uint32_t ultrasonic_get_distance()
{
    uint32_t cycles = pio_sm_get_blocking(pio, sm);
    //return 0xFFFFFFFF - cycles;
    return cycles;
}
