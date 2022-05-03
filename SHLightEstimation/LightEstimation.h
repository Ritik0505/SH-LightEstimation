#pragma once
#include"Window.h"
#include "vulkan/vulkan.h"
#include "helper.h"
#include<vector>

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

    struct {
            VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
            VkCommandPool commandpool = VK_NULL_HANDLE;
            VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
            VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
            VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
            VkPipeline pipeline = VK_NULL_HANDLE;
            VkSemaphore semaphore = VK_NULL_HANDLE;
        }graphics;

        struct {
            VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
            VkPipeline pipeline = VK_NULL_HANDLE;
            VkCommandPool commandpool = VK_NULL_HANDLE;
            VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
            VkSemaphore semaphore = VK_NULL_HANDLE;
        }compute;

        VkDescriptorSetLayout m_descriptorSetLayout;
        std::vector<VkDescriptorSet> m_descriptorSet;
        VkRenderPass m_RenderPass;
        std::vector<VkCommandBuffer> m_DrawCmdBuffers;
        std::vector<VkFramebuffer> m_FrameBuffers;

        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_ImageViews;
        uint32_t m_ImageIndex; //Index of Acquired Image from SwapChain
    
    void initVulkan();
    void displayHandles();
    
    bool CheckExtensionsAvailability(const char* extension, const std::vector<VkExtensionProperties>& availableExtensions);
    
    bool CheckPhysicalDeviceProperties(VkPhysicalDevice p_device, uint32_t& selectedGraphicsQueueFamilyIndex, uint32_t& selectedPresentationQueueFamilyIndex);
    bool createInstance();
    bool CreateLogicalDevice();
    bool GetDeviceQueue();
    bool CreatePresentationSurface();
    
    uint32_t GetSwapChainImages(VkSurfaceCapabilitiesKHR & surfaceCapabilites);
    VkSurfaceFormatKHR    GetSwapChainFormat(std::vector<VkSurfaceFormatKHR> & surfaceFormats);
    VkExtent2D GetSwapChainExtent(VkSurfaceCapabilitiesKHR & surfaceCapabilities);
    VkImageUsageFlags GetSwapChainUsageFlags(VkSurfaceCapabilitiesKHR & surfaceCapabilities);
    VkSurfaceTransformFlagBitsKHR GetSwapChainTransform(VkSurfaceCapabilitiesKHR & surfaceCapabilities);
    VkPresentModeKHR GetSwapChainPresentMode(std::vector<VkPresentModeKHR> & presentModes);
    
    bool CreateSwapChain();
    bool CreateSwapChainImageViews();
    
public:
    Window window;
    
    bool init();
    void cleanUp();
};
