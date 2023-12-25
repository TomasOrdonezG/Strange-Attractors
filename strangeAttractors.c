#include "strangeAttractors.h"

// * MAIN
int main(int argc, char **argv)
{
    // * Initialize SDL
    SDL_Window *window = SDL_CreateWindow("Strange Attractors", 10, 10, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, 1280, 720);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    // Set initial attractor
    currentAttractorType = LORENZ;
    currentAttractor = &(AttractorModels[currentAttractorType]);

    // Save default model
    defaultModel = *currentAttractor;
    initializeFrustum();
    initializeModel();

    // * Main game loop
    int running = 1;
    double minx = 100, maxx = -100, miny = 100, maxy = -100, minz = 100, maxz = -100;
    while (running) {
        handleEvents(&running);

        SDL_GetMouseState(&mouse.x, &mouse.y);
        clear(renderer);
        if (!colourControl) calculateAttractor();

        // * Render each line
        int i = 0;
        float point_i[3];
        for (struct Point *current_point = currentAttractor->trail.head; current_point != NULL && !colourControl; current_point = current_point->next, i++) {
            float point_f[3] = {current_point->x, current_point->y, current_point->z};

            // Calculate midpoints
            {
                if (current_point->x > maxx) maxx = current_point->x;
                if (current_point->x < minx) minx = current_point->x;
                if (current_point->y > maxy) maxy = current_point->y;
                if (current_point->y < miny) miny = current_point->y;
                if (current_point->z > maxz) maxz = current_point->z;
                if (current_point->z < minz) minz = current_point->z;
            }

            // * Center the model at (0,0,0) and rotate segments
            point_f[0] -= currentAttractor->midpoint.x;
            point_f[1] -= currentAttractor->midpoint.y;
            point_f[2] -= currentAttractor->midpoint.z;
            rotateX(&point_f[0], &point_f[1], &point_f[2], currentAttractor->rotation.angle_x);
            rotateY(&point_f[0], &point_f[1], &point_f[2], currentAttractor->rotation.angle_y);
            rotateZ(&point_f[0], &point_f[1], &point_f[2], currentAttractor->rotation.angle_z);
            point_f[2] += 1 / (currentAttractor->zoom*currentAttractor->zoom);
            
            // * Increment angle
            currentAttractor->rotation.angle_x = fmod(currentAttractor->rotation.angle_x + currentAttractor->rotation.dangle_x, 2 * PI);
            currentAttractor->rotation.angle_y = fmod(currentAttractor->rotation.angle_y + currentAttractor->rotation.dangle_y, 2 * PI);
            currentAttractor->rotation.angle_z = fmod(currentAttractor->rotation.angle_z + currentAttractor->rotation.dangle_z, 2 * PI);

            // * Apply perspective projection
            project(point_f[0], point_f[1], point_f[2], &point_f[0], &point_f[1], &point_f[2]);

            // * Draw segments
            if (current_point != currentAttractor->trail.head) {
                int r, g, b, a;
                getTrailRGBA(trail_rgba.ri, trail_rgba.rf, trail_rgba.gi, trail_rgba.gf, trail_rgba.bi, trail_rgba.bf, trail_rgba.ai, trail_rgba.af, i, currentAttractor->trail.length, &r, &g, &b, &a);
                aalineRGBA(renderer, point_i[0], point_i[1], point_f[0], point_f[1], r, g, b, a);
            }

            // * Record points
            for (int l = 0; l < 3; l++) point_i[l] = point_f[l];

            #ifdef CIRCLE
                // * Draw Circle at tail and/or head
                if (current_point == currentAttractor->trail.tail) {
                    filledCircleRGBA(renderer, point_f[0], point_f[1], 2, 255, 255, 255, 255);
                }
            #endif
        }

        // Settings
        controls(renderer);
        trailColourControl(renderer);

        // Present
        SDL_RenderPresent(renderer);
    }    

    freeModel(currentAttractor->trail.head);
    currentAttractor->trail.head = NULL;

    // Quit SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    printf("Estimated midpoint: (%.2lf, %.2lf, %.2lf)\n", (minx+maxx)/2, (miny+maxy)/2, (minz+maxz)/2);
    
    return 0;
}

// * FUNCTION DEFINITIONS
void project(float x, float y, float z, float *x_proj, float *y_proj, float *z_proj)
{
    // Project
    *x_proj = x * (frustum.n / frustum.r);
    *y_proj = y * (frustum.n / frustum.t);
    *z_proj = -z * (frustum.f + z + (2 * frustum.f * frustum.n)) / (frustum.f - frustum.n);

    // Homogenize
    *x_proj /= -z;
    *y_proj /= -z;
    *z_proj /= -z;

    // Translate to screen
    *x_proj += frustum.r;
    *y_proj += frustum.t;

    // Fit screen
    *x_proj *= SCREEN_WIDTH / (2 * frustum.r);
    *y_proj *= SCREEN_HEIGHT / (2 * frustum.t);

    return;
}

void clear(SDL_Renderer *renderer)
{      
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, BLACK);
    SDL_RenderClear(renderer);
    return;
}

void rotateX(float *x, float *y, float *z, float angle)
{
    if (angle == 0) return;
    float s = sin(angle);
    float c = cos(angle);
    float yr = (*y * c) - (*z * s);
    float zr = (*y * s) + (*z * c);
    *y = yr;
    *z = zr;
    return;
}

void rotateY(float *x, float *y, float *z, float angle)
{
    if (angle == 0) return;
    float s = sin(angle);
    float c = cos(angle);
    float xr = (*z * s) + (*x * c);
    float zr = (*z * c) - (*x * s);
    *x = xr;
    *z = zr;
    return;
}

void rotateZ(float *x, float *y, float *z, float angle)
{
    if (angle == 0) return;
    float c = cos(angle);
    float s = sin(angle);
    float xr = (*x * c) - (*y * s);
    float yr = (*x * s) + (*y * c);
    *x = xr;
    *y = yr;
    return;
}

void handleEvents(int *running)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Mouse up and down positions
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            mouse.down = 1;
            SDL_GetMouseState(&mouse.down_x, &mouse.down_y);
        }
        if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
            mouse.down = 0;
            SDL_GetMouseState(&mouse.up_x, &mouse.up_y);
        }

        // Quit event on quit or `ESC`
        if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            *running = 0;
        }

        // Keydown events
        if (event.type == SDL_KEYDOWN) switch (event.key.keysym.sym) {
            case SDLK_s: // `S` Settings toggle
                colourControl = 0;
                settings = !settings;
                break;
            case SDLK_c: // `C` Trail Colour Settings toggle
                settings = 0;
                colourControl = !colourControl;
                break;
            case SDLK_r: // `R` Restart model
                setCurrentAttractor(currentAttractorType);
                break;
        }

        // New attractor on number key
        for (char key = '1'; key < '1' + MODEL_COUNT && event.type == SDL_KEYDOWN; key++) {
            if (event.key.keysym.sym == key)
                setCurrentAttractor(key - '1');
        }
    }
    return;
}

void calculateAttractor()
{
    // * Calculate new point according to current attractor
    struct Point *newPoint = malloc(sizeof(struct Point));
    currentAttractor->attractorFunction(newPoint);

    // * Append new point to the model
    newPoint->next = NULL;
    currentAttractor->trail.tail->next = newPoint;
    currentAttractor->trail.tail = newPoint;
    currentAttractor->trail.length++;
    
    // * Remove head(s)
    int frees = (currentAttractor->trail.length >= currentAttractor->trail.maxLength) + (currentAttractor->trail.length > currentAttractor->trail.maxLength);
    for (int i = 0; i < frees; i++) {
        struct Point *oldHead = currentAttractor->trail.head;
        currentAttractor->trail.head = currentAttractor->trail.head->next;
        free(oldHead);
        currentAttractor->trail.length--;
    }

    return;
}

void freeModel(struct Point *head)
{
    if (head->next) freeModel(head->next);
    head->next = NULL;
    currentAttractor->trail.length--;
    free(head);
    return;
}

void initializeModel()
{
    // Initial point
    currentAttractor->trail.head = malloc(sizeof(struct Point));
    currentAttractor->trail.tail = currentAttractor->trail.head;
    currentAttractor->trail.tail->x = currentAttractor->initialPosition.x;
    currentAttractor->trail.tail->y = currentAttractor->initialPosition.y;
    currentAttractor->trail.tail->z = currentAttractor->initialPosition.z;
    currentAttractor->trail.length++;

    // Initial trail
    struct Point *next_point = currentAttractor->trail.tail;
    while (currentAttractor->trail.length < currentAttractor->trail.maxLength) {
        struct Point *new_point = malloc(sizeof(struct Point));
        new_point->next = next_point;

        new_point->x = default_x;
        new_point->y = default_y;
        new_point->z = default_z;
        
        next_point = new_point;
        currentAttractor->trail.head = new_point;
        currentAttractor->trail.length++;
    }
    return;
}

void initializeFrustum()
{
    frustum.r = frustum.n * tan(FOV / 2);
    frustum.t = frustum.r / aspect_ratio;
    return;
}

void controls(SDL_Renderer *renderer)
{
    if (settings) {
        // Set up links
        sliders[0].link = &(currentAttractor->parameters.a);
        sliders[1].link = &(currentAttractor->parameters.b);
        sliders[2].link = &(currentAttractor->parameters.c);
        sliders[3].link = &(currentAttractor->zoom);
        sliders[4].link = &(currentAttractor->rotation.dangle_x);
        sliders[5].link = &(currentAttractor->rotation.dangle_y);
        sliders[6].link = &(currentAttractor->rotation.dangle_z);
        sliders[7].link = &(currentAttractor->dtime);
        sliders[8].int_link = &(currentAttractor->trail).maxLength;

        int top = 80;
        int bottom  = SCREEN_HEIGHT - 80;
        int radius = 8;
        int slider_count = (sizeof(sliders) / sizeof(sliders[0]));
        int spacing  = SCREEN_WIDTH / (slider_count + 1);
        static int alpha = 255;

        // * Sliders
        for (int i = 0; i < slider_count; i++) {
            int x = spacing * (i + 1);

            // Calculate Y based on link value
            float slope = ((top - bottom) / (sliders[i].max - sliders[i].min));
            sliders[i].y = slope * (sliders[i].int_link ? (float)(*(sliders[i].int_link)) : *(sliders[i].link) - sliders[i].min) + bottom;

            // Render value
            char s[30];
            sprintf(s, "%s = %.2f", sliders[i].label, (sliders[i].int_link ? (float)(*(sliders[i].int_link)) : *(sliders[i].link)));
            stringRGBA(renderer, x - 30, top - 20, s, 255, 255, 255, alpha);

            // Render slider
            lineRGBA(renderer, x, bottom, x, top, 255, 255, 255, alpha);
            filledCircleRGBA(renderer, x, sliders[i].y, radius, 255, 255, 255, alpha);

            // Slider clicked
            if ((mouse.down && mouse.down_x < x + radius && mouse.down_x > x - radius && mouse.down_y > top && mouse.down_y < bottom) || sliders[i].selected) {
                alpha = 50;
                sliders[i].selected = 1;
                if (mouse.y <= bottom && mouse.y >= top) { // Change link value based of mouse Y
                    if (sliders[i].int_link) // Change int link
                        *(sliders[i].int_link) = ((sliders[i].max - sliders[i].min) / (top - bottom))*(mouse.y - bottom) + sliders[i].min;
                    else if (sliders[i].link) // Change double link
                        *(sliders[i].link) = ((sliders[i].max - sliders[i].min) / (top - bottom))*(mouse.y - bottom) + sliders[i].min;
                }
            }
            
            // Unselect
            if (!mouse.down) {
                alpha = 255;
                sliders[i].selected = 0;
            }
        }
    }
    return;
}

void trailColourControl(SDL_Renderer *renderer)
{
    if (colourControl) {
        clear(renderer);

        // * Slider variables
        int top = 300;
        int bottom = SCREEN_HEIGHT - 100;
        int width = 50;
        int height = bottom - top;
        int spacing = SCREEN_WIDTH / 5;
        int r, g, b, a;

        // * Preview trail variables
        int radius = 20;
        int prev_y = top / 2;
        int length = 0.85 * SCREEN_WIDTH;
        int left = (SCREEN_WIDTH / 2) - (0.5 * length);
        int right = (SCREEN_WIDTH / 2) + (0.5 * length);

        // * Initialize rgba struct
        static struct {
            int *link_i;
            int *link_f;
            int yi;
            int yf;
            int selected_i;
            int selected_f;
        }
        rgba[4] = {
            {&(trail_rgba.ri), .link_f = &(trail_rgba.rf)},
            {&(trail_rgba.gi), .link_f = &(trail_rgba.gf)},
            {&(trail_rgba.bi), .link_f = &(trail_rgba.bf)},
            {&(trail_rgba.ai), .link_f = &(trail_rgba.af)}
        };
        
        // * Draw Preview trail
        for (int i = 0; i < length + (2 * radius); i++) {
            // Calculate colours
            getTrailRGBA(trail_rgba.ri, trail_rgba.rf, trail_rgba.gi, trail_rgba.gf, trail_rgba.bi, trail_rgba.bf, trail_rgba.ai, trail_rgba.af, i, length + (2 * radius), &r, &g, &b, &a);

            // X position and line length
            int x = (left - radius) + i;
            int line_half_length = radius;

            // Calculate round ends
            if (x < left)
                line_half_length = sqrt((radius*radius) - ((left - x) * (left - x)));
            if (x > right)
                line_half_length = sqrt((radius*radius) - ((x - right) * (x - right)));

            // Render
            lineRGBA(renderer, x, prev_y - line_half_length, x, prev_y + line_half_length, r, g, b, a);
        }

        // * Draw Rectangles        
        for (int i = 0; i <= height; i++) {
            getTrailRGBA(255, 0, 255, 0, 255, 0, 255, 0, i, height, &r, &g, &b, &a);
            lineRGBA(renderer, (spacing * 1) - (width / 2), i + top, (spacing * 1) + (width / 2), i + top, r, 0, 0, 255);
            lineRGBA(renderer, (spacing * 2) - (width / 2), i + top, (spacing * 2) + (width / 2), i + top, 0, g, 0, 255);
            lineRGBA(renderer, (spacing * 3) - (width / 2), i + top, (spacing * 3) + (width / 2), i + top, 0, 0, b, 255);
            lineRGBA(renderer, (spacing * 4) - (width / 2), i + top, (spacing * 4) + (width / 2), i + top, 255, 255, 255, a);
        }

        // * Draw Sliders
        for (int i = 0; i < 4; i++) {
            int x = spacing * (i + 1);

            // Calculate Y based on link values
            rgba[i].yi = ((top - bottom) / (float)255) * *(rgba[i].link_i) + bottom;
            rgba[i].yf = ((top - bottom) / (float)255) * *(rgba[i].link_f) + bottom;

            // Render values
            {
                char value_i[5], value_f[5];
                sprintf(value_i, "%3d", *(rgba[i].link_i));
                sprintf(value_f, "%-3d", *(rgba[i].link_f));
                stringRGBA(renderer, x - (width / 2) - 50, rgba[i].yi - 2, value_i, WHITE);
                stringRGBA(renderer, x + (width / 2) + 27, rgba[i].yf - 2, value_f, WHITE);
            }

            // Render sliders
            int triangle_side = 20;
            {
                aatrigonRGBA(renderer, x - (width / 2), rgba[i].yi, x - (width / 2) - triangle_side, rgba[i].yi + (triangle_side / 2), x - (width / 2) - triangle_side, rgba[i].yi - (triangle_side / 2), WHITE);
                aatrigonRGBA(renderer, x + (width / 2), rgba[i].yf, x + (width / 2) + triangle_side, rgba[i].yf + (triangle_side / 2), x + (width / 2) + triangle_side, rgba[i].yf - (triangle_side / 2), WHITE);
                filledTrigonRGBA(renderer, x - (width / 2), rgba[i].yi, x - (width / 2) - triangle_side, rgba[i].yi + (triangle_side / 2), x - (width / 2) - triangle_side, rgba[i].yi - (triangle_side / 2), WHITE);
                filledTrigonRGBA(renderer, x + (width / 2), rgba[i].yf, x + (width / 2) + triangle_side, rgba[i].yf + (triangle_side / 2), x + (width / 2) + triangle_side, rgba[i].yf - (triangle_side / 2), WHITE);
                lineRGBA(renderer, x - (width / 2), rgba[i].yi    , x + (width / 2), rgba[i].yi    , WHITE);
                lineRGBA(renderer, x - (width / 2), rgba[i].yi + 1, x + (width / 2), rgba[i].yi + 1, BLACK);
                lineRGBA(renderer, x - (width / 2), rgba[i].yf    , x + (width / 2), rgba[i].yf    , WHITE);
                lineRGBA(renderer, x - (width / 2), rgba[i].yf + 1, x + (width / 2), rgba[i].yf + 1, BLACK);
            }

            // Sliders clicked
            int y_bound = mouse.down_y >= top && mouse.down_y <= bottom;
            if ((mouse.down && mouse.down_x >= x - (width / 2) - triangle_side && mouse.down_x <= x - (width / 2) && y_bound) || rgba[i].selected_i) {
                rgba[i].selected_i = 1;
                if (mouse.y <= bottom && mouse.y >= top) // Change link value based of mouse Y
                    *(rgba[i].link_i) = ((float)255 / (top - bottom)) * (mouse.y - bottom);
            }
            if ((mouse.down && mouse.down_x <= x + (width / 2) + triangle_side && mouse.down_x >= x + (width / 2) && y_bound) || rgba[i].selected_f) {
                rgba[i].selected_f = 1;
                if (mouse.y <= bottom && mouse.y >= top) // Change link value based of mouse Y
                    *(rgba[i].link_f) = ((float)255 / (top - bottom)) * (mouse.y - bottom);
            }

            // Unselect
            if (!mouse.down) {
                rgba[i].selected_i = 0;
                rgba[i].selected_f = 0;
            }
        }
    }
}

void getTrailRGBA(int ri, int rf, int gi, int gf, int bi, int bf, int ai, int af, int pos, int length, int *r, int *g, int *b, int *a)
{
    *r = ((rf - ri) / (float)length) * pos + ri;
    *g = ((gf - gi) / (float)length) * pos + gi;
    *b = ((bf - bi) / (float)length) * pos + bi;
    *a = ((af - ai) / (float)length) * pos + ai;
    return;
}

void setCurrentAttractor(enum StrangeAttractorType newAttractorType)
{
    // When `newAttractorType` is set as `currentAttractorType`
    // this function will simply restart the current Attractor

    // Free old model
    freeModel(currentAttractor->trail.head);
    currentAttractor->trail.head = NULL;

    // Save old default in old attractor model
    *currentAttractor = defaultModel;

    // Change the current attractor type to the new type
    currentAttractorType = newAttractorType;

    // Set new model as current attractor
    currentAttractor = &(AttractorModels[currentAttractorType]);

    // Save new attractor's default
    defaultModel = *currentAttractor;

    // Initialize model as the new model
    initializeModel();

    return;
}

// * ATTRACTOR FUNCTIONS

void BanlueAttractor(struct Point *newPoint)
{
    float x = currentAttractor->trail.tail->x;
    float y = currentAttractor->trail.tail->y;
    float z = currentAttractor->trail.tail->z;
    double a = currentAttractor->parameters.a;
    double dt = currentAttractor->dtime;

    newPoint->x = x + (y - x) * dt;
    newPoint->y = y + (-z * tanh(x)) * dt;
    newPoint->z = z + (-a + (x * y) + abs(y)) * dt;
    return;
}

void LorenzAttractor(struct Point *newPoint)
{
    float x = currentAttractor->trail.tail->x;
    float y = currentAttractor->trail.tail->y;
    float z = currentAttractor->trail.tail->z;
    double a = currentAttractor->parameters.a;
    double b = currentAttractor->parameters.b;
    double c = currentAttractor->parameters.c;
    double dt = currentAttractor->dtime;

    newPoint->x = x + (a * (y - x)) * dt;
    newPoint->y = y + (x * (b - z) - y) * dt;
    newPoint->z = z + (x * y - c * z) * dt;
    return;
}

void HalvorsenAttractor(struct Point *newPoint)
{
    float x = currentAttractor->trail.tail->x;
    float y = currentAttractor->trail.tail->y;
    float z = currentAttractor->trail.tail->z;
    double a = currentAttractor->parameters.a;
    double dt = currentAttractor->dtime;

    newPoint->x = x + (-a*x - 4*y - 4*z - y*y) * dt;
    newPoint->y = y + (-a*y - 4*z - 4*x - z*z) * dt;
    newPoint->z = z + (-a*z - 4*x - 4*y - x*x) * dt;
    return;
}

void AizawaAttractor(struct Point *newPoint)
{
    float x = currentAttractor->trail.tail->x;
    float y = currentAttractor->trail.tail->y;
    float z = currentAttractor->trail.tail->z;
    double a = currentAttractor->parameters.a;
    double b = currentAttractor->parameters.b;
    double c = currentAttractor->parameters.c;
    double dt = currentAttractor->dtime;

    newPoint->x = x + ((z - 0.7) * x - c*y) * dt;
    newPoint->y = y + (c * x + (z - 0.7) * y) * dt;
    newPoint->z = z + (0.6 + b*z - ((z*z*z) / 3) - (x*x + y*y)*(1 + a*z) + 0.1*z*x*x*x) * dt;
    return;
}

void LuChenAttractor(struct Point *newPoint)
{
    float x = currentAttractor->trail.tail->x;
    float y = currentAttractor->trail.tail->y;
    float z = currentAttractor->trail.tail->z;
    double a = currentAttractor->parameters.a;
    double b = currentAttractor->parameters.b;
    double c = currentAttractor->parameters.c;
    double dt = currentAttractor->dtime;

    newPoint->x = x + (-((a*b*x) / (a+b)) - y*z + c) * dt;
    newPoint->y = y + (a*y + x*z) * dt;
    newPoint->z = z + (b*z + x*y) * dt;

    // printf("(%.2f, %.2f, %.2f)\n", currentAttractor->parameters.angle_x, currentAttractor->parameters.angle_y, currentAttractor->parameters.angle_z);
    
    return;
}

void GenesioAttractor(struct Point *newPoint)
{
    float x = currentAttractor->trail.tail->x;
    float y = currentAttractor->trail.tail->y;
    float z = currentAttractor->trail.tail->z;
    double a = currentAttractor->parameters.a;
    double b = currentAttractor->parameters.b;
    double c = currentAttractor->parameters.c;
    double dt = currentAttractor->dtime;

    newPoint->x = x + (y) * dt;
    newPoint->y = y + (z) * dt;
    newPoint->z = z + (-c*x - b*y - a*z + x*x) * dt;
    return;
}
