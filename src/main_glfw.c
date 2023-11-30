#include "cglm/types.h"
#include "cglm/vec2.h"
#include <assert.h>
#include <stdio.h>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "common.h"
#include "allocator.h"
#include "shaders.h"
#include "game/sprites.h"

struct Entity
{
    u32 id;
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
    GLuint spriteVao;
};

// Vertex Shader
static const char *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aUV;\n"
"out vec3 vColor;\n"
"void main() {\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"    vColor = aColor;\n"
"}";

// Fragment Shader
static const char *fragmentShaderSource =
"#version 330 core\n"
"in vec3 vColor;\n"
"out vec4 FragColor;\n"
"void main() {\n"
"    FragColor = vec4(vColor.r, vColor.g, vColor.b, 1.0); //vec4(1.0, 0.5, 0.2, 1.0);\n"
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

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(appState->shaderProgram);

    glBindVertexArray(appState->spriteVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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

void OnFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
    UNUSED(window);
    glViewport(0, 0, width, height);
}

void HandleInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
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

    int minDim = width > height ? height : width;
    float aspect = 4.0f/3.0f;
    GLFWwindow *window = glfwCreateWindow(minDim*.8f, minDim*.8f / aspect, "Fejnando", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, &state);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);

    gladLoadGL(glfwGetProcAddress);

    if (!gladLoadGL(glfwGetProcAddress))
    {
        glfwTerminate();
        return -1;
    }

    state->shaderProgram = CompileShaders(vertexShaderSource, fragmentShaderSource);

    const char *crabFilePath = "../resources/crab.png";
    FILE *imageFile = fopen(crabFilePath, "rb");
    if (!imageFile)
    {
        fprintf(stderr, "Could not open file %s\n", crabFilePath);
        glfwTerminate();
        return -1;
    }

    s32 crabWidth;
    s32 crabHeight;
    s32 channels;
    s32 desiredChannels = 4;
    u8* crabData = stbi_load_from_file(imageFile, &crabWidth, &crabHeight, &channels, desiredChannels);
    fclose(imageFile);

    struct SpriteVertex
    {
        vec3 pos;
        vec3 color;
        vec2 uv;
    };

    struct SpriteVertex vertices[] = {
        { .pos = {-0.5f, -0.5f, 0.0f}, .color = {1.0f, 0.0f, 0.0f}, .uv = {0.0f, 0.0f}},
        { .pos = {-0.5f, 0.5f, 0.0f},  .color = {0.5f, 0.5f, 0.0f}, .uv = {0.0f, 1.0f}},
        { .pos = {0.5f, 0.5f, 0.0f},   .color = {0.2f, 0.8f, 0.0f}, .uv = {1.0f, 1.0f}},
        { .pos = {0.5f, 0.5f, 0.0f},   .color = {0.0f, 0.5f, 1.0f}, .uv = {1.0f, 1.0f}},
        { .pos = {0.5f, -0.5f, 0.0f},  .color = {0.0f, 0.5f, 1.0f}, .uv = {1.0f, 0.0f}},
        { .pos = {-0.5f, -0.5f, 0.0f}, .color = {0.0f, 0.5f, 1.0f}, .uv = {0.0f, 0.0f}},
    };

    glGenVertexArrays(1, &state->spriteVao);

    // Vertex Buffer Object (VBO)
    GLuint spriteVbo;
    glGenBuffers(1, &spriteVbo);

    // Bind the VAO
    glBindVertexArray(state->spriteVao);

    // Bind the VBO and copy the vertex data to it
    glBindBuffer(GL_ARRAY_BUFFER, spriteVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    GLint positionAttribLocation = glGetAttribLocation(state->shaderProgram, "aPos");
    glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *)OFFSET_OF(struct SpriteVertex, pos));
    glEnableVertexAttribArray(positionAttribLocation);

    GLint colorAttribLocation = glGetAttribLocation(state->shaderProgram, "aColor");
    glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *)OFFSET_OF(struct SpriteVertex, color));
    glEnableVertexAttribArray(colorAttribLocation);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        HandleInput(window);

        Render(state);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &state->spriteVao);
    glDeleteBuffers(1, &spriteVbo);
    glDeleteProgram(state->shaderProgram);

    glfwTerminate();

    return 0;
}
