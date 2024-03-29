#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "util.h"

const int PROFILE = 1;

void chooseEdge(int *x, int *y)
{
    extern int global_seed;

    int switcher[4][2] = {{1, 0}, {0, 1}, {1, 0}, {0, 1}};
    int base[4][2] = {{-100, -100}, {100, -100}, {-100, 100}, {-100, -100}};

    //Segment denote every edge of rectangle.
    int segmentIndex = global_seed/4%4;
    *x = base[segmentIndex][0] + switcher[segmentIndex][0]*200*timeRandf();
    *y = base[segmentIndex][1] + switcher[segmentIndex][1]*200*timeRandf();
}

bool isRightAngle(int x, int y, float angle) 
{
    return pow(x, 2) + pow(y, 2) >= pow(x + cos(angle), 2) + pow(y + sin(angle), 2);
}

dot* createDotFromEdge()
{
    dot* d = (dot*)malloc(sizeof(dot));
    int x, y;
    
    //Choose position.
    chooseEdge(&x, &y);

    //Choose vector.
    float angle = timeRand()%20;
    if (!isRightAngle(x, y, angle)) {
        angle += M_PI;
    }

    d->x = x;
    d->y = y;
    d->angle = angle;
    return d;
}

void deleteDots() {
    linked_node *cur = getHeaderNode();
    while (cur) {
        dot *d = cur->dot;
        if (abs(d->x) > sVirtualWidth || abs(d->y) > sVirtualHeight) {
            deleteDot(d);
            cur = getHeaderNode();
        } else {
            cur = cur->next;
        }
    }
}

void updatePosition()
{
    linked_node *cur = getHeaderNode();
    while (cur) {
        dot *d = cur->dot;

        int realSpeed = speed + (dotStatusSpeedUp == true ? 2 : 0);
        if (d == NULL) {
            LOGD("nullll||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||");
            return;
        }
        d->x += realSpeed*cos(d->angle);
        d->y += realSpeed*sin(d->angle);
        cur = cur->next;
    }

    //TODO Not very sure whether this is to delete.
    /*deleteDots();*/
}

void createDotPos(dot *d, GLfloat* pos) {
    float x = d->x;
    float y = d->y;
    float size = 5;
    pos[0] = x - size; pos[1] = y + size;
    pos[2] = x + size; pos[3] = y + size;
    pos[4] = x - size; pos[5] = y - size;
    pos[6] = x + size; pos[7] = y - size;
}

void drawShitDot(dot *d) {
    GLuint posHandler = glGetAttribLocation(gTexProgram, "a_Position");
    GLuint texCoordHandler = glGetAttribLocation(gTexProgram, "a_TexCoord");

    GLuint mvpHandler = glGetUniformLocation(gTexProgram, "u_MVPMatrix");
    GLuint samplerHandler = glGetUniformLocation(gTexProgram, "u_sampler");
    GLuint colorHandler = glGetUniformLocation(gTexProgram, "u_color");

    loadScreenProjection(mvpHandler);

    GLfloat pos[4*2];
    createDotPos(d, pos);
    glVertexAttribPointer(posHandler, 2, GL_FLOAT, GL_FALSE, 0, pos);
    glEnableVertexAttribArray(posHandler);

    glUniform1i(samplerHandler, 0);
    glVertexAttribPointer(texCoordHandler, 2, GL_FLOAT, GL_FALSE, 0, gShitTexCoords);
    glEnableVertexAttribArray(texCoordHandler);

    setDotColor(colorHandler);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    checkGlError("dot");
}

void drawDot(dot *d)
{
    //*The void color of dot is 0xfffffff*
    /*setDotColor();*/

    //Ensure the projection matrix is the default value.
    loadIdentity(gViewProjectionHandler);

    GLfloat size[] = {DOT_SIZE};
    glEnableVertexAttribArray(gSizeHandler);
    glEnableVertexAttribArray(gPosHandler);

    glVertexAttribPointer(gSizeHandler, 1, GL_FLOAT, GL_FALSE, 0, size);

    GLfloat pointer[] = {(GLfloat)d->x, (GLfloat)d->y};
    glVertexAttribPointer(gPosHandler, 2, GL_FLOAT, GL_FALSE, 0, pointer);
    glDrawArrays(GL_POINTS, 0, 1);

    glDisableVertexAttribArray(gPosHandler);
    glDisableVertexAttribArray(gSizeHandler);
}

void setDotColor(GLuint handler)
{
    if (flyStatus == (FLY_BEND | FLY_SPEED_UP)) {
        setColor(handler, COLOR_BEND_SPEED_UP);
    } else if (flyStatus == FLY_BEND) {
        setColor(handler, COLOR_BEND);
    } else if (flyStatus == FLY_SPEED_UP) {
        setColor(handler, COLOR_SPEED_UP);
    } else {
        setColor(handler, COLOR_VOID);
    }
}

void drawDots()
{
    glUseProgram(gTexProgram);
    checkGlError("bind texture");

    linked_node *cur = getHeaderNode();
    while (cur) {
        dot *d = cur->dot;
        /*drawDot(d);*/
        drawShitDot(d);
        cur = cur->next;
    }
}

void addNewDots(int cnt) 
{
    int i;
    for (i = 0; i < cnt; i ++) {
        dot* d = createDotFromEdge(i);
        insertDot(d);
    }
}

void updateDots() 
{
    int cnt = nodesSize();
    if (cnt < dotCount) {
        addNewDots(dotCount - cnt);
    }

    updatePosition();
}

bool isCollision() 
{
    linked_node *cur = getHeaderNode();
    while (cur) {
        dot* d = cur->dot;
        if (distant(d->x, d->y, planeX, planeY) <= PLANE_SIZE) {
            return true;
        }
        cur = cur->next;
    }

    return false;
}

bool closerNewAngle(dot* d, float newAngle)
{
    float oldAngle = d->angle;
    double d1 = distant(planeX, planeY, d->x + cos(oldAngle), d->y + sin(oldAngle));
    double d2 = distant(planeX, planeY, d->x + cos(newAngle), d->y + sin(newAngle));
    return d1 > d2;
}

void bendAngle(dot* d) 
{
    //Bend dot a bit if the dot fly towards plane.
    double oldAngle = d->angle;

    double newAngle = oldAngle + BEND_UNIT;
    if (closerNewAngle(d, newAngle)) {
        d->angle = newAngle;
        return;
    }

    newAngle = oldAngle - BEND_UNIT;
    if (closerNewAngle(d, newAngle)) {
        d->angle = newAngle;
        return;
    }

    //If the dot if fly away from plane, then don't bend the dot.
}

void dotToString(dot* d)
{
    if (d == NULL) {
        printf("null\n");
    } else {
        printf("x: %d, y: %d, angle %f\n", d->x, d->y, d->angle);
    }
}
