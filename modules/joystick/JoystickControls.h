#include <stdio.h>
#include <stdlib.h>

enum JSDIRECTIONS {JSDIRECTIONS_UP,
                    JSDIRECTIONS_DOWN,
                    JSDIRECTIONS_LEFT,
                    JSDIRECTIONS_RIGHT,
                    JSDIRECTIONS_NUM_DIRECTIONS,
                    JSDIRECTIONS_NONE};

void JSCONTROLS_init(void);
int JSCONTROLS_getStickDirection(void);