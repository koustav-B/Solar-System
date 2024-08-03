#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

float zRotated = 0.0; // Planet rotation angle
float cameraDistance = 350.0; // Initial camera distance from the scene
float sunBrightness =800.0;
bool darkBackground = false; // Flag to toggle background color
float speedFactor = 0.0; // Initial speed factor

#define KEY_ESC 27
#define KEY_ZOOM_IN '+'
#define KEY_ZOOM_OUT '-'
#define KEY_BRIGHTNESS_UP ']'
#define KEY_BRIGHTNESS_DOWN '['
#define KEY_SPEED_UP '>'
#define KEY_SPEED_DOWN '<'
#define KEY_VIEW_TOP 'x'
#define KEY_VIEW_LEFT 'y'
#define KEY_VIEW_RIGHT 'z'
#define KEY_VIEW_ELEVATED 'e' // Key for elevated view
#define KEY_TOGGLE_BACKGROUND 'b' // Toggle background color key
#define STOP 's'

enum CameraView { TOP_VIEW, LEFT_VIEW, RIGHT_VIEW, ELEVATED_VIEW };
CameraView currentView = TOP_VIEW;

void reshapeFunc(int x, int y)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLdouble)x / (GLdouble)y, 1.0, 1000.0); // Adjusted near and far clipping planes for better depth perception
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, x, y);
}

void drawEllipse(float radiusX, float radiusZ, float color[])
{
    glColor3fv(color); // Set orbit color
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; ++i)
    {
        float radian = i * M_PI / 180.0f;
        glVertex3f(radiusX * cos(radian), 0.0, radiusZ * sin(radian));
    }
    glEnd();
}

void drawStars(int numStars, float distance)
{
    glColor3f(1.0, 1.0, 1.0); // White color for stars
    glPointSize(2.0); // Adjust point size for stars

    glBegin(GL_POINTS);
    for (int i = 0; i < numStars; ++i)
    {
        float x = rand() % (int)(distance * 2) - distance;
        float y = rand() % (int)(distance * 2) - distance;
        float z = rand() % (int)(distance * 2) - distance;
        glVertex3f(x, y, z);
    }
    glEnd();
}
void drawPlanet(float orbitRadiusX, float orbitRadiusZ, float planetRadius, float orbitSpeed, float planetColor[])
{
    // Set material properties for the planet
    GLfloat ambientColor[] = {planetColor[0] * 0.2, planetColor[1] * 0.2, planetColor[2] * 0.2, 1.0};
    GLfloat diffuseColor[] = {planetColor[0], planetColor[1], planetColor[2], 1.0};
    GLfloat specularColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {50.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientColor);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularColor);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

    glPushMatrix();
    float angle = zRotated * orbitSpeed * speedFactor * M_PI / 180.0;
    float x = orbitRadiusX * cos(angle);
    float z = orbitRadiusZ * sin(angle);
    glTranslatef(x, 0.0, z); // Position relative to the sun

    // Draw the planet as a wireframe sphere
    glutWireSphere(planetRadius, 20, 20); // Adjusted subdivisions for clarity

    glPopMatrix();
}



void renderText(float x, float y, const char* text, void* font, float color[3])
{
    glColor3fv(color);
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++)
    {
        glutBitmapCharacter(font, *c);
    }
}

void drawPlanetNamesWithColors()
{
    const char* planetNames[8] = {
        "Mercury",
        "Venus",
        "Earth",
        "Mars",
        "Jupiter",
        "Saturn",
        "Uranus",
        "Neptune"
    };

    float planetColors[8][3] = {
        {0.0, 0.1, 0.1},   // Mercury (dark grey)
        {1.0, 0.5, 0.0},   // Venus (orange brown)
        {0.1, 0.2, 0.8},   // Earth (blue)
        {0.8, 0.2, 0.1},   // Mars (red)
        {1.0, 0.5, 0.0},   // Jupiter (orange)
        {0.0, 0.5, 0.5},   // Saturn (bluish green)
        {0.5, 1.0, 1.0},   // Uranus (cyan)
        {0.1, 0.0, 1.0}    // Neptune (purple)
    };

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float x = 10.0;
    float y = 580.0;
    for (int i = 0; i < 8; ++i)
    {
        renderText(x, y, planetNames[i], GLUT_BITMAP_HELVETICA_18, planetColors[i]);
        y -= 20.0;
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Draw_Sun()
{
    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

// Define light position and color
    GLfloat lightPosition[] = {0.0, 100.0, 100.0, 0.0}; // Light positioned at (0, 100, 100) with infinite distance
    GLfloat lightAmbient[] = {0.2, 0.2, 0.2, 1.0};      // Subtle ambient light
    GLfloat lightDiffuse[] = {1.0, 0.9, 0.6, 1.0};      // Warmer diffuse light
    GLfloat lightSpecular[] = {1.0, 1.0, 1.0, 1.0};     // Strong white specular light

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Set material properties for the sun
    GLfloat sunAmbient[] = {1.0, 0.6, 0.0, 1.0}; // Stronger ambient light
GLfloat sunDiffuse[] = {1.0, 0.6, 0.0, 1.0}; // Stronger diffuse light
GLfloat sunSpecular[] = {1.0, 1.0, 1.0, 1.0}; // Bright white specular highlights
GLfloat sunShininess[] = {150.0}; // Increase shininess for a more intense effect

    glMaterialfv(GL_FRONT, GL_AMBIENT, sunAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sunDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sunSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, sunShininess);

    // Draw the sun as a wireframe sphere
    glColor3f(1.0, 0.6, 0.0); // Set color to orange
    glutWireSphere(4.0, 200, 200);

    // Draw sun rays
    glColor3f(1.0, 0.6, 0.0); // Set color to orange for rays
    glLineWidth(4.0); // Adjust line width for rays

    glBegin(GL_LINES);
    for (int i = 0; i < 360; i += 10)
    {
        float radian = i * M_PI / 180.0f;
        glVertex3f(0.0, 0.0, 0.0); // Center of the sun
        glVertex3f(20.0 * cos(radian), 20.0 * sin(radian), 0.0); // Ray endpoints
    }
    glEnd();
}

void drawAsteroidBelt(float orbitRadiusX, float orbitRadiusZ, int numAsteroids)
{
    glColor3f(0.5, 0.5, 0.5); // Grey color for asteroids

    for (int i = 0; i < numAsteroids; ++i)
    {
        float angle = (float)i / numAsteroids * 360.0f;
        float radian = angle * M_PI / 180.0f;
        float distance = ((rand() % 100) / 200.0f) - 0.25; // Random offset for irregularity
        float x = (orbitRadiusX + distance) * cos(radian);
        float z = (orbitRadiusZ + distance) * sin(radian);
        glPushMatrix();
        glTranslatef(x, 0.0, z);
        glutWireSphere(0.2, 5, 5); // Smaller wireframe spheres for asteroids
        glPopMatrix();
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set camera view based on user input
    switch (currentView)
    {
        case TOP_VIEW:
            gluLookAt(0.0, cameraDistance, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);
            break;
        case LEFT_VIEW:
            gluLookAt(cameraDistance, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
            break;
        case RIGHT_VIEW:
            gluLookAt(0.0, 0.0, cameraDistance, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
            break;
        case ELEVATED_VIEW:
            gluLookAt(cameraDistance, cameraDistance, cameraDistance, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
            break;
    }

    // Toggle background color based on flag
    if (darkBackground)
    {
        glClearColor(0.0, 0.0, 0.0, 1.0); // Dark background
    }
    else
    {
        glClearColor(1.0, 1.0, 1.0, 1.0); // Slightly dark blue background
    }

    Draw_Sun();

float mercuryColor[] = {0.5, 0.5, 0.5};   // Mercury (light grey)
float venusColor[] = {1.0, 0.5, 0.0};     // Venus (orange brown)
float earthColor[] = {0.1, 0.2, 0.8};     // Earth (blue)
float marsColor[] = {0.8, 0.2, 0.1};      // Mars (red)
float jupiterColor[] = {1.0, 0.5, 0.0};   // Jupiter (orange)
float saturnColor[] = {0.8, 0.7, 0.2};    // Saturn (yellowish)
float uranusColor[] = {0.5, 1.0, 1.0};   // Uranus (cyan)
float neptuneColor[] = {0.1, 0.0, 1.0};   // Neptune (blue)

    float asteroidBeltColor[] = {0.5, 0.5, 0.5}; // Asteroid belt (grey)

    // Draw orbits for planets
drawEllipse(10.0, 12.0, mercuryColor);
drawEllipse(20.0, 24.0, venusColor);
drawEllipse(30.0, 36.0, earthColor);
drawEllipse(40.0, 48.0, marsColor);
drawEllipse(55.0, 66.0, jupiterColor);
drawEllipse(70.0, 84.0, saturnColor);
drawEllipse(85.0, 102.0, uranusColor);
drawEllipse(100.0, 120.0, neptuneColor);

// Draw planets with the same colors as orbits
drawPlanet(10.0, 12.0, 0.5, 4.78, mercuryColor); // Mercury
drawPlanet(20.0, 24.0, 0.95, 3.50, venusColor); // Venus
drawPlanet(30.0, 36.0, 1.0, 2.98, earthColor); // Earth
drawPlanet(40.0, 48.0, 0.53, 2.41, marsColor); // Mars
drawPlanet(55.0, 66.0, 2.0, 1.31, jupiterColor); // Jupiter
drawPlanet(70.0, 84.0, 1.75, 0.97, saturnColor); // Saturn
drawPlanet(85.0, 102.0, 1.5, 0.68, uranusColor); // Uranus
drawPlanet(100.0, 120.0, 1.5, 0.54, neptuneColor); // Neptune


    // Draw asteroid belt
    drawAsteroidBelt(47.0, 57.0, 500); // Example parameters for asteroid belt

    drawStars(100, 500.0); // Adjusted number of stars and distance

    // Draw planet names
    drawPlanetNamesWithColors();

    glFlush();
    glutSwapBuffers();
}

void idleFunc()
{
    zRotated += 0.1; // Adjusted rotation speed for smoother animation
    display();
}

void keyboardFunc(unsigned char key, int x, int y)
{
    switch (key)
    {
        case KEY_ESC:
            exit(0);
            break;
        case KEY_ZOOM_IN:
            cameraDistance -= 10.0;
            if (cameraDistance < 50.0) // Minimum zoom limit
                cameraDistance = 50.0;
            break;
        case KEY_ZOOM_OUT:
            cameraDistance += 10.0;
            if (cameraDistance > 400.0) // Maximum zoom limit
                cameraDistance = 400.0;
            break;
        case KEY_BRIGHTNESS_UP:
            sunBrightness += 10.0;
            if (sunBrightness > 100.0) // Maximum brightness limit
                sunBrightness = 100.0;
            break;
        case KEY_BRIGHTNESS_DOWN:
            sunBrightness -= 10.0;
            if (sunBrightness < 10.0) // Minimum brightness limit
                sunBrightness = 10.0;
            break;
        case KEY_SPEED_UP:
            speedFactor += 0.1;
            break;
        case KEY_SPEED_DOWN:
            speedFactor -= 0.1;
            if (speedFactor < 0.0) // Minimum speed limit
                speedFactor = 0.0;
            break;
        case STOP:
            if(speedFactor==0){
                speedFactor=1;
            }
            else{
            speedFactor = 0;
            }
            break;
        case KEY_VIEW_TOP:
            currentView = TOP_VIEW;
            break;
        case KEY_VIEW_LEFT:
            currentView = LEFT_VIEW;
            break;
        case KEY_VIEW_RIGHT:
            currentView = RIGHT_VIEW;
            break;
        case KEY_VIEW_ELEVATED:
            currentView = ELEVATED_VIEW;
            break;
        case KEY_TOGGLE_BACKGROUND:
            darkBackground = !darkBackground;
            break;
    }
    display();
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightAmbient[] = {0.5, 0.5, 0.5, 1.0}; // Slightly brighter ambient light
    GLfloat lightDiffuse[] = {1.0, 1.0, 1.0, 1.0}; // Bright white diffuse light
    GLfloat lightSpecular[] = {1.0, 1.0, 1.0, 1.0}; // Bright white specular highlights
    GLfloat lightPosition[] = {10.0, 10.0, 10.0, 1.0}; // Light positioned at (10, 10, 10)


    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    srand(static_cast<unsigned int>(time(0)));
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Solar System Simulation Designed By Koustav Biswas & Team ");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeFunc);
    glutKeyboardFunc(keyboardFunc);
    glutIdleFunc(idleFunc);
    glutMainLoop();

    return 0;
}
