#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <GLES2/gl2.h>
#include "util.h"

int global_seed = 0;

float toRadians(int degree) {
    return M_PI/180*degree;
}

int timeRand()
{
    srand(time(NULL) * global_seed);
    global_seed = (global_seed + 1)%100000;
    return rand();
}

float timeRandf()
{
    int ri = timeRand();
    return (float)(ri%100)/100;
}

double distant(double xa, double ya, double xb, double yb) 
{
    return sqrt(pow(xa - xb, 2) + pow(ya - yb, 2));
}

void printArray(GLfloat* array, int length) {
    int i;
    for (i = 0; i < length; i ++) {
        LOGD("index: %d\tvalue: %f", i, *array);
        array += 1;
        if (i%2 != 0) {
            LOGD("----------------------------------------------------------------------------------------------------");
        }
    }
}

bool isRunning() {
    return gameStatus == STATUS_NORMAL;
}

bool isQuited() {
    return gameStatus == STATUS_QUITED;
}

bool isFinished() {
    return gameStatus == STATUS_FINISHED;
}
