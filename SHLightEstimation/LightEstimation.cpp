#include "LightEstimation.h"
#include <vector>

bool LightEstimation::init(){
    window.initWindow();
    if(!createInstance())
        return false;
        
    return true;
}

void LightEstimation::cleanUp(){
    vkDestroyInstance(instance, nullptr);
    window.cleanup();
}

bool LightEstimation::CheckExtensionsAvailability(const char* extension, const std::vector<VkExtensionProperties>& availableExtensions)
{
    for (size_t i = 0; i < availableExtensions.size(); i++) {
        if (strcmp(availableExtensions.at(i).extensionName, extension) == 0) {
            return true;
        }
    }
    return false;
}

bool LightEstimation::createInstance(){
    uint32_t extensionCount = 0;
        if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr) != VK_SUCCESS) {
            std::cout << "ERROR OCCURED DURING INSTANCE EXTENSIONS ENUMERATION " << std::endl;
            return false;
        }

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        if (vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data()) != VK_SUCCESS) {
            std::cout << "ERROR OCCURED DURING INSTANCE EXTENSIONS ENUMERATION " << std::endl;
            return false;
        }

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (size_t i = 0; i < glfwExtensionCount; ++i) {
            if (!CheckExtensionsAvailability(glfwExtensions[i], availableExtensions)) {
                std::cout << "COULD NOT FIND INSTANCE EXTENSION NAMED \"" << glfwExtensions[i] << "\"!" << std::endl;
                return false;
            }
        }

        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pNext = nullptr;
        applicationInfo.pApplicationName = "ParticleSimulation";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = nullptr;
        instanceCreateInfo.flags = 0;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.ppEnabledLayerNames = nullptr;
        instanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
        instanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
            std::cout << "**FAILED TO CREATE INSTANCE**" << std::endl;
            return false;
        }
        if (instance == VK_NULL_HANDLE) {
            std::cout << "**FAILED TO CREATE INSTANCE**" << std::endl;
            return false;
        }

        return true;
}
