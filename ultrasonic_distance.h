#include "pico/stdlib.h"

// If you want to switch between the two APIs you will need to recompile since there is no 
// deinit in the API yet. You can't switch between them on the fly.

// Bit-bang API:
// the trigger and echo pins should be consecutive, trigger pin first then echo pin second
bool ultrasonic_init_bit_bang(uint pin_base);
void ultrasonic_start_measure_bit_bang();
float ultrasonic_get_distance_bit_bang();

// PIO API:
// the trigger and echo pins should be consecutive, trigger pin first then echo pin second
bool ultrasonic_distance_init_pio(uint pin_base);
void ultrasonic_start_measure_pio();
float ultrasonic_get_distance_pio();
