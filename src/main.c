#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define NUM_PLANETS 8
#define SUN_RADIUS 1.0f
#define ORBIT_SPEED 0.01f

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    float distanceFromSun;
    float orbitSpeed;
    float angle;
    float radius;
    float color[3];
} Planet;

Planet planets[NUM_PLANETS] = {
    {3.0f, ORBIT_SPEED * 4, 0, 0.2f, {0.8f, 0.5f, 0.2f}},  // Mercúrio
    {5.0f, ORBIT_SPEED * 2.5, M_PI / 6, 0.4f, {1.0f, 0.8f, 0.4f}},  // Vênus
    {7.0f, ORBIT_SPEED * 1.7, M_PI / 4, 0.5f, {0.0f, 0.5f, 1.0f}},  // Terra
    {9.0f, ORBIT_SPEED * 1.3, M_PI / 2, 0.35f, {1.0f, 0.0f, 0.0f}}, // Marte
    {12.0f, ORBIT_SPEED * 1.1, M_PI / 1.5, 0.8f, {0.8f, 0.65f, 0.0f}}, // Júpiter
    {15.0f, ORBIT_SPEED * 0.9, M_PI, 0.7f, {0.82f, 0.7f, 0.55f}}, // Saturno
    {18.0f, ORBIT_SPEED * 0.7, M_PI / 3, 0.5f, {0.0f, 1.0f, 1.0f}}, // Urano
    {21.0f, ORBIT_SPEED * 0.5, M_PI / 2.5, 0.45f, {0.0f, 0.0f, 1.0f}} // Netuno
};

float camX = 0.0f, camY = 30.0f, camZ = 30.0f;  
float camAngleX = 0.0f, camAngleY = 0.0f;
int mouseX = 0, mouseY = 0, isMouseDown = 0;

void updatePlanets(Planet planets[]) {
    for (int i = 0; i < NUM_PLANETS; ++i) {
        planets[i].angle += planets[i].orbitSpeed;
        if (planets[i].angle > 2 * M_PI) {
            planets[i].angle -= 2 * M_PI;
        }
    }
}

void renderSphere(float radius) {
    GLUquadric *quad = gluNewQuadric();
    gluSphere(quad, radius, 32, 16);
    gluDeleteQuadric(quad);
}

void renderOrbit(float distance) {
    int numPoints = 360;
    glColor3f(0.5f, 0.5f, 0.5f); 

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < numPoints; i++) {
        float angle = i * (2 * M_PI / numPoints);
        float x = distance * cos(angle);
        float z = distance * sin(angle);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
}

void renderPlanets() {
    for (int i = 0; i < NUM_PLANETS; ++i) {
        renderOrbit(planets[i].distanceFromSun); 

        glPushMatrix();
        glRotatef(planets[i].angle * 180.0f / M_PI, 0.0f, 1.0f, 0.0f); 
        glTranslatef(planets[i].distanceFromSun, 0.0f, 0.0f); 

        glColor3fv(planets[i].color);
        renderSphere(planets[i].radius);

        glPopMatrix();
    }
}

void handleCameraInput(SDL_Event *event) {
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        isMouseDown = 1;
        mouseX = event->motion.x;
        mouseY = event->motion.y;
    }
    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
        isMouseDown = 0;
    }

    if (event->type == SDL_MOUSEMOTION && isMouseDown) {
        int deltaX = event->motion.x - mouseX;
        int deltaY = event->motion.y - mouseY;
        camAngleY += deltaX * 0.1f;
        camAngleX += deltaY * 0.1f;
        mouseX = event->motion.x;
        mouseY = event->motion.y;
    }

    if (event->type == SDL_MOUSEWHEEL) {
        if (event->wheel.y > 0) {
            camZ -= 1.0f; 
        } else {
            camZ += 1.0f; 
        }

        if (camZ < 10.0f) camZ = 10.0f; 
        if (camZ > 100.0f) camZ = 100.0f; 
    }
}

void renderBackground() {
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-50.0f, -50.0f, -50.0f);
    glVertex3f(50.0f, -50.0f, -50.0f);
    glVertex3f(50.0f, 50.0f, -50.0f);
    glVertex3f(-50.0f, 50.0f, -50.0f);
    glEnd();
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Simulação do Sistema Solar 3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext(window);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            handleCameraInput(&event);
        }

        updatePlanets(planets);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        glRotatef(camAngleX, 1.0f, 0.0f, 0.0f); 
        glRotatef(camAngleY, 0.0f, 1.0f, 0.0f); 

        gluLookAt(camX, camY, camZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        renderBackground();  

        glPushMatrix();
        glColor3f(1.0f, 1.0f, 0.0f);  
        renderSphere(SUN_RADIUS);
        glPopMatrix();

        renderPlanets();

        SDL_GL_SwapWindow(window);
        SDL_Delay(16);  
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
