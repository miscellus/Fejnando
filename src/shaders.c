#include "shaders.h"
#include <glad/gl.h>

u32 CompileShaders(const char *vertexShaderSource, const char *fragmentShaderSource)
{
    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for shader compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "Vertex shader compilation error: %s\n", infoLog);
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check for shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "Fragment shader compilation error: %s\n", infoLog);
    }

    // Create and link the shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for shader program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "Shader program linking error: %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
