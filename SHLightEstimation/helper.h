struct ImageParameters {
    VkImage                       Handle;
    VkImageView                   View;
    VkSampler                     Sampler;
    VkDeviceMemory                Memory;

    ImageParameters() :
        Handle(VK_NULL_HANDLE),
        View(VK_NULL_HANDLE),
        Sampler(VK_NULL_HANDLE),
        Memory(VK_NULL_HANDLE) {
    }
};

struct SwapChainParameters {
    VkSwapchainKHR                Handle;
    VkFormat                      Format;
    VkExtent2D                    Extent;

    SwapChainParameters() :
        Handle(VK_NULL_HANDLE),
        Format(VK_FORMAT_UNDEFINED),
        Extent() {
    }
}static swapChainParameters;
//
// struct VulkanCommonParameters {
//     VkInstance                    Instance;
//     VkPhysicalDevice              PhysicalDevice;
//     VkDevice                      Device;
//     QueueParameters               GraphicsQueue;
//     QueueParameters               PresentQueue;
//     VkSurfaceKHR                  PresentationSurface;
//     SwapChainParameters           SwapChain;
//
//     VulkanCommonParameters() :
//         Instance(VK_NULL_HANDLE),
//         PhysicalDevice(VK_NULL_HANDLE),
//         Device(VK_NULL_HANDLE),
//         GraphicsQueue(),
//         PresentQueue(),
//         PresentationSurface(VK_NULL_HANDLE),
//         SwapChain() {
//     }
// };

// For Holding Parameters for Memory Barrier
struct BufferTransition {
    VkBuffer buffer;
    VkAccessFlags currentAccess;    // how the buffer has been used so far
    VkAccessFlags newAccess;    // how the buffer will be used from now on
    uint32_t currentQueueFamily;
    uint32_t newQueueFamily;
};

struct ImageTransition {
    VkImage image;
    VkAccessFlags currentAccess;    // how the image has been used so far
    VkAccessFlags newAccess;    // how the image will be used from now on
    VkImageLayout currentLayout;
    VkImageLayout newLayout;
    VkImageAspectFlagBits aspect;
};

struct BufferParameters {
    VkBuffer* bufferHandle;
    VkFormat format;
    VkFormatFeatureFlags formatFeaturesBits;
    VkBufferUsageFlags usageBits;
    uint32_t size;
};

//Custom structures for Updating Descriptor Sets
struct BufferDescriptorInfo {
    VkDescriptorSet TargetDescriptorSet;
    uint32_t TargetDescriptorBindidng;
    uint32_t TargetArrayElement;
    VkDescriptorType TargetDescriptorType;
    std::vector<VkDescriptorBufferInfo> BufferInfos;
};

struct TexelBufferDescriptorInfo {
    VkDescriptorSet TargetDescriptorSet;
    uint32_t TargetDescriptorBinding;
    uint32_t TargetArrayElement;
    VkDescriptorType TargetDescriptorType;
    std::vector<VkBufferView> TexelBufferViews;
};

struct Buffer{
    bool CreateBuffer(VkPhysicalDevice& m_PhysicalDevice, VkDevice& m_Device, BufferParameters& bufferParameters, VkMemoryPropertyFlags& memoryPropertyFlags, VkDeviceMemory& memoryObject);
    bool createBuffer(VkPhysicalDevice& m_PhysicalDevice, VkDevice& m_Device, BufferParameters& bufferParameters, VkMemoryPropertyFlags& memoryPropertyFlags, VkDeviceMemory& memoryObject);
    //bool AllocateAndBindMemoryObject(VkPhysicalDevice& m_PhysicalDevice, VkDevice& m_Device, VkMemoryPropertyFlags& memoryProperties, VkBuffer& buffer, VkDeviceMemory& memoryObject);
    bool AllocateAndBindMemoryObjectToImage(VkPhysicalDevice& m_PhysicalDevice, VkDevice& m_Device, VkMemoryPropertyFlags& memoryPropertyFlags, VkImage& image, VkDeviceMemory& memoryObject);
    uint32_t findMemoryType(VkPhysicalDevice& m_PhysicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    bool CreateBufferView(VkDevice& device, VkBuffer& buffer, const VkDeviceSize& size, VkBufferView& bufferView);
    bool CreateImageView(VkDevice& device, VkImage& image, VkImageViewType& viewType, VkFormat& format, VkImageAspectFlagBits& aspects, VkImageView& imageView);

    ~Buffer() {
        // destroy bufferview-> free memory-> destroy buffer;
    }
};
