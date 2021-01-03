/**
 * Example of Using OpenGL 3.x Drawing a rotating triangle
 */
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "../common/Shader.h"
#include "../common/Text.h"

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

GLFWwindow *window;
GLuint vertex_buffer, vertex_shader, fragment_shader, program;
GLint vpos_location, vcol_location;

std::shared_ptr<Shader> triShader;

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
    // Load OpenGL Context
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress); // https://stackoverflow.com/questions/58053885/having-an-issue-with-gladloadgl-im-getting-an-error-saying-it-does-not-take
    glfwSwapInterval(1);

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create VAO, VBO
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    triShader = Shader::LoadShader("shaders/tri.vs", "shaders/tri.fs");

    // Get attributes locations
    vpos_location = triShader->getAttributeLocation("vPos");
    vcol_location = triShader->getAttributeLocation("vCol");

    // Send the vertex attributes
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void *)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), (void *)(sizeof(float) * 2));

    TextRenderer::Init("fonts/arial.ttf", 48);
}

float rotateDegree = 0.f;
void onUpdate()
{
    rotateDegree += 1.f;
}

int g_width, g_height;
float ratio;
glm::mat4 mvpMatrix{1.f}, projectionMatrix{1.f}, modelMatrix{1.f};
float cameraZoom = 1.f;
void onRender()
{
    glClear(GL_COLOR_BUFFER_BIT);

    modelMatrix = glm::rotate(glm::mat4{1.f}, glm::radians(rotateDegree), glm::vec3(0.f, 0.f, 1.f));
    projectionMatrix = glm::ortho(-ratio * cameraZoom, ratio * cameraZoom, -cameraZoom, cameraZoom, -1.f, 1.f);
    mvpMatrix = projectionMatrix * modelMatrix;

    triShader->use();
    triShader->setMat4("MVP", mvpMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glm::mat4 proj = glm::ortho(0.f, (float)g_width, 0.f, (float)g_height);
    TextRenderer::Begin(proj);
    TextRenderer::Text(glm::vec2{100.f, 100.f}, 1.f, glm::vec3{1.f, 0.f, 0.f}, "Hello, World");
    TextRenderer::End();
}

void onResize()
{
    glfwGetFramebufferSize(window, &g_width, &g_height);
    ratio = (float)g_width / (float)g_height;
    glViewport(0, 0, g_width, g_height);
}

int main(void)
{
    onInit();

    while (!glfwWindowShouldClose(window))
    {
        onResize();
        onUpdate();
        onRender();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
