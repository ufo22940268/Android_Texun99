#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

#include "util.h"

//TODO implement thess vars.
int updateCount;
int gameStatus = STATUS_NORMAL;
long startTime;
long lastingTime = -1;
long currentTime;

extern int flyStatus;

pthread_mutex_t gNodeLock;

GLuint gProgram;

GLfloat triangleCoords[] = {
    0.0f, 50.0f,
    -50.0f, -50.0f,
    50.0f, -50.0f,
};

GLfloat triangleColor[] = {
    1.0f, 0.0f, 0.0f, 1.0f,
};

GLfloat *gPlaneCoords;

GLuint sWindowHeight;
GLuint sWindowWidth;

GLfloat sVirtualHeight;
GLfloat sVirtualWidth;

GLuint gProjectionHandler;
GLuint gColorHandler; 
GLuint gPosHandler; 
GLuint gSizeHandler; 
GLuint gOrthoHandler; 

bool DEBUG = 0;

static void checkGlError(const char* op) {
    if (DEBUG) {
        GLint error;
        for (error = glGetError(); error; error
                = glGetError()) {
            LOGD("after %s() glError (0x%x)\n", op, error);
        }
    }
}

void debugError(char* str) {
    int error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGD("error: %s\t error code: %x ", str, error);
    }
}

GLuint loadShader(GLenum type, const GLchar* pSource) {
    GLuint shader= glCreateShader(type);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGD("Could not compile shader %d:\n%s\n",
                            type, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

void initPlaneCoords() {
    int count = 360;
    int stride = 2;
    GLfloat size = PLANE_SIZE;
    gPlaneCoords = (GLfloat*)malloc(sizeof(GLfloat)*count*stride);
    int i;
    for (i = 0; i < count*stride; i += 2) {
        gPlaneCoords[i] = cos(toRadians(i))*size;
        gPlaneCoords[i + 1] = sin(toRadians(i))*size;
    }
}

void loadSource(char* vertexSource, char* fragmentSource) {
    gProgram = glCreateProgram();

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    glAttachShader(gProgram, vertexShader);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    glAttachShader(gProgram, fragmentShader);

    glLinkProgram(gProgram);

    free(vertexSource);
    free(fragmentSource);
}

void
startTimer() {
    pthread_t *highThread = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create(highThread, NULL, highFpsTimer, NULL);

    pthread_t *lowThread = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create(lowThread, NULL, lowFpsTimer, NULL);

    startTime = time(NULL);
}

void
initLocks() {
    pthread_mutex_init(&gNodeLock, NULL);
}

void
destroyLocks() {
    pthread_mutex_destroy(&gNodeLock);
}

//TODO stop update threads when program exit.
void
stopThreads() {
}


AAsset*
getAsset(JNIEnv *env, jobject thiz, char *fileName) {
    AAssetManager *manager = AAssetManager_fromJava(env, thiz);
    return AAssetManager_open(manager, fileName, AASSET_MODE_STREAMING);
}

char*
readSource(AAsset *asset) {
    int length = AAsset_getLength(asset);
    char *str = (char*)malloc(sizeof(char)*length);
    memset(str, 0, length);
    LOGD("-----------------length:%d------------------", length);
    if (asset != NULL) {
        AAsset_read(asset, str, sizeof(char)*length);
        LOGD("-----------------%s------------------", str);
        AAsset_close(asset);
    }

    return str;

}

void
Java_opengl_demo_NativeRenderer_init(JNIEnv *env, jobject thiz, jobject assetManager) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    gameStatus = STATUS_NORMAL;
    initDatas();

    initLocks();

    AAsset *asset = getAsset(env, assetManager, "vertex.glsl");
    char *vertexSource = readSource(asset);
    asset = getAsset(env, assetManager, "fragment.glsl");
    char *fragmentSource = readSource(asset);
    loadSource(vertexSource, fragmentSource);
    initPlaneCoords();

    gProjectionHandler = glGetUniformLocation(gProgram, "uMVPMatrix");
    gColorHandler = glGetUniformLocation(gProgram, "vColor");
    gPosHandler = glGetAttribLocation(gProgram, "vPosition");
    gSizeHandler = glGetAttribLocation(gProgram, "vPointSize");
    gOrthoHandler = glGetUniformLocation(gProgram, "uOthoMatrix");
    checkGlError("3");

    startTimer();

    (*env)->DeleteLocalRef(env, assetManager);
}

void
Java_opengl_demo_NativeRenderer_change(JNIEnv *env,
        jobject thiz, int width, int height) {
    glViewport(0, 0, width, height);
    sWindowWidth = width;
    sWindowHeight = height;
    debugError("4");
}

void
*test_print() {
    while (1) {
        useconds_t msec = 1000*1000;
        usleep(msec);
    }
}

//TODO
void finishGame() 
{
    gameStatus = STATUS_FINISHED;

    if (lastingTime == -1) {
        lastingTime = time(NULL) - startTime;
    }
}

void quitGame() {
    gameStatus = STATUS_QUITED;

    initDatas();
    stopThreads();
    destroyLocks();
}

void initDatas() {
    resetKeyStatus();
    resetTimeData();
    freeAllNodes();

    startTime = time(NULL);
    lastingTime = -1;
    flyStatus = FLY_VOID;
}

void
Java_opengl_demo_NativeRenderer_test(JNIEnv *env, jobject thiz) {
    /*pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t));e/
    /*pthread_create(thread, NULL, test_print, NULL);*/
}

void
Java_opengl_demo_NativeRenderer_step(JNIEnv *env, jobject thiz) {
    lockNode();

    glUseProgram(gProgram);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    loadScreenProjection(gOrthoHandler);

    drawPlane();
    checkGlError("plane");

    drawDots();
    checkGlError("dot");

    unlockNode();
}

void
Java_opengl_demo_MainActivity_quitGame(JNIEnv *env, jobject thiz) {
    quitGame();
}

void
Java_opengl_demo_MainActivity_pressUp(JNIEnv *env, jobject thiz) {
    /*LOGD("press Up--------");*/
    movePlaneInDirection(DIRECTION_UP);
}

void
Java_opengl_demo_MainActivity_pressDown(JNIEnv *env, jobject thiz) {
    /*LOGD("press Down--------");*/
    movePlaneInDirection(DIRECTION_DOWN);
}

void
Java_opengl_demo_MainActivity_pressLeft(JNIEnv *env, jobject thiz) {
    movePlaneInDirection(DIRECTION_LEFT);
}

void
Java_opengl_demo_MainActivity_pressRight(JNIEnv *env, jobject thiz) {
    movePlaneInDirection(DIRECTION_RIGHT);
}
