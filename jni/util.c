#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <GLES2/gl2.h>
#include "util.h"

int global_seed = 0;

extern long startTime;
extern GLuint gTexProgram;
extern 

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

void drawString(int x, int y, int index) 
{
    glUseProgram(gTexProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureHandlers[0]);
    checkGlError("bind string texture");

    GLuint posHandler = glGetAttribLocation(gTexProgram, "a_Position");
    GLuint texCoordHandler = glGetAttribLocation(gTexProgram, "a_TexCoord");
    GLuint mvpHandler = glGetUniformLocation(gTexProgram, "u_MVPMatrix");
    GLuint samplerHandler = glGetUniformLocation(gTexProgram, "u_sampler");
    GLuint colorHandler = glGetUniformLocation(gTexProgram, "u_color");

    loadScreenProjection(mvpHandler);

    GLfloat pos[4*2];
    createStringPos((float)x, (float)y, pos);
    glVertexAttribPointer(posHandler, 2, GL_FLOAT, GL_FALSE, 0, pos);
    glEnableVertexAttribArray(posHandler);

    glUniform1i(samplerHandler, 0);
    glVertexAttribPointer(texCoordHandler, 2, GL_FLOAT, GL_FALSE, 0, createStringTextureCoords(index));
    glEnableVertexAttribArray(texCoordHandler);

    GLfloat stringColor[4] = {1, 1, 1, 1};
    glUniform4fv(colorHandler, 1, stringColor);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    checkGlError("draw string text.");
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
    GLfloat ar[] = {r, g, b, 1};
    glUniform4fv(handler, 1, ar);
}

double distant(double xa, double ya, double xb, double yb) 
{
    return sqrt(pow(xa - xb, 2) + pow(ya - yb, 2));
}
