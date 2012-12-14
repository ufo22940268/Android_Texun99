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
