#include <stdio.h>
#include <stdlib.h>

enum upOrDown {UP, DOWN};

void LEDCONTROLS_init(void);

void LEDCONTROLS_ready_msg(void);
void LEDCONTROLS_show_direction(int);

void LEDCONTROLS_flash_leds(int, int);
void LEDCONTROLS_reset_leds(void);


