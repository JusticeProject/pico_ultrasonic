#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"

#include "ultrasonic_distance.h"

//************************************************************************************************************

// We need two GPIO, so we are using GPIO 2 and 3. PIO likes consecutive GPIOs.
// GPIO 2 = trigger
// GPIO 3 = echo
#define ULTRASONIC_GPIO_PIN_BASE 2

//************************************************************************************************************

int main() {
    stdio_init_all();

    //ultrasonic_init_bit_bang(ULTRASONIC_GPIO_PIN_BASE);
    ultrasonic_distance_init_pio(ULTRASONIC_GPIO_PIN_BASE);
    
    while (true)
    {
        //int c = getchar_timeout_us(0);
        int c = getchar();
        printf("you entered %c\n", c);

        if ('h' == c)
        {
            // TODO: update when new commands are added
            printf("\n\n");
            printf("running on %s\n", PICO_BOARD);
            printf("q = query distance\n");
        }
        else if ('q' == c)
        {
            printf("starting to query for distance\n");

            //ultrasonic_start_measure_gpio();
            //float value = ultrasonic_get_distance_gpio();
            //printf("result = %f\n", value);

            ultrasonic_start_measure_pio();
            float value = ultrasonic_get_distance_pio();
            printf("result = %f\n", value);
        }
        else
        {
            printf("unknown cmd %c\n", c);
        }
    }
}
