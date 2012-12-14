#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <jni.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

#include "types.h"

#define SHUFFLE time(NULL)

extern int gameStatus;

float toRadians(int degree);
void drawCircle();
void drawPlane();
void movePlane(int x, int y);
void movePlaneInDirection(int key);
void drawDots();
void setDotColor();
void drawString(int x, int y, int index);
void drawFlyStatusString(char* str);
bool isCollision();
void drawDot(dot *d);

void setColor(GLuint handler, int color);

void updateDots();
void chooseEdge(int *x, int *y);

int timeRand();
float timeRandf(); 
double distant(double xa, double ya, double xb, double yb);

void dotToString(dot* d);

linked_node* getHeaderNode();
void insertDot(dot *dot);
bool deleteDot(dot *dot);
int nodesSize();
void freeAllNodes();

dot* createDotFromEdge();
void bendAngle(dot* d);
void bendAllDots();

void resetTimeData();
void updateTimeData();

bool isControllerKey(char key);
void controlPlane(char key);

void resetKeyStatus();
int getPlaneDirection();
void pressKey(char key);
void releaseKey(char key);

void controlPlaneRelease(char key);

void resetGame();

bool closerNewAngle(dot* d, float newAngle);

/**
 * Is the right angle to fly towards to zero point.
 */
bool isRightAngle(int x, int y, float angle);

bool shouldBendDots();
bool shouldSpeedUpDots();
bool shouldSpeedUpDotsPermanentaly();

//----------------------------------------------------------------------------------------------------
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "renderer", __VA_ARGS__) 
GLfloat *createStringTextureCoords(int index); 
GLfloat *createTextureCoords(int top, int height, int width); 
char* readSource(AAsset *asset);
void checkGlError(const char* op); 
void drawFlyStatus();
void drawShitDot(dot *d);
void drawSpeedUpString();
void drawTexture();
void drawTracingAndSpeedUpString();
void drawTracingString();
void drawTrollFace();
void drawScore();
void finishGame();
void initDatas();
void loadIdentity(GLuint handle); 
void loadScreenProjection(GLuint handle); 
void lockNode();
void printArray(GLfloat *array, int length);
void test_drawBackground();
void translate(int x, int y);
void unlockNode();
void* bendAngleTimer();
void* dataTimer();
void* highFpsTimer();
void* lowFpsTimer();
void* moveTimer();
void* viewTimer();
bool isRunning();
bool isQuited();
bool isFinished();

