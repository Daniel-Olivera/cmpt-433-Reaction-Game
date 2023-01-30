/*
    Provides different functions for interacting with
    and controlling the LEDs
*/

#include "LedControls.h"
#include "../tools/tools.h"

#define TRIGGER_FILE_PATH "/sys/class/leds/beaglebone:green:usr%d/trigger"
#define TRIGGER_1 "/sys/class/leds/beaglebone:green:usr1/trigger"
#define TRIGGER_2 "/sys/class/leds/beaglebone:green:usr2/trigger"
#define TRIGGER_3 "/sys/class/leds/beaglebone:green:usr3/trigger"

#define BRIGHTNESS_FILE_PATH "/sys/class/leds/beaglebone:green:usr%d/brightness"
#define BRIGHTNESS_1 "/sys/class/leds/beaglebone:green:usr1/brightness"
#define BRIGHTNESS_2 "/sys/class/leds/beaglebone:green:usr2/brightness"
#define BRIGHTNESS_3 "/sys/class/leds/beaglebone:green:usr3/brightness"

#define NUM_LEDS 4

static FILE* triggerFiles[NUM_LEDS];
static FILE* brightnessFiles[NUM_LEDS];

static void LEDCONTROLS_open_files(char filePath[256], FILE* files[]);
static void LEDCONTROLS_close_files(FILE* files[]);

static void LEDCONTROLS_set_brightness(FILE*, char[]);
static void LEDCONTROLS_set_trigger(FILE* triggerFile, char mode[]);

void LEDCONTROLS_init(void)
{
    LEDCONTROLS_open_files(TRIGGER_FILE_PATH, triggerFiles);

    for(int i = 0; i < NUM_LEDS; i++){
        LEDCONTROLS_set_trigger(triggerFiles[i], "none");
    }
    
    LEDCONTROLS_close_files(triggerFiles);
}

void LEDCONTROLS_ready_msg(void)
{
    LEDCONTROLS_open_files(BRIGHTNESS_FILE_PATH, brightnessFiles);
    printf("Get ready...\n");

    LEDCONTROLS_set_brightness(brightnessFiles[0], "0");
    LEDCONTROLS_set_brightness(brightnessFiles[1], "1");
    LEDCONTROLS_set_brightness(brightnessFiles[2], "1");
    LEDCONTROLS_set_brightness(brightnessFiles[3], "0");

    LEDCONTROLS_close_files(brightnessFiles);
}

void LEDCONTROLS_show_direction(int direction)
{
    LEDCONTROLS_open_files(BRIGHTNESS_FILE_PATH, brightnessFiles);

    if (direction == UP){
        LEDCONTROLS_set_brightness(brightnessFiles[0], "1");
        LEDCONTROLS_set_brightness(brightnessFiles[1], "0");
        LEDCONTROLS_set_brightness(brightnessFiles[2], "0");
        LEDCONTROLS_set_brightness(brightnessFiles[3], "0");
    }
    else{
        LEDCONTROLS_set_brightness(brightnessFiles[0], "0");
        LEDCONTROLS_set_brightness(brightnessFiles[1], "0");
        LEDCONTROLS_set_brightness(brightnessFiles[2], "0");
        LEDCONTROLS_set_brightness(brightnessFiles[3], "1");
    }

    LEDCONTROLS_close_files(brightnessFiles);
}

static void LEDCONTROLS_open_files(char filePath[256], FILE* files[]){

    for(int i = 0; i < NUM_LEDS; i++){
        char fileName[256];
        sprintf(fileName, filePath, i); 
        files[i] = fopen(fileName, "w");

        if (files[i] == NULL) {
            printf("ERROR OPENING %s\n", fileName);
            exit(1);
        }
    }
}


static void LEDCONTROLS_close_files(FILE* files[])
{
    for(int i = 0; i < NUM_LEDS; i++){
        fclose(files[i]);
    }
}

void LEDCONTROLS_flash_leds(int hertz, int duration){

    /*  **magic number** 
        subtract 5ms from the sleep() because
        flashing the exact hertz for the exact duration
        was not producing the specified amount of flashes
    */
    int flashOffset = 5;
    int flashDuration = 1000 / hertz;
    long long startTime = getTimeInMs();

    while(1){

        LEDCONTROLS_open_files(BRIGHTNESS_FILE_PATH, brightnessFiles);
        for (int i = 0; i < NUM_LEDS; i++){
            LEDCONTROLS_set_brightness(brightnessFiles[i], "0");
        }
        LEDCONTROLS_close_files(brightnessFiles);
        

        sleepForMs(flashDuration-flashOffset);


        LEDCONTROLS_open_files(BRIGHTNESS_FILE_PATH, brightnessFiles);
        for (int i = 0; i < NUM_LEDS; i++){
            LEDCONTROLS_set_brightness(brightnessFiles[i], "1");
        }
        LEDCONTROLS_close_files(brightnessFiles);

        if ((getTimeInMs() - startTime) >= duration)
            break;
    }
    
}

static void LEDCONTROLS_set_brightness(FILE* led, char brightness[]){

    int charWritten = fprintf(led, brightness);
        if(charWritten <= 0){
            printf("ERROR WRITING DATA\n");
            exit(1);
        }
}

void LEDCONTROLS_reset_leds(void)
{
    LEDCONTROLS_open_files(TRIGGER_FILE_PATH, triggerFiles);

    LEDCONTROLS_set_trigger(triggerFiles[0], "heartbeat");
    LEDCONTROLS_set_trigger(triggerFiles[1], "mmc0");
    LEDCONTROLS_set_trigger(triggerFiles[2], "cpu0");
    LEDCONTROLS_set_trigger(triggerFiles[3], "mmc1");
    
    LEDCONTROLS_close_files(triggerFiles);
}

void LEDCONTROLS_set_trigger(FILE* triggerFile, char mode[])
{
    int charWritten = fprintf(triggerFile, mode);
    if(charWritten <= 0){
        printf("ERROR WRITING DATA\n");
        exit(1);
    }
}