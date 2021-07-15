#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Display {
public:
    GLFWwindow* Window;
    Display(GLint width, GLint height, const GLchar* name);

    void Clear(float r, float g, float b, float a);
    void SwapBuffers();

    ~Display();

private:
    void WindowSettings();
};

Display::Display(GLint width, GLint height, const GLchar* name) {
    glfwInit();
    
    WindowSettings();

    Window = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (!Window) {
        std::cout << "ERROR::WINDOW::FAILED_TO_INITIALIZE" << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
    
    glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "ERROR::GLAD::FAILED_TO_INITIALIZE" << std::endl;
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
}

void Display::WindowSettings() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
}

void Display::Clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::SwapBuffers() {
    glfwSwapBuffers(Window);
}

Display::~Display() {
    glfwTerminate();
}