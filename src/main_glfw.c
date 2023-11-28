#include "cglm/types.h"
#include "cglm/vec2.h"
#include <assert.h>
#include <stdio.h>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "allocator.h"
#include "shaders.h"

struct Entity
{
    EntityId id;
    SpriteId spriteId;

    vec2 p;
    vec2 dp;
    vec2 ddp;

    vec2 dim;
};

struct AppState
{
    Allocator *alloc;

    struct Entity player;

    bool keyLeft;
    bool keyRight;
    bool keyUp;
    bool keyDown;

    GLuint shaderProgram;
    GLuint VAO;
};

// Vertex Shader
static const char *vertexShaderSource = "#version 330 core\n"
                                        "layout (location = 0) in vec3 aPos;\n"
                                        "void main() {\n"
                                        "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                        "}";

// Fragment Shader
static const char *fragmentShaderSource = "#version 330 core\n"
                                          "out vec4 FragColor;\n"
                                          "void main() {\n"
                                          "    FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
                                          "}";

// Function to set up OpenGL state and render the triangle
void Render(struct AppState *appState)
{
    const float dt = 0.1;

    struct Entity *player = &appState->player;

    player->ddp[0] = dt*((appState->keyLeft ? -1.0f : 0.0f) + (appState->keyRight ? 1.0f : 0.0f));
    player->ddp[1] = dt*((appState->keyUp ? -1.0f : 0.0f) + (appState->keyDown ? 1.0f : 0.0f));
    
    glm_vec2_add(player->ddp, player->dp, player->dp);
    glm_vec2_add(player->dp, player->p, player->p);

    GLuint shaderProgram = appState->shaderProgram;
    GLuint VAO = appState->VAO;

    glClear(GL_COLOR_BUFFER_BIT);

    // Use the shader program
    glUseProgram(shaderProgram);

    // Draw the triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    UNUSED(mods);
    UNUSED(scancode);

    struct AppState *state = glfwGetWindowUserPointer(window);

    if (action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        bool newKeyState = action == GLFW_PRESS;
        switch (key)
        {
        case GLFW_KEY_LEFT:
            state->keyLeft = newKeyState;
            break;
        case GLFW_KEY_RIGHT:
            state->keyRight = newKeyState;
            break;
        case GLFW_KEY_UP:
            state->keyUp = newKeyState;
            break;
        case GLFW_KEY_DOWN:
            state->keyDown = newKeyState;
            break;
        }
    }
}

int main(void)
{
    struct AppState *state = MemZeroAlloc(DefaultAllocator, sizeof(*state));
    state->alloc = DefaultAllocator;

    if (!glfwInit())
    {
        return -1;
    }


    // actually, try to get the second monitor if there is one, otherwise the primary
    int monitorCount = -1;
    GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);
    GLFWmonitor *monitor;

    assert(monitorCount > 0);

    if (monitorCount > 1)
    {
        monitor = monitors[1];
    }
    else
    {
        monitor = monitors[0];
    }

    int width = glfwGetVideoMode(monitor)->width;
    int height = glfwGetVideoMode(monitor)->height;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow *window = glfwCreateWindow(width, height, "Fejnando 1", monitor, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, &state);
    glfwSetKeyCallback(window, KeyCallback);

    gladLoadGL(glfwGetProcAddress);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        glfwTerminate();
        return -1;
    }

    // Vertex data for a colored triangle
    float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

    glGenVertexArrays(1, &state->VAO);

    // Vertex Buffer Object (VBO)
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // Bind the VAO
    glBindVertexArray(state->VAO);

    // Bind the VBO and copy the vertex data to it
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    state->shaderProgram = CompileShaders(vertexShaderSource, fragmentShaderSource);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        Render(state);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &state->VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(state->shaderProgram);

    glfwTerminate();

    return 0;
}
