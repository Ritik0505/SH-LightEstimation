#pragma once
#include"Window.h"
#include "vulkan/vulkan.h"

class LightEstimation{
private:
    VkInstance m_Instance = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device = VK_NULL_HANDLE;
    uint32_t m_QueueFamilyIndex;
    VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
    VkQueue m_PresentQueue = VK_NULL_HANDLE;
    uint32_t m_GraphicsQueueFamilyIndex = 0;
    uint32_t m_PresentationQueueFamilyIndex = 0;
    VkSurfaceKHR m_PresentationSurface = VK_NULL_HANDLE;
    VkExtent2D m_SwapChainExent;
    VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
    VkSemaphore m_ImageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_RenderingFinishedSemaphore = VK_NULL_HANDLE;
    VkCommandPool m_PresentQueueCommandPool = VK_NULL_HANDLE;
    VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

    
    void initVulkan();
    void displayHandles();
    void extracted(VkApplicationInfo &appInfo);

    bool CheckExtensionsAvailability(const char* extension, const std::vector<VkExtensionProperties>& availableExtensions);
    
    bool CheckPhysicalDeviceProperties(VkPhysicalDevice p_device, uint32_t& selectedGraphicsQueueFamilyIndex, uint32_t& selectedPresentationQueueFamilyIndex);
    bool createInstance();
    bool CreateLogicalDevice();
    bool GetDeviceQueue();
    bool CreatePresentationSurface();
    
public:
    Window window;
    
    bool init();
    void cleanUp();
};
