#include "pico/stdlib.h"

// the trigger and echo pins should be consecutive, trigger pin first then echo pin second
bool ultrasonic_distance_init(uint trigger_pin, uint echo_pin);

void ultrasonic_start_measure();
uint32_t ultrasonic_get_distance();
