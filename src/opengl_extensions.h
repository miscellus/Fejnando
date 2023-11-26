#ifndef OPENGL_EXTENSIONS_H
#define OPENGL_EXTENSIONS_H

#include <GL/gl.h>

// Define the list of OpenGL functions using X-macros
#define OPENGL_FUNCTION_LIST \
    X(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC) \
    X(glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC) \
    X(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC) \
    X(glGenBuffers, PFNGLGENBUFFERSPROC) \
    X(glDeleteBuffers, PFNGLDELETEBUFFERSPROC) \
    X(glBindBuffer, PFNGLBINDBUFFERPROC) \
    X(glBufferData, PFNGLBUFFERDATAPROC) \
    X(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC) \
    X(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC) \
    X(glCreateShader, PFNGLCREATESHADERPROC) \
    X(glShaderSource, PFNGLSHADERSOURCEPROC) \
    X(glCompileShader, PFNGLCOMPILESHADERPROC) \
    X(glGetShaderiv, PFNGLGETSHADERIVPROC) \
    X(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC) \
    X(glCreateProgram, PFNGLCREATEPROGRAMPROC) \
    X(glAttachShader, PFNGLATTACHSHADERPROC) \
    X(glLinkProgram, PFNGLLINKPROGRAMPROC) \
    X(glGetProgramiv, PFNGLGETPROGRAMIVPROC) \
    X(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC) \
    X(glUseProgram, PFNGLUSEPROGRAMPROC) \
    X(glDeleteShader, PFNGLDELETESHADERPROC) \
    X(glDeleteProgram, PFNGLDELETEPROGRAMPROC) \
    // END

// Declare function pointers for each OpenGL function
#define X(name, type) extern type name;
OPENGL_FUNCTION_LIST
#undef X

// Initialize function pointers
void InitOpenGLExtensions(void);

#endif // OPENGL_EXTENSIONS_H

