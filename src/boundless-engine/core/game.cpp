#include "core/game.hpp"

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <AL/al.h>

#ifdef __APPLE__ //Mac OSX has a different name for the header file
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif


void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %i %s\n", error, description);
}

namespace Boundless {

    Game::Game() {

    }

    Game::~Game() {

    }

    int Game::run() {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit()) {
            // Initialization failed
            fprintf(stderr, "Couldn't initialize glfw\n");
            return 1;
        }
        
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        GLFWwindow* window = glfwCreateWindow(1280, 720, "My Title", NULL, NULL);
        if (!window)
        {
            // Window or OpenGL context creation failed
            fprintf(stderr, "Window creations failed");
        }

        glfwMakeContextCurrent(window);


        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
            glfwTerminate();
            return -1;
        }

        while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();
        }

        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_SUCCESS;
    }

}