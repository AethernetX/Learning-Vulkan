#pragma once

#include "pb_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace pb{
    class PbModel {
        public:

        struct Vertex {
            glm::vec3 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();            
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();            
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

        };

        PbModel(PbDevice &device, const PbModel::Builder &builder);
        ~PbModel();

        PbModel(const PbModel &) = delete;
        PbModel &operator=(const PbModel &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

        private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffers(const std::vector<uint32_t> &indices);

        PbDevice &pbDevice;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
    };
}