#include <league-of-dwarves/config.hpp>
#include <league-of-dwarves/factorial.hpp>
#include <league-of-dwarves/hello_world.hpp>

#include <iostream>
#include <string>
#include <GLFW/glfw3.h>

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %i %s\n", error, description);
}

int main()
{
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


    while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);

}
