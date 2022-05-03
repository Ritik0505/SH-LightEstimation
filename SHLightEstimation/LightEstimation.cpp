#include "LightEstimation.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

bool LightEstimation::init(){
    window.initWindow();
    if(!createInstance())
        return false;
    if (!CreateLogicalDevice()) {
            return false;
        }
    std::cout << "called CreateLogicalDevice()" << std::endl;
    if (!GetDeviceQueue()) {
        return false;
    }
    std::cout << "called GetDeviceQueue()" << std::endl;
    
    return true;
}

void LightEstimation::cleanUp(){
    vkDestroyInstance(m_Instance, nullptr);
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

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) != VK_SUCCESS) {
            std::cout << "**FAILED TO CREATE INSTANCE**" << std::endl;
            return false;
        }
        if (m_Instance == VK_NULL_HANDLE) {
            std::cout << "**FAILED TO CREATE INSTANCE**" << std::endl;
            return false;
        }

        return true;
}

bool LightEstimation::CheckPhysicalDeviceProperties(VkPhysicalDevice p_device, uint32_t& selectedGraphicsQueueFamilyIndex, uint32_t& selectedPresentationQueueFamilyIndex)
{
    uint32_t extensionCount = 0;
    if (vkEnumerateDeviceExtensionProperties(p_device, nullptr, &extensionCount, nullptr) != VK_SUCCESS) {
        std::cout << "COULD NOT ENUMERATE DEVICE EXTENSION PROPERTIES" << p_device << std::endl;
        return false;
    }

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    if (vkEnumerateDeviceExtensionProperties(p_device, nullptr, &extensionCount, availableExtensions.data()) != VK_SUCCESS) {
        std::cout << "COULD NOT ENUMERATE DEVICE EXTENSION PROPERTIES" << p_device << std::endl;
        return false;
    }

    std::vector<const char*> requiredExtensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
    };

    for (size_t i = 0; i < requiredExtensions.size(); i++) {
        if (CheckExtensionsAvailability(requiredExtensions.at(i), availableExtensions)) {
            std::cout << "PHYSICAL DEVICE" << p_device << "DOES NOT SUPPORT EXTENSION \"" << requiredExtensions.at(i) << std::endl;
            return false;
        }
    }

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(p_device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(p_device, &deviceFeatures);
    
    uint32_t queueFamiliesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(p_device, &queueFamiliesCount, nullptr);
    if (!queueFamiliesCount) {
        std::cout << "Physical Device : " << p_device << "doesn't have any queue families " << std::endl;
        return false;
    }

    std::vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamiliesCount);
    std::vector<VkBool32> queuePresentSupport(queueFamiliesCount);

    vkGetPhysicalDeviceQueueFamilyProperties(p_device, &queueFamiliesCount, queueFamiliesProperties.data());

    uint32_t graphicsQueuefamilyIndex = UINT32_MAX;
    uint32_t presentQueueFamilyIndex = UINT32_MAX;

    for (uint32_t i = 0; i < queueFamiliesCount; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(p_device, i, m_PresentationSurface, &queuePresentSupport[i]);
        if ((queueFamiliesProperties[i].queueCount > 0) &&
            (queueFamiliesProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            //Selecting queue that support graphics first
            if (graphicsQueuefamilyIndex == UINT32_MAX) {
                graphicsQueuefamilyIndex = i;
            }

            //Selecting Queue that supports both graphics and presentation (if there is any)
            if (queuePresentSupport[i]) {
                selectedGraphicsQueueFamilyIndex = i;
                selectedPresentationQueueFamilyIndex = i;
                return true;
            }
        }
    }

    //If there is no queue that supports both graphics and presentation we have to select separate presentation queue
    for (uint32_t i = 0; i < queueFamiliesCount; i++) {
        if (queuePresentSupport[i]) {
            presentQueueFamilyIndex = i;
            break;
        }
    }

    if (graphicsQueuefamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX) {
        std::cout << "Could not find queue family with required properties on physical device " << p_device << "!" << std::endl;
        return false;
    }

    selectedGraphicsQueueFamilyIndex = graphicsQueuefamilyIndex;
    selectedPresentationQueueFamilyIndex = presentQueueFamilyIndex;
    return true;
}

bool LightEstimation::CreateLogicalDevice()
{
    uint32_t deviceCount = 0;
    if ((vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr)) != VK_SUCCESS) {
        std::cout << "ERROR OCURRED DURING PHYSICAL DEVICE ENUMERATION " << std::endl;
        return false;
    }

    std::vector<VkPhysicalDevice> PhysicalDevices(deviceCount);
    if ((vkEnumeratePhysicalDevices(m_Instance, &deviceCount, PhysicalDevices.data())) != VK_SUCCESS) {
        std::cout << "ERROR OCURRED DURING PHYSICAL DEVICE ENUMERATION " << std::endl;
        return false;
    }

    uint32_t selectedGraphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t selectedPresentQueueFamilyIndex = UINT32_MAX;

    for (auto device : PhysicalDevices) {
        if (CheckPhysicalDeviceProperties(device, selectedGraphicsQueueFamilyIndex, selectedPresentQueueFamilyIndex)) {
            m_PhysicalDevice = device;
            break;
        }
    }

    if (m_PhysicalDevice == VK_NULL_HANDLE) {
        std::cout << "**COULD NOT SELECT PHYSICAL DEVICE OF SELECTED PROPERTIES**" << std::endl;
        return false;
    }

    std::vector<float> queuePriorites = { 1.0f };
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfo;
    queueCreateInfo.push_back({
    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    nullptr,
    0,
    selectedGraphicsQueueFamilyIndex,
    static_cast<uint32_t>(queuePriorites.size()),
    queuePriorites.data(),
        });

    if (selectedGraphicsQueueFamilyIndex != selectedPresentQueueFamilyIndex) {
        queueCreateInfo.push_back({
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        nullptr,
        0,
        selectedPresentQueueFamilyIndex,
        static_cast<uint32_t>(queuePriorites.size()),
        queuePriorites.data(),
            });
    }

    std::vector<const char*> requiredExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfo.data();
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
    deviceCreateInfo.pEnabledFeatures = nullptr;

    if (vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device) != VK_SUCCESS) {
        std::cout << "**COULD NOT CREATE LOGICAL DEVICE**" << std::endl;
        return false;
    }

    m_GraphicsQueueFamilyIndex = selectedGraphicsQueueFamilyIndex;
    m_PresentationQueueFamilyIndex = selectedPresentQueueFamilyIndex;

    return true;
}

bool LightEstimation::GetDeviceQueue()
{

    vkGetDeviceQueue(m_Device, m_GraphicsQueueFamilyIndex, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_PresentationQueueFamilyIndex, 0, &m_PresentQueue);
    return true;
}

bool LightEstimation::CreatePresentationSurface()
{
    if(!glfwCreateWindowSurface(m_Instance, window.getWindowHandle(), NULL, m_PresentationSurface))
        return false;
    return true;
}

uint32_t LightEstimation::GetSwapChainImages(VkSurfaceCapabilitiesKHR& surfaceCapabilites)
{
    uint32_t imageCount = surfaceCapabilites.minImageCount + 1;
    ((surfaceCapabilites.maxImageCount > 0) && (imageCount > surfaceCapabilites.maxImageCount)) ?
        imageCount = surfaceCapabilites.maxImageCount : 1;

    return uint32_t(imageCount);
}

VkSurfaceFormatKHR LightEstimation::GetSwapChainFormat(std::vector<VkSurfaceFormatKHR>& surfaceFormats)
{
    // If the list contains only one one entry with undefined format
    // It means that there are no preferred surface formats and any one can be chosen

    if ((surfaceFormats.size() == 1) && (surfaceFormats.at(0).format == VK_FORMAT_UNDEFINED)) {
        return VkSurfaceFormatKHR{ VK_FORMAT_R8G8B8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
    }

    //Else check if list contains R8 G8 B8 A8 format with Non Linear ColorSpace
    for (VkSurfaceFormatKHR& format : surfaceFormats) {
        if (format.format == VK_FORMAT_R8G8B8A8_UNORM) {
            return format;
        }
    }

    // Returning the first format from the list
    return surfaceFormats.at(0);
}

VkExtent2D LightEstimation::GetSwapChainExtent(VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
    //If width == height == -1
    //we define the size by ourselves but it must fit within defined confines
    if (surfaceCapabilities.currentExtent.width == -1) {
        VkExtent2D swapchainExtent = { 600, 600 };
        if (swapchainExtent.width < surfaceCapabilities.minImageExtent.width) {
            swapchainExtent.width = surfaceCapabilities.minImageExtent.width;
        }
        if (swapchainExtent.height < surfaceCapabilities.minImageExtent.height) {
            swapchainExtent.height = surfaceCapabilities.minImageExtent.height;
        }
        if (swapchainExtent.width > surfaceCapabilities.maxImageExtent.width) {
            swapchainExtent.width = surfaceCapabilities.maxImageExtent.width;
        }
        if (swapchainExtent.height > surfaceCapabilities.maxImageExtent.height) {
            swapchainExtent.height = surfaceCapabilities.maxImageExtent.height;
        }
        m_SwapChainExent = swapchainExtent;
        return swapchainExtent;
    }
    m_SwapChainExent = surfaceCapabilities.currentExtent;
    return surfaceCapabilities.currentExtent;
}

VkImageUsageFlags LightEstimation::GetSwapChainUsageFlags(VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
    if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        return VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    std::cout << "VK_IMAGE_USAGE_TRANSFER_DST_BIT not supported by SwapChain " << std::endl;
    std::cout << "Supported Usage Flags include: " << std::endl
        << (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT ? "VK_IMAGE_USAGE_TRANSFER_SRC_BIT\n" : "")
        << (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT ? "VK_IMAGE_USAGE_TRANSFER_DST_BIT\n" : "")
        << (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT ? "VK_IMAGE_USAGE_SAMPLED_BIT\n" : "")
        << (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT ? "VK_IMAGE_USAGE_STORAGE_BIT\n" : "")
        << (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ? "VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT\n" : "")
        << (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT ? "VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT\n" : "")
        << (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT ? "VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT\n" : "")
        << (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT ? "VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT\n" : "")
        << std::endl;

    return static_cast<VkImageUsageFlags>(-1);
}

VkSurfaceTransformFlagBitsKHR LightEstimation::GetSwapChainTransform(VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        return VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    return surfaceCapabilities.currentTransform;
}

VkPresentModeKHR LightEstimation::GetSwapChainPresentMode(std::vector<VkPresentModeKHR>& presentModes)
{
    for (VkPresentModeKHR& mode : presentModes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return mode;
        }
    }

    //This mode is always available
    for (VkPresentModeKHR& mode : presentModes) {
        if (mode == VK_PRESENT_MODE_FIFO_KHR) {
            return mode;
        }
    }

    std::cout << "EVEN FIFO_MODE NOT SUPPORTED" << std::endl;
    return static_cast<VkPresentModeKHR>(-1);
}

bool LightEstimation::CreateSwapChain()
{
    // Acquiring Surface Capabilities
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_PresentationSurface, &surfaceCapabilities) != VK_SUCCESS) {
        std::cout << "COULD NOT CHECK PRESENTATION SURFACE CAPABILITIES " << std::endl;
        return false;
    }

    // Acquiring Supported Surface Formats
    uint32_t formatsCount = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_PresentationSurface, &formatsCount, nullptr) != VK_SUCCESS) {
        std::cout << "ERROR OCCURRED DURING PRESENTATION SURFACE FORMAT ENUMERATION " << std::endl;
        return false;
    }

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatsCount);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_PresentationSurface, &formatsCount, surfaceFormats.data()) != VK_SUCCESS) {
        std::cout << "ERROR OCCURRED DURING PRESENTAION SURFACE FORMAT ENUMERATION " << std::endl;
        return false;
    }

    //Acquiring Supported Presentation Modes
    uint32_t presentModesCount = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_PresentationSurface, &presentModesCount, nullptr) != VK_SUCCESS) {
        std::cout << "ERROR OCCURRED DURING PRESENTATION SURFACE PRESENT MODES ENUMERATION " << std::endl;
        return false;
    }

    std::vector<VkPresentModeKHR> presentModes(presentModesCount);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_PresentationSurface, &presentModesCount, presentModes.data()) != VK_SUCCESS) {
        std::cout << "ERROR OCCURRED DURING PRESENTATION SURFACE PRESENT MODES ENUMERATION " << std::endl;
        return false;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = nullptr;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.surface = m_PresentationSurface;
    swapchainCreateInfo.minImageCount = GetSwapChainImages(surfaceCapabilities);

    VkSurfaceFormatKHR format = GetSwapChainFormat(surfaceFormats);
    swapchainCreateInfo.imageFormat = format.format;
    swapchainCreateInfo.imageColorSpace = format.colorSpace;

    swapchainCreateInfo.imageExtent = GetSwapChainExtent(surfaceCapabilities);

    //     imageArrayLayers  Defines the number of layers in a swap chain images(that is, views);
    //     typically this value will be one but if we want to create multiview or stereo(stereoscopic 3D) images,
    //    we can set it to some higher value.
    swapchainCreateInfo.imageArrayLayers = 1;

    swapchainCreateInfo.imageUsage = GetSwapChainUsageFlags(surfaceCapabilities);
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    swapchainCreateInfo.preTransform = GetSwapChainTransform(surfaceCapabilities);
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = GetSwapChainPresentMode(presentModes);
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = m_SwapChain;

    if (vkCreateSwapchainKHR(m_Device, &swapchainCreateInfo, nullptr, &m_SwapChain)) {
        std::cout << "COULD NOT CREATE SWAPCHAIN " << std::endl;
        return false;
    }

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
    m_SwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

    swapChainParameters.Handle = m_SwapChain;
    swapChainParameters.Extent = GetSwapChainExtent(surfaceCapabilities);
    swapChainParameters.Format = format.format;

    return true;
}

bool LightEstimation::CreateSwapChainImageViews()
{
    uint32_t size = m_SwapChainImages.size();
    m_ImageViews.resize(size);

    for (uint32_t i = 0; i < size; i++) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = m_SwapChainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = swapChainParameters.Format;
        imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY };
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image views!");
        }
    }
    return true;
}


void LightEstimation::displayHandles(){
    std::cout << "Instance Handle: " << m_Instance << std::endl;
    std::cout << "Physical Device Handle: " << m_PhysicalDevice << std::endl;
    std::cout << "Logical Device Handle: " << m_Device << std::endl;
    std::cout << "SwapChain Handle: " << m_SwapChain << std::endl;
}
