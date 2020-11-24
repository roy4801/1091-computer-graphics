/**
 * Example of Using OpenGL 3.x Drawing a rotating triangle
 */
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>

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

static const char *vertex_shader_src = R"(
#version 330 core

uniform mat4 MVP;

layout(location = 0) in vec2 vPos;
layout(location = 1) in vec3 vCol;

out vec3 v_color;

void main()
{
    gl_Position = MVP * vec4(vPos, 0.0, 1.0);
    v_color = vCol;
})";

static const char *fragment_shader_src = R"(
#version 330 core

in vec3 v_color;
out vec4 color;

void main()
{
    color = vec4(v_color, 1.0);
})";

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
GLint mvp_location, vpos_location, vcol_location;

void getShaderError(const char* tag, GLuint shaderID)
{
    int success;
	char log[512];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(shaderID, 512, nullptr, log);
		fprintf(stderr, "Shader Error: [%s] %s\n", tag, log);
	}
}

void onInit()
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    // Specify the OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

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

    // Create VAO, VBO
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Initialize the vertex shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);
    getShaderError("Vertex", vertex_shader);
    // Initialize the fragment shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);
    getShaderError("Fragment", fragment_shader);
    // Compile and link the shader
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int success;
	char log[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(program, 512, nullptr, log);
		fprintf(stderr, "Shader Error: [Linking] %s\n", log);
	}

    // Get attributes locations
    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
    // Send the vertex attributes
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void *)(sizeof(float) * 2));
}

float rotateDegree = 0.f;
void onUpdate()
{
    rotateDegree += 1.f;
}

float ratio;
glm::mat4 mvpMatrix{1.f}, projectionMatrix{1.f}, modelMatrix{1.f};
float cameraZoom = 1.f;
void onRender()
{
    glClear(GL_COLOR_BUFFER_BIT);

    modelMatrix = glm::rotate(glm::mat4{1.f}, glm::radians(rotateDegree), glm::vec3(0.f, 0.f, 1.f));
    projectionMatrix = glm::ortho(-ratio * cameraZoom, ratio * cameraZoom, -cameraZoom, cameraZoom, -1.f, 1.f);
    mvpMatrix = projectionMatrix * modelMatrix;

    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int width, height;
void onResize()
{
    glfwGetFramebufferSize(window, &width, &height);
    ratio = width / (float)height;
    glViewport(0, 0, width, height);
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
