#include <GLES2/gl2.h>
#include <assert.h>
#include "util.h"

extern GLuint sWindowWidth;
extern GLuint sWindowHeight;
extern GLfloat sVirtualWidth;
extern GLfloat sVirtualHeight;
extern GLuint gViewProjectionHandler;
extern GLuint gModelProjectionHandler;

bool DEBUG = true;
bool error_printed = false;

void loadIdentity(GLuint handle) {
    GLfloat identity[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    };
    glUniformMatrix4fv(handle, 1, GL_FALSE, identity);
}

void loadScreenProjection(GLuint handler) {
    assert(sWindowHeight != 0);
    assert(sWindowWidth != 0);
    GLfloat ratio = (GLfloat)sWindowHeight/sWindowWidth;
    assert(ratio != 0);

    GLfloat left   = -100.0f;
    GLfloat right  = 100.0f;
    GLfloat bottom = -100.0f*ratio;
    GLfloat top    = 100.0f*ratio;
    GLfloat identity[16] = {
        2/(right - left)              , 0.0f                          , 0.0f , 0.0f ,
        0.0f                          , 2/(top - bottom)              , 0.0f , 0.0f ,
        0.0f                          , 0.0f                          , 1.0f , 0.0f ,
        (right + left)/(right - left) , (top + bottom)/(top - bottom) , 0.0f , 1.0f ,
    };
    glUniformMatrix4fv(handler, 1, GL_FALSE, identity);
}

void translate(int x, int y) {
    GLfloat translateMat[16] = {
        1.0f , 0.0f , 0.0f , 0.0f ,
        0.0f , 1.0f , 0.0f , 0.0f ,
        0.0f , 0.0f , 1.0f , 0.0f ,
        x    , y    , 0.0f , 1.0f ,
    };
    glUniformMatrix4fv(gViewProjectionHandler, 1, GL_FALSE, translateMat);
}

void checkGlError(const char* op) {
    if (DEBUG && !error_printed) {
        GLint error;
        for (error = glGetError(); error; error
                = glGetError()) {
            LOGD("after %s() glError (0x%x)\n", op, error);
            error_printed = true;
        }
    }
}

GLfloat convertToRelativePos(int pixel) {
    return (GLfloat)pixel/512;
}

GLfloat *createTextureCoords(int ptop, int width, int height) {
    GLfloat left = 0.0;
    GLfloat top = convertToRelativePos(ptop);
    GLfloat right = convertToRelativePos(width);
    GLfloat bottom = top + convertToRelativePos(height);

    GLfloat *matrix = (GLfloat*)malloc(sizeof(GLfloat)*16);

    matrix[0] = left  ; matrix[1] = top    ;
    matrix[2] = right ; matrix[3] = top    ;
    matrix[4] = left  ; matrix[5] = bottom ;
    matrix[6] = right ; matrix[7] = bottom ;

    return matrix;
}

GLfloat *createStringTextureCoords(int index) {
    switch(index) {
        case INDEX_TRACING_STRING:
            return gTracingTexCoords;
        case INDEX_SPEEDUP_STRING:
            return gSpeedupTexCoords;
        case INDEX_SPEEDUP_AND_TRACING_STRING:
            return gSpeedupAndTracingTexCoords;
        default:
            LOGD("no such string texture index.");
            return 0;
    }
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

void drawScore() {
    GLfloat color[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat x = 0;
    GLfloat y = 0;
    GLfloat width = 100;
    GLfloat height = 64;
    GLfloat pos[] = {
        x - width/2, y + height/2,
        x + width/2, y + height/2,
        x - width/2, y - height/2,
        x + width/2, y - height/2,
    };
    drawTexture(pos, gScoreTexCoords, color);
}

void setColor(GLuint handler, int color) 
{
    float r = (float)((color&0xff0000)>>16)/255;
    float g = (float)((color&0x00ff00)>>8)/255; 
    float b = (float)((color&0x0000ff)>>0)/255;
    GLfloat ar[] = {r, g, b, 1.0};
    glUniform4fv(handler, 1, ar);
}

void test_drawBackground() {
    GLfloat color[4] = {1, 1, 1, 1};
    GLfloat pos[8] = {
        -80.0 , 80.0  ,
        80.0  , 80.0  ,
        -80.0 , -80.0 ,
        80.0  , -80.0 ,
    };
    GLfloat coords[8] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
    };
    drawTexture(pos, coords, color);
}
