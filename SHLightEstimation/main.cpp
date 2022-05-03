#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "LightEstimation.h"
#include <iostream>

int main() {
    
    LightEstimation application;
    
    //Initalising application
    if(!application.init())
        throw std::runtime_error("Could not initialise application");
    
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    while(!glfwWindowShouldClose(application.window.getWindowHandle())) {
        glfwPollEvents();
    }
    
    application.cleanUp();
    
    return 0;
}
