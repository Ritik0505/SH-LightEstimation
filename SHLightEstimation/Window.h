#pragma once
#include<GLFW/glfw3.h>
#include <iostream>

class Window{
    GLFWwindow* window;
    
public:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    
    GLFWwindow* getWindowHandle(){
        return window;
    }
    
    void initWindow(){
        glfwInit();
        std::cout<<"Init Window\n";
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(WIDTH, HEIGHT, "SHLightEstimation", nullptr, nullptr);
    }
    
    void cleanup() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};
