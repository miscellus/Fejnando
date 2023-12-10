#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "common.h"
#include "allocator.h"
#include "shaders.h"

struct Entity
{
    u32 id;
    vec2 position;
    vec2 velocity;
    vec2 acceleration;

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
    GLuint multiplyColorLocation;
    GLuint matModelLocation;
    GLuint matViewLocation;
    GLuint matProjectionLocation;
    GLuint tileCoordLocation;

    u32 viewMode;
};

static vec3 quadVertices[] = {
    { -1.0f, -1.0f,  0.0f },
    {  1.0f, -1.0f,  0.0f },
    {  1.0f,  1.0f,  0.0f },
    {  1.0f,  1.0f,  0.0f },
    { -1.0f,  1.0f,  0.0f },
    { -1.0f, -1.0f,  0.0f },
};

// Vertex Shader
static const char *vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aUV;\n"
"out vec4 vColor;\n"
"out vec2 vUV;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main() {\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"    vColor = vec4(1.0);\n"
"    vUV = aUV;\n"
"}";

// Fragment Shader
static const char *fragmentShaderSource =
"#version 330 core\n"
"in vec4 vColor;\n"
"in vec2 vUV;\n"
"out vec4 FragColor;\n"
"uniform vec4 multiplyColor;\n"
"uniform sampler2D textureMain;\n"
"uniform vec2 tile;\n"
"void main() {\n"
"    FragColor = texture(textureMain, (vUV + tile) * 0.125) * vColor * multiplyColor;\n"
"}";

// Function to set up OpenGL state and render the triangle
void Render(struct AppState *appState)
{
    const float dt = 0.1f;

    struct Entity *player = &appState->player;

    player->acceleration[0] = dt*((appState->keyLeft ? -1.0f : 0.0f) + (appState->keyRight ? 1.0f : 0.0f));
    player->acceleration[1] = dt*((appState->keyUp ? -1.0f : 0.0f) + (appState->keyDown ? 1.0f : 0.0f));

    glm_vec2_add(player->acceleration, player->velocity, player->velocity);
    glm_vec2_add(player->velocity, player->position, player->position);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Render
    //
    glUseProgram(appState->shaderProgram);

    // float t = sinf(glfwGetTime()*1)*0.5f + 0.5f;
    // float y = sinf(glfwGetTime()*0.7853981633974483)*0.5f + 0.5f;
    // glUniform4f(appState->multiplyColorLocation, t, y, t*y, 1.0);
    glUniform4f(appState->multiplyColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

    struct {
        vec3 pos;
        float scale;
        vec2 tile;
    } cubes[] = {
        {{ 1.0f,  1.0, 0.0}, 1.0f, {0.0f, 0.0f}},
        {{-1.5f, -1.0, 0.0}, 3.0f, {4.0f, 4.0f}},
    };

    for (u32 i = 0; i < ARRAY_LEN(cubes); ++i)
    {
        mat4 model;
        glm_mat4_identity(model);
        glm_translated(model, cubes[i].pos);
        glm_mat4_scale(model, cubes[i].scale);
        // glm_rotate(model, glm_rad(-55.0f), (vec3){1.0f, 0.0f, 0.0f});
        // glm_rotate(model, cubes[i].rotateFactor * glfwGetTime(), (vec3){0.0f, 0.0f, 1.0f});

        mat4 view;
        glm_mat4_identity(view);
        glm_translate(view, (vec3){0.0f, 0.0f, -10.0f});

        mat4 projection;
        // {
        //     float left = 0.0f;
        //     float right = 800.0f;
        //     float bottom = 0.0f;
        //     float top = 600.0f;
        //     float nearZ = 0.1f;
        //     float farZ = 1000.0f;
        //     glm_ortho(left, right, bottom, top, nearZ, farZ, projection);
        // }
        glm_perspective(glm_rad(55), 16.0f/9.0f, 0.1f, 100.0f, projection);

        // mat4s transform;
        // glm_mat4_identity(transform.raw);
        // transform = glms_translate(transform, (vec3s){.raw = {0.5f, player->position.y, 1.0f}});
        // glm_rotate_z(transform.raw, glfwGetTime(), transform.raw);

        glUniformMatrix4fv(appState->matModelLocation, 1, false, model[0]);
        glUniformMatrix4fv(appState->matViewLocation, 1, false, view[0]);
        glUniformMatrix4fv(appState->matProjectionLocation, 1, false, projection[0]);
        glUniform2f(appState->tileCoordLocation, cubes[i].tile[0], cubes[i].tile[1]);

        glBindVertexArray(appState->spriteVao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
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
        case GLFW_KEY_F3:
            if (newKeyState)
            {
                ++state->viewMode;
                glPolygonMode(GL_FRONT_AND_BACK, (state->viewMode & 1) ? GL_FILL : GL_LINE);
            }
        }
    }
}

static void OnFramebufferSizeChange(GLFWwindow* window, int width, int height)
{
    UNUSED(window);
    glViewport(0, 0, width, height);
}

static void HandleInput(GLFWwindow *window)
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

    float width = (float)glfwGetVideoMode(monitor)->width;
    float height = (float)glfwGetVideoMode(monitor)->height;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    float dim = width > height ? height : width;
    dim *= 0.8f;
    float aspect = width/height;
    GLFWwindow *window = glfwCreateWindow((s32)dim, (s32)(dim/aspect), "Fejnando", NULL, NULL);
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

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    state->shaderProgram = CompileShaders(vertexShaderSource, fragmentShaderSource);
    state->multiplyColorLocation = glGetUniformLocation(state->shaderProgram, "multiplyColor");
    state->matModelLocation = glGetUniformLocation(state->shaderProgram, "model");
    state->matViewLocation = glGetUniformLocation(state->shaderProgram, "view");
    state->matProjectionLocation = glGetUniformLocation(state->shaderProgram, "projection");
    state->tileCoordLocation = glGetUniformLocation(state->shaderProgram, "tile");

    const char *crabFilePath = "../resources/vehicles.png";
    FILE *imageFile = fopen(crabFilePath, "rb");
    if (!imageFile)
    {
        fprintf(stderr, "Could not open file %s.\n", crabFilePath);
        glfwTerminate();
        return -1;
    }

    // load crab texture
    u32 crabTexture;
    glGenTextures(1, &crabTexture);
    glBindTexture(GL_TEXTURE_2D, crabTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    s32 crabWidth;
    s32 crabHeight;
    s32 channels;
    s32 desiredChannels = 4;
    stbi_set_flip_vertically_on_load(true);
    u8* crabData = stbi_load_from_file(imageFile, &crabWidth, &crabHeight, &channels, desiredChannels);
    if (crabData)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, crabWidth, crabHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, crabData);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(crabData);
    }
    else
    {
        fprintf(stderr, "Could not load image %s.\n", crabFilePath);
        glfwTerminate();
        return -1;
    }
    fclose(imageFile);

    struct SpriteVertex
    {
        vec3 pos;
        // vec3 color;
        vec2 uv;
    };

    struct SpriteVertex vertices[] = {
        {.pos = {-1.0f,-1.0f,-1.0f}, .uv = {0.0f, 0.0f}},
        {.pos = { 1.0f,-1.0f,-1.0f}, .uv = {1.0f, 0.0f}},
        {.pos = { 1.0f, 1.0f,-1.0f}, .uv = {1.0f, 1.0f}},
        {.pos = { 1.0f, 1.0f,-1.0f}, .uv = {1.0f, 1.0f}},
        {.pos = {-1.0f, 1.0f,-1.0f}, .uv = {0.0f, 1.0f}},
        {.pos = {-1.0f,-1.0f,-1.0f}, .uv = {0.0f, 0.0f}},

        // {.pos = {-1.0f,-1.0f, 1.0f}, .uv = {0.0f,0.0f}},
        // {.pos = { 1.0f,-1.0f, 1.0f}, .uv = {1.0f,0.0f}},
        // {.pos = { 1.0f, 1.0f, 1.0f}, .uv = {1.0f,1.0f}},
        // {.pos = { 1.0f, 1.0f, 1.0f}, .uv = {1.0f,1.0f}},
        // {.pos = {-1.0f, 1.0f, 1.0f}, .uv = {0.0f,1.0f}},
        // {.pos = {-1.0f,-1.0f, 1.0f}, .uv = {0.0f,0.0f}},

        // {.pos = {-1.0f, 1.0f, 1.0f}, .uv = {0.0f,0.0f}},
        // {.pos = {-1.0f, 1.0f,-1.0f}, .uv = {1.0f,0.0f}},
        // {.pos = {-1.0f,-1.0f,-1.0f}, .uv = {1.0f,1.0f}},
        // {.pos = {-1.0f,-1.0f,-1.0f}, .uv = {1.0f,1.0f}},
        // {.pos = {-1.0f,-1.0f, 1.0f}, .uv = {0.0f,1.0f}},
        // {.pos = {-1.0f, 1.0f, 1.0f}, .uv = {0.0f,0.0f}},

        // {.pos = { 1.0f, 1.0f, 1.0f}, .uv = {0.0f,0.0f}},
        // {.pos = { 1.0f, 1.0f,-1.0f}, .uv = {1.0f,0.0f}},
        // {.pos = { 1.0f,-1.0f,-1.0f}, .uv = {1.0f,1.0f}},
        // {.pos = { 1.0f,-1.0f,-1.0f}, .uv = {1.0f,1.0f}},
        // {.pos = { 1.0f,-1.0f, 1.0f}, .uv = {0.0f,1.0f}},
        // {.pos = { 1.0f, 1.0f, 1.0f}, .uv = {0.0f,0.0f}},

        // {.pos = {-1.0f,-1.0f,-1.0f}, .uv = {0.0f,0.0f}},
        // {.pos = { 1.0f,-1.0f,-1.0f}, .uv = {1.0f,0.0f}},
        // {.pos = { 1.0f,-1.0f, 1.0f}, .uv = {1.0f,1.0f}},
        // {.pos = { 1.0f,-1.0f, 1.0f}, .uv = {1.0f,1.0f}},
        // {.pos = {-1.0f,-1.0f, 1.0f}, .uv = {0.0f,1.0f}},
        // {.pos = {-1.0f,-1.0f,-1.0f}, .uv = {0.0f,0.0f}},

        // {.pos = {-1.0f, 1.0f,-1.0f}, .uv = {0.0f,0.0f}},
        // {.pos = { 1.0f, 1.0f,-1.0f}, .uv = {1.0f,0.0f}},
        // {.pos = { 1.0f, 1.0f, 1.0f}, .uv = {1.0f,1.0f}},
        // {.pos = { 1.0f, 1.0f, 1.0f}, .uv = {1.0f,1.0f}},
        // {.pos = {-1.0f, 1.0f, 1.0f}, .uv = {0.0f,1.0f}},
        // {.pos = {-1.0f, 1.0f,-1.0f}, .uv = {0.0f,0.0f}},
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

    // GLint colorAttribLocation = glGetAttribLocation(state->shaderProgram, "aColor");
    // glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *)OFFSET_OF(struct SpriteVertex, color));
    // glEnableVertexAttribArray(colorAttribLocation);

    GLint uvAttribLocation = glGetAttribLocation(state->shaderProgram, "aUV");
    glVertexAttribPointer(uvAttribLocation, 2, GL_FLOAT, GL_FALSE, sizeof(*vertices), (void *)OFFSET_OF(struct SpriteVertex, uv));
    glEnableVertexAttribArray(uvAttribLocation);

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
