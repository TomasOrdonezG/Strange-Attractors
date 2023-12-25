#ifndef STRANGE_ATTRACTORS_H
#define STRANGE_ATTRACTORS_H

// * HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <math.h>
#include "lib/SDL2_gfx/SDL2_gfxPrimitives.h"

// * MACRODEFINITIONS
#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT (780)
#define RGB_MAX (255)
#define PI (3.14152)
#define WHITE 255, 255, 255, 255
#define BLACK 0, 0, 0, 255
#define CIRCLE
#define MODEL_COUNT 6

// * GENERAL EXTERNAL VARIABLES
float aspect_ratio = SCREEN_WIDTH/SCREEN_HEIGHT;
float FOV = 3.5;
int settings = 0;
int colourControl = 0;

struct UserMouse {
    int down;
    int x;
    int y;
    int down_x;
    int down_y;
    int up_x;
    int up_y;
} mouse = {
    .down = 0,
};

struct Frustum {
    float r;
    float t;
    float n;
    float f;
} frustum = {
    .n = 0.05,
    .f = 20.0
};

struct Trail_Colour {
    int ri;
    int rf;
    int gi;
    int gf;
    int bi;
    int bf;
    int ai;
    int af;
} trail_rgba = {
    // Yellow to Purple
    .ri = 255,
    .rf = 117,
    .gi = 255,
    .gf = 11,
    .bi = 79,
    .bf = 255,
    .ai = 196,
    .af = 255
};

struct Sliders {
    char *label;
    double *link;
    int *int_link;
    double max;
    double min;
    double y;
    int selected;
} sliders[] = {
    {
        .label = "a",
        .max = 30,
        .min = -10
    },
    {
        .label = "b",
        .max = 30,
        .min = -5
    },
    {
        .label = "c",
        .max = 30,
        .min = -1
    },
    {
        .label = "zoom",
        .max = 1,
        .min = 0.1
    },
    {
        .label = "Xrot",
        .max = 0.00001,
        .min = 0.0
    },
    {
        .label = "Yrot",
        .max = 0.00001,
        .min = 0.0
    },
    {
        .label = "Zrot",
        .max = 0.00001,
        .min = 0.0
    },
    {
        .label = "dt",
        .max = 0.05,
        .min = 0.00001
    },
    {
        .label = "length",
        .max = 5000,
        .min = 2
    },
};


// * STRANGE ATTRACTORS

struct Point {
    struct Point *next;
    float x;
    float y;
    float z;
};

// Attractor Function Prototypes
void BanlueAttractor(struct Point *newPoint);
void LorenzAttractor(struct Point *newPoint);
void HalvorsenAttractor(struct Point *newPoint);
void AizawaAttractor(struct Point *newPoint);
void LuChenAttractor(struct Point *newPoint);
void GenesioAttractor(struct Point *newPoint);

// Strange Attractor Models
double default_x = 0.01;
double default_y = 0.01;
double default_z = 0.01;
typedef struct {
    double dtime;
    double zoom;
    void (*attractorFunction)(struct Point*);
    struct {
        double a;
        double b;
        double c;
        double d;
        double e;
    } parameters;
    struct {
        double x;
        double y;
        double z;
    } initialPosition;
    struct {
        float x;
        float y;
        float z;
    } midpoint;
    struct {
        double angle_x;
        double angle_y;
        double angle_z;
        double dangle_x;
        double dangle_y;
        double dangle_z;
    } rotation;
    struct {
        struct Point *head;
        struct Point *tail;
        int length;
        int maxLength;
    } trail;
} StrangeAttractor;
StrangeAttractor defaultModel; // Model used to store original settings
StrangeAttractor AttractorModels[MODEL_COUNT] = {
    // ? LORENZ MODEL
    {
        .dtime = 0.005,
        .zoom = 0.17,
        .attractorFunction = LorenzAttractor,
        .parameters = {10.0, 28.0, 8.0/3.0},
        .initialPosition = {0.8, 0.5, 4.1},
        .midpoint = {1.94, 3.53, 25.57},
        .rotation = {.dangle_x = 0.000000, 0.000001, 0.000000},
        .trail = {.maxLength = 1000,}
    },
    // ? BANLUE MODEL
    {
        .dtime = 0.02,
        .zoom = 0.25,
        .attractorFunction = BanlueAttractor,
        .parameters = {2, 0, 0},
        .initialPosition = {0.8, 0.5, 0.1},
        .midpoint = {-0.18, -0.34, 0.75},
        .rotation = {.dangle_x = 0.000002, 0.000001, 0.000000},
        .trail = {.maxLength = 1000}
    },
    // ? HALVORSEN MODEL
    {
        .dtime = 0.02,
        .zoom = 0.2,
        .attractorFunction = HalvorsenAttractor,
        .parameters = {1.97, 0, 0},
        .initialPosition = {0.8, 0.5, 4.1},
        .midpoint = {-2.28, -3.88, -4.41},
        .rotation = {.dangle_x = 0.000002, 0.000001, 0.000000},
        .trail = {.maxLength = 3000}
    },
    // ? AIZAWA MODEL
    {
        .dtime = 0.0145,
        .zoom = 0.65,
        .attractorFunction = AizawaAttractor,
        .parameters = {0.25, 0.96, 3.5},
        .initialPosition = {0.01, 0.01, 0.01},
        .midpoint = {-0.01, 0.05, 0.61},
        .rotation = {.dangle_x = 0.000000, 0.000001, 0.0000005},
        .trail = {.maxLength = 1000}
    },
    // ? LUCHEN MODEL
    {
        .dtime = 0.0027,
        .zoom = 0.19,
        .attractorFunction = LuChenAttractor,
        .parameters = {-10.0, -4.0, 18.1},
        .initialPosition = {0.8, 0.5, 4.1},
        .midpoint = {3.29, 4.07, 18.24},
        .rotation = {1.2, 0.2, 4.2, 0.000000, 0.000001, 0.000000},
        .trail = {.maxLength = 2000}
    },
    // ? GENESIO MODEL
    {
        .dtime = 0.022,
        .zoom = 0.75,
        .attractorFunction = GenesioAttractor,
        .parameters = {0.439, 1.1, 1.0},
        .initialPosition = {0.1, 0.1, 0.0},
        .midpoint = {0.34, 0.14, 0.05},
        .rotation = {.dangle_x = 0.000000, 0.000002, 0.000001},
        .trail = {.maxLength = 1000}
    },
};

// Current attractor
enum StrangeAttractorType {
    LORENZ,
    BANLUE,
    HALVORSEN,
    AIZAWA,
    LUCHEN,
    GENESIO,
} currentAttractorType;
StrangeAttractor *currentAttractor;

// * GENERAL FUNCTION PROTOTYPES
void project(float x, float y, float z, float *xp, float *yp, float *zp);
void clear(SDL_Renderer *renderer);
void handleEvents(int *running);
void rotateX(float *x, float *y, float *z, float angle);
void rotateY(float *x, float *y, float *z, float angle);
void rotateZ(float *x, float *y, float *z, float angle);
void calculateAttractor();
void freeModel(struct Point *head);
void initializeModel();
void initializeFrustum();
void controls(SDL_Renderer *renderer);
void getTrailRGBA(int ri, int rf, int gi, int gf, int bi, int bf, int ai, int af, int pos, int length, int *r, int *g, int *b, int *a);
void trailColourControl(SDL_Renderer *renderer);
void setCurrentAttractor(enum StrangeAttractorType newAttractorType);

#endif
