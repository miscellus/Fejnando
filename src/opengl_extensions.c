#include "opengl_extensions.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <GL/glx.h>

// Initialize function pointers
void InitOpenGLExtensions(void) {
#ifdef __linux__
    // // Linux-specific code using GLX
    // // Replace "your_library_name" with the actual library name (e.g., "libGL.so.1")
    // void* libGL = dlopen("your_library_name", RTLD_LAZY);
    // if (!libGL) {
    //     fprintf(stderr, "Failed to open libGL\n");
    //     exit(EXIT_FAILURE);
    // }

    // Load each function pointer
#define X(name, type) \
    name = (type)glXGetProcAddress((const GLubyte*)#name); \
    if (!name) { \
        fprintf(stderr, "Failed to load OpenGL function: %s\n", #name); \
        exit(EXIT_FAILURE); \
    }
    OPENGL_FUNCTION_LIST
#undef X

    // // Close the library
    // dlclose(libGL);

#elif _WIN32
    // Windows-specific code using WGL
    HMODULE hOpenGL = LoadLibrary("opengl32.dll");
    if (!hOpenGL) {
        fprintf(stderr, "Failed to open opengl32.dll\n");
        exit(EXIT_FAILURE);
    }

    // Load each function pointer
#define X(type, name) \
    name = (type)wglGetProcAddress(#name); \
    if (!name) { \
        fprintf(stderr, "Failed to load OpenGL function: %s\n", #name); \
        exit(EXIT_FAILURE); \
    }
    OPENGL_FUNCTION_LIST
#undef X

    // Close the library
    FreeLibrary(hOpenGL);

#else
    #error "Unsupported platform"
#endif
}

