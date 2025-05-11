#include "pico/stdlib.h"

// the trigger and echo pins should be consecutive, trigger pin first then echo pin second
bool ultrasonic_distance_init_gpio(uint pin_base);
void ultrasonic_start_measure_gpio();
float ultrasonic_get_distance_gpio();

bool ultrasonic_distance_init_pio(uint pin_base);
void ultrasonic_start_measure_pio();
float ultrasonic_get_distance_pio();
