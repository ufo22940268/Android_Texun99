#include <unistd.h>
#include <time.h>
#include "util.h"

extern int gameStatus;
extern long currentTime;

void*
viewTimer() {
    //When collision happens, just stop animation.
    //TODO Display a game report data. such as lasting time, reputation.
    if (isCollision()) {
        finishGame();
    } else {
        updateDots();
    }

    //Update current time.
    currentTime = time(NULL);

    //TODO It seems that the flush operation isn't necessary to make
    return NULL;
}

void*
dataTimer() {
    if (gameStatus == STATUS_NORMAL) {
        updateTimeData();
    }

    return NULL;
}

void*
bendAngleTimer() {
    if (gameStatus == STATUS_NORMAL) {
        bendAllDots();
    }

    return NULL;
}

void*
moveTimer() {
    if (gameStatus == STATUS_NORMAL) {
        movePlaneInDirection(getPlaneDirection());
    }

    usleep(MOVE_INTERVAL);
    moveTimer();
    return NULL;
}

void*
highFpsTimer() {
    if (isQuited()) {
        return NULL;
    }

    if (isRunning()) {
        lockNode();
        viewTimer();
        bendAngleTimer();
        unlockNode();
    }

    usleep(HIGH_FPS_INTERVAL);

    highFpsTimer();
    return NULL;
}

void*
lowFpsTimer() {
    if (isQuited()) {
        return NULL;
    }

    if (isRunning()) {
        dataTimer();
    }

    usleep(LOW_FPS_INTERVAL);

    lowFpsTimer();
    return NULL;
}
