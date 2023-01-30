/*
    Initializes the joystick as well as provides all of the
    functionalities needed to get joystick inputs from the user
*/

#include "JoystickControls.h"
#include <string.h>

#define JSUP_VAL "/sys/class/gpio/gpio26/value"
#define JSRT_VAL "/sys/class/gpio/gpio47/value"
#define JSDN_VAL "/sys/class/gpio/gpio46/value"
#define JSLFT_VAL "/sys/class/gpio/gpio65/value"

#define JSUP_D "/sys/class/gpio/gpio26/direction"
#define JSRT_D "/sys/class/gpio/gpio47/direction"
#define JSDN_D "/sys/class/gpio/gpio46/direction"
#define JSLFT_D "/sys/class/gpio/gpio65/direction"

static FILE* jsDirectionFiles[JSDIRECTIONS_NUM_DIRECTIONS];

int currentDirection = JSDIRECTIONS_NONE;

static void JSCONTROLS_open_direction_files(char*);
static void JSCONTROLS_close_direction_files(void);
static void JSCONTROLS_readStickDirection(void);
static void runCommand(char* command);

void JSCONTROLS_init(void)
{
    runCommand("config-pin p8.14 gpio"); //joystick up pin
    runCommand("config-pin p8.15 gpio"); //joystick right pin
    runCommand("config-pin p8.16 gpio"); //joystick down pin
    runCommand("config-pin p8.18 gpio"); //joystick left pin

    JSCONTROLS_open_direction_files("w");

    for(int i = 0; i < JSDIRECTIONS_NUM_DIRECTIONS; i++){
        int charWritten = fprintf(jsDirectionFiles[i], "%s", "in");
        if(charWritten <= 0){
            printf("ERROR SETTING INPUT DIRECTION");
        }
    }
    
    JSCONTROLS_close_direction_files();

}

static void JSCONTROLS_open_direction_files(char* mode)
{
    jsDirectionFiles[0] = fopen(JSUP_D, mode);
    jsDirectionFiles[1] = fopen(JSRT_D, mode);
    jsDirectionFiles[2] = fopen(JSDN_D, mode);
    jsDirectionFiles[3] = fopen(JSLFT_D, mode);

    for(int i = 0; i < JSDIRECTIONS_NUM_DIRECTIONS; i++){
        if (jsDirectionFiles[i] == NULL){
            printf("ERROR: Unable to open direction file: %d", i);
        }
    }
}

static void JSCONTROLS_close_direction_files(void)
{
    for(int i = 0; i < JSDIRECTIONS_NUM_DIRECTIONS; i++){
        fclose(jsDirectionFiles[i]);
    }
}

int JSCONTROLS_getStickDirection(void)
{
    JSCONTROLS_readStickDirection();
    return currentDirection;
}

static void JSCONTROLS_readStickDirection(void)
{
    JSCONTROLS_open_direction_files("r");

    const int MAX_LENGTH = 1024;
    char JSBuff_UP[MAX_LENGTH];
    char JSBuff_RT[MAX_LENGTH];
    char JSBuff_DN[MAX_LENGTH];
    char JSBuff_LFT[MAX_LENGTH];

    fgets(JSBuff_UP, MAX_LENGTH, jsDirectionFiles[0]);
    fgets(JSBuff_RT, MAX_LENGTH, jsDirectionFiles[1]);
    fgets(JSBuff_DN, MAX_LENGTH, jsDirectionFiles[2]);
    fgets(JSBuff_LFT, MAX_LENGTH, jsDirectionFiles[3]);

    JSCONTROLS_close_direction_files();

    if(strcmp(JSBuff_UP, "0\n") == 0){
        currentDirection = JSDIRECTIONS_UP;
    }
    else if(strcmp(JSBuff_RT, "0\n") == 0){
        currentDirection = JSDIRECTIONS_RIGHT;
    }
    else if(strcmp(JSBuff_DN, "0\n") == 0){
        currentDirection = JSDIRECTIONS_DOWN;
    }
    else if(strcmp(JSBuff_LFT, "0\n") == 0){
        currentDirection = JSDIRECTIONS_LEFT;
    }
    else {
        currentDirection = JSDIRECTIONS_NONE;
    }
}

static void runCommand(char* command)
{
    FILE *pipe = popen(command, "r");

    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
    }

    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0){
        perror("Unable to execute command: ");
        printf("    %s\n", command);
        printf("    exit code: %d\n", exitCode);
    }
}

