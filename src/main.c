#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "common.h"

// Unity build
#include "shaders.c"

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
void render(GLuint shaderProgram, GLuint VAO)
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the shader program
    glUseProgram(shaderProgram);

    // Draw the triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

int main(void)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        return -1;
    }

    // Create a GLFW window and set OpenGL context properties
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    GLFWwindow *window = glfwCreateWindow(800, 600, "Modern OpenGL Triangle", glfwGetPrimaryMonitor(), NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

    // Vertex data for a colored triangle
    float vertices[] = {-0.6f, -0.6f, 0.0f, 0.6f, -0.6f, 0.0f, 0.0f, 0.6f, 0.0f};

    // Vertex Array Object (VAO)
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    // Vertex Buffer Object (VBO)
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // Bind the VAO
    glBindVertexArray(VAO);

    // Bind the VBO and copy the vertex data to it
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    GLuint shaderProgram = CompileShaders(vertexShaderSource, fragmentShaderSource);

    while (!glfwWindowShouldClose(window))
    {
        render(shaderProgram, VAO);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}
