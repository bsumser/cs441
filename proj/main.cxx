#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "common/loadShader.hpp"


//calllback function when resizing window
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
using namespace glm;

int main(int argc, char *argv[])
{
    std::cout << "Hello World" << std::endl;

    int width = 1920;
    int height = 1080;

// Initialise GLFW
    glewExperimental = true; // Needed for core profile
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true;


    glViewport(0, 0, width, height);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to init GLEW\n");
        return -1;
    }
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,};
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    //compile shader program
    GLuint programID = LoadShaders( "./common/SimpleVertexShader.vertexshader", "./common/SimpleFragmentShader.fragmentshader" );

    double lastTime = glfwGetTime();
    float deltaTime = 0;
    int fps = 0;
    do{
        double curTime = glfwGetTime();
        // Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);

        // Draw nothing, see you in tutorial 2 !
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,(void*)0);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            printf("moving up one space\n");
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            printf("moving left one space\n");
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            printf("moving down one space\n");
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            printf("moving right one space\n");

        deltaTime = float(curTime - lastTime);
        lastTime = glfwGetTime();
        fps = 1 / deltaTime;
        printf("%d fps\n", fps);

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
       glfwWindowShouldClose(window) == 0 );

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
