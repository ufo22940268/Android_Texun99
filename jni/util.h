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

extern GLuint gProjectionHandler;
extern GLuint gColorHandler; 
extern GLuint gPosHandler; 
extern GLfloat *gPlaneCoords;
extern GLuint gSizeHandler; 

extern void loadIdentity(GLuint handle); 
extern void loadScreenProjection(GLuint handle); 
extern void translate(int x, int y);

extern void* viewTimer();
extern void* dataTimer();
extern void* bendAngleTimer();
extern void* moveTimer();
extern void* lowFpsTimer();
extern void* highFpsTimer();

extern void finishGame();

extern void lockNode();
extern void unlockNode();

char* readSource(AAsset *asset);
void initDatas();

void drawShitDot(dot *d);


extern bool DEBUG;
extern bool error_printed;
extern void checkGlError(const char* op); 

GLfloat *createTextureCoords(int top, int height, int width); 
GLfloat *createStringTextureCoords(int index); 
//
////Test refactored old interface.
//extern void drawCircle();
//e
