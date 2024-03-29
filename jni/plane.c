#include <stdlib.h>
#include "util.h"

extern int gameStatus;

float planeX = 0;
float planeY = 0;

//Deprecated.
void drawPlane() {
    translate(planeX, planeY);
    setColor(gColorHandler, 0x66ccff);
    drawCircle();
}

//TODO
void createPlanePositionArray(GLfloat *pos) {
    GLfloat x = planeX;
    GLfloat y = planeY;
    pos[0] = x - PLANE_SIZE; pos[1] = y + PLANE_SIZE;
    pos[2] = x + PLANE_SIZE; pos[3] = y + PLANE_SIZE;
    pos[4] = x - PLANE_SIZE; pos[5] = y - PLANE_SIZE;
    pos[6] = x + PLANE_SIZE; pos[7] = y - PLANE_SIZE;
}

void drawTrollFace() {
    GLfloat color[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat pos[16];
    createPlanePositionArray(pos);
    drawTexture(pos, gTrollTexCoords, color);
}

bool isMoveIllegal(int x, int y) {
    if (abs(planeX + x) > 100 || abs(planeY + y) > 100) {
        return true;
    } else {
        return false;
    }
}

void movePlane(int x, int y)
{
    if (isMoveIllegal(x, y)) {
        return;
    }

    planeX += x;
    planeY += y;
}

void movePlaneInDirection(int direction)
{
    if (gameStatus == STATUS_FINISHED) {
        return;
    }

    switch (direction) {
	case DIRECTION_UP:
	    movePlane(0, MOVE_SIZE);
	    break;
	case DIRECTION_DOWN:
	    movePlane(0, -MOVE_SIZE);
	    break;
	case DIRECTION_LEFT:
	    movePlane(-MOVE_SIZE, 0);
	    break;
	case DIRECTION_RIGHT:
	    movePlane(MOVE_SIZE, 0);
	    break;
	case DIRECTION_LEFT_UP:
	    movePlane(-MOVE_SIZE, MOVE_SIZE);
	    break;
	case DIRECTION_RIGHT_UP:
	    movePlane(MOVE_SIZE, MOVE_SIZE);
	    break;
	case DIRECTION_LEFT_DOWN:
	    movePlane(-MOVE_SIZE, -MOVE_SIZE);
	    break;
	case DIRECTION_RIGHT_DOWN:
	    movePlane(MOVE_SIZE, -MOVE_SIZE);
	    break;
    }
}
