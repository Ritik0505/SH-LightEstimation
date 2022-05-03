#pragma once
#include"Window.h"
#include "vulkan/vulkan.h"

class LightEstimation{
private:
    VkInstance instance;
    
    void initVulkan();
    void extracted(VkApplicationInfo &appInfo);

    bool CheckExtensionsAvailability(const char* extension, const std::vector<VkExtensionProperties>& availableExtensions);
    bool createInstance();
public:
    Window window;
    
    bool init();
    void cleanUp();
};
