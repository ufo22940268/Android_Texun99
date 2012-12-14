#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <GLES2/gl2.h>
#include "util.h"

int global_seed = 0;

extern long startTime;
extern GLuint gTexProgram;

float toRadians(int degree) {
    return M_PI/180*degree;
}

void drawCircle()
{
    glEnableVertexAttribArray(gPosHandler);
    glVertexAttribPointer(gPosHandler, 2, GL_FLOAT, GL_FALSE, 0, gPlaneCoords);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 360);
    glDisableVertexAttribArray(gPosHandler);
}

void createStringPos(GLfloat x, GLfloat y, GLfloat *pos) {
    int width = 80;
    int height = 30;
    pos[0] = x         ; pos[1] = y          ;
    pos[2] = x + width ; pos[3] = y          ;
    pos[4] = x         ; pos[5] = y - height ;
    pos[6] = x + width ; pos[7] = y - height ;
}

void drawDotStatusString(int index) {
    drawString(-sVirtualWidth, sVirtualHeight, index);
}

void drawTracingString() {
    drawDotStatusString(INDEX_TRACING_STRING);
}

void drawSpeedUpString() {
    drawDotStatusString(INDEX_SPEEDUP_STRING);
}

void drawTracingAndSpeedUpString() {
    drawDotStatusString(INDEX_SPEEDUP_AND_TRACING_STRING);
}

void drawString(int x, int y, int index) 
{
    GLfloat texturePos[4*2];
    createStringPos((float)x, (float)y, texturePos);
    GLfloat *textureCoords = createStringTextureCoords(index);
    GLfloat color[4] = {1, 1, 1, 1};
    drawTexture(texturePos, textureCoords, color);
}

void drawTexture(GLfloat *texturePos, GLfloat *textureCoords, GLfloat *color) {
    glUseProgram(gTexProgram);

    GLuint posHandler = glGetAttribLocation(gTexProgram, "a_Position");
    GLuint texCoordHandler = glGetAttribLocation(gTexProgram, "a_TexCoord");
    GLuint mvpHandler = glGetUniformLocation(gTexProgram, "u_MVPMatrix");
    GLuint samplerHandler = glGetUniformLocation(gTexProgram, "u_sampler");
    GLuint colorHandler = glGetUniformLocation(gTexProgram, "u_color");

    loadScreenProjection(mvpHandler);

    glVertexAttribPointer(posHandler, 2, GL_FLOAT, GL_FALSE, 0, texturePos);
    glEnableVertexAttribArray(posHandler);

    glUniform1i(samplerHandler, 0);
    glVertexAttribPointer(texCoordHandler, 2, GL_FLOAT, GL_FALSE, 0, textureCoords);
    glEnableVertexAttribArray(texCoordHandler);

    glUniform4fv(colorHandler, 1, color);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void drawFlyStatus()
{
    if (flyStatus == FLY_VOID) {
        return;
    } else if (flyStatus == (FLY_BEND | FLY_SPEED_UP)) {
        drawTracingAndSpeedUpString();
    } else if (flyStatus == FLY_BEND) {
        drawTracingString();
    } else if (flyStatus == FLY_SPEED_UP) {
        drawSpeedUpString();
    } else {
        LOGD("status string error\n");
    }
}

void drawFlyStatusString(char* str) {
    /*drawString(-100, 90, str);*/
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

void setColor(GLuint handler, int color) 
{
    float r = (float)((color&0xff0000)>>16)/255;
    float g = (float)((color&0x00ff00)>>8)/255; 
    float b = (float)((color&0x0000ff)>>0)/255;
    GLfloat ar[] = {r, g, b, 1.0};
    glUniform4fv(handler, 1, ar);
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
