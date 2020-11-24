/**
 * Example of Using OpenGL 2.x Drawing a rotating triangle
 */
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

GLFWwindow *window;

void onInit()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    // Specify the OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Construct Window
    window = glfwCreateWindow(1280, 720, "Example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    // Keyboard input
    glfwSetKeyCallback(window, key_callback);
    // Load OpenGL
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); // https://stackoverflow.com/questions/58053885/having-an-issue-with-gladloadgl-im-getting-an-error-saying-it-does-not-take
    glfwSwapInterval(1);
}

// Resize with the viewport
float cameraZoom = 1.f;
void onResize()
{
    float ratio;
    int width, height;

    // Set viewport size
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    glViewport(0, 0, width, height);

    // Setting up orthographic projection (for 2D)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-(cameraZoom*ratio), (cameraZoom*ratio), -cameraZoom, cameraZoom, -100, 100);
    //
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

float rotateDegree = 0.f;
void onUpdate()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Rotate
    glRotatef(rotateDegree, 0.f, 0.f, 1.f);
    rotateDegree += 1.f;
}

void onRender()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Start Drawing here
    glBegin(GL_TRIANGLES);
    {
        glVertex2f(-0.6f, -0.4f);
        glColor3f(1.f, 0.f, 0.f);
        //
        glVertex2f(0.6f, -0.4f);
        glColor3f(0.f, 1.f, 0.f);
        //
        glVertex2f(0.f, 0.6f);
        glColor3f(0.f, 0.f, 1.f);
    }
    glEnd();
}

int main(void)
{
    onInit();
    //
    while (!glfwWindowShouldClose(window))
    {
        onResize();
        onUpdate();
        onRender();
        //
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
