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

GLuint gEnvProgram;
GLuint gCuteShitProgram;

GLfloat triangleCoords[] = {
    0.0f, 50.0f,
    -50.0f, -50.0f,
    50.0f, -50.0f,
};

GLfloat triangleColor[] = {
    1.0f, 0.0f, 0.0f, 1.0f,
};

GLuint gTextureHandlers[1];

GLfloat *gPlaneCoords;

GLuint sWindowHeight;
GLuint sWindowWidth;

GLfloat sVirtualHeight;
GLfloat sVirtualWidth;

GLuint gViewProjectionHandler;
GLuint gColorHandler; 
GLuint gPosHandler; 
GLuint gSizeHandler; 
GLuint gModelProjectionHandler; 

bool DEBUG = true;
bool error_printed = false;

static void checkGlError(const char* op) {
    if (DEBUG && !error_printed) {
        GLint error;
        for (error = glGetError(); error; error
                = glGetError()) {
            LOGD("after %s() glError (0x%x)\n", op, error);
            error_printed = true;
        }
    }
}

void debugError(char* str) {
    int error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGD("error: %s\t error code: %x ", str, error);
    }
}

AAsset*
getAsset(JNIEnv *env, jobject thiz, char *fileName) {
    AAssetManager *manager = AAssetManager_fromJava(env, thiz);
    return AAssetManager_open(manager, fileName, AASSET_MODE_STREAMING);
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

GLuint loadSource(JNIEnv *env, jobject assetManager, char* vertexFile, char* fragmentFile) {
    AAsset *asset = getAsset(env, assetManager, vertexFile);
    char *vertexSource = readSource(asset);
    asset = getAsset(env, assetManager, fragmentFile);
    char *fragmentSource = readSource(asset);

    GLuint program = glCreateProgram();

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    glAttachShader(program, vertexShader);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLint bufLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
        if (bufLength) {
            char* buf = (char*) malloc(bufLength);
            if (buf) {
                glGetProgramInfoLog(program, bufLength, NULL, buf);
                LOGD("Could not link program:\n%s\n", buf);
                free(buf);
            }
        }
        glDeleteProgram(program);
        program = 0;
    }

    free(vertexSource);
    free(fragmentSource);

    return program;
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

char*
readSource(AAsset *asset) {
    int length = AAsset_getLength(asset);
    char *str = (char*)malloc(sizeof(char)*length + 3);
    memset(str, 0, length + 3);
    /*LOGD("-----------------length:%d------------------", length);*/
    if (asset != NULL) {
        AAsset_read(asset, str, sizeof(char)*length + 3);
        /*LOGD("-----------------%s------------------", str);*/
        AAsset_close(asset);
    }
    return str;
}

void
initPrograms(JNIEnv *env, jobject assetManager) {
    gEnvProgram = loadSource(env, assetManager, "environment_vertex.glsl", "environment_fragment.glsl");
    checkGlError("2");
    gCuteShitProgram = loadSource(env, assetManager, "cute_shit_vertex.glsl", "cute_shit_fragment.glsl");
    checkGlError("3");
}

void initTextures() {
    glGenTextures(1, gTextureHandlers);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void
Java_opengl_demo_NativeRenderer_init(JNIEnv *env, jobject thiz, jobject assetManager) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    /*glClearColor(1.0f, 1.0f, 1.0f, 1.0f);*/

    gameStatus = STATUS_NORMAL;

    initDatas();
    initLocks();
    initTextures();
    initPrograms(env, assetManager);
    initPlaneCoords();

    gViewProjectionHandler = glGetUniformLocation(gEnvProgram, "u_ViewProjection");
    gColorHandler = glGetUniformLocation(gEnvProgram, "vColor");
    gPosHandler = glGetAttribLocation(gEnvProgram, "vPosition");
    gSizeHandler = glGetAttribLocation(gEnvProgram, "vPointSize");
    gModelProjectionHandler = glGetUniformLocation(gEnvProgram, "u_ModelProjection");

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
    /**pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t));*/
    /*[>pthread_create(thread, NULL, test_print, NULL);<]*/
}

void
Java_opengl_demo_NativeRenderer_step(JNIEnv *env, jobject thiz) {
    lockNode();

    glUseProgram(gEnvProgram);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    loadScreenProjection(gModelProjectionHandler);

    drawPlane();
    checkGlError("plane");

    drawDots();
    dot d = {.x = 50, .y = 50};
    /*drawShitDot(&d);*/
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
