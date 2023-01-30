/*
    Main loop of the program
    Where the game is played and controlled
*/

#include <stdio.h>
#include <stdlib.h>

#include "modules/leds/LedControls.h"
#include "modules/tools/tools.h"
#include "modules/joystick/JoystickControls.h"

int recordInMs = 10000;
enum code {noInput, selectedToQuit};

static void closeGame(int);

int main(int argc, char* args[])
{
    printf("Hello embedded world, from Daniel!\n");
    printf("When the LEDs light up, press the joystick in that direction!\n");
    printf("(Press left or right to exit)\n");

    LEDCONTROLS_init();
    JSCONTROLS_init();

    while(1){
        LEDCONTROLS_ready_msg();
        long long randomTime = getRandomTimeInMs();

        sleepForMs(randomTime);

        if(JSCONTROLS_getStickDirection() == JSDIRECTIONS_UP ||
            JSCONTROLS_getStickDirection() == JSDIRECTIONS_DOWN){
            printf("Too soon!\n");
            sleepForMs(1000);
            continue;
        }

        if(JSCONTROLS_getStickDirection() == JSDIRECTIONS_LEFT ||
            JSCONTROLS_getStickDirection() == JSDIRECTIONS_RIGHT){
            closeGame(selectedToQuit);
        }

        int direction = getRandomZeroOrOne();

        switch(direction){
            case UP:
                printf("Press UP now!\n");
                break;
            case DOWN:
                printf("Press DOWN now!\n");
                break;
        }

        LEDCONTROLS_show_direction(direction);

        long long startTime = getTimeInMs();
        while (1){
            int input = JSCONTROLS_getStickDirection();

            if (input == direction){
                printf("Correct!\n");
                LEDCONTROLS_flash_leds(4, 500);

                int reactionTime = getTimeInMs() - startTime;
                if (reactionTime < recordInMs){
                    printf("New best time!\n");
                    recordInMs = reactionTime;
                }
                printf("Your reaction time was %dms; best so far in game is %dms.\n", reactionTime, recordInMs);
                break;
            }
            else if (input == JSDIRECTIONS_LEFT || input == JSDIRECTIONS_RIGHT){
                closeGame(selectedToQuit);
            }
            else if (getTimeInMs() - startTime == 5000 && input == JSDIRECTIONS_NONE){
                closeGame(noInput);
            }
            else if (input != direction && input != JSDIRECTIONS_NONE){
                printf("Incorrect.\n");
                LEDCONTROLS_flash_leds(10, 1000);
                break;
            }
        }

        sleepForMs(1000);
    }

    LEDCONTROLS_reset_leds();

    return 0;
}

static void closeGame(int code)
{
    LEDCONTROLS_reset_leds();

    if(code){
        printf("User selected to quit.\n");
        exit(0);  
    }
    else{
        printf("No input within 5000ms; quitting!\n");
        exit(0);
    }
}