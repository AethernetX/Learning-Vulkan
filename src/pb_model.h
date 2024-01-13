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
            glm::vec2 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();            
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();            
        };

        PbModel(PbDevice &device, const std::vector<Vertex> &vertices);
        ~PbModel();

        PbModel(const PbModel &) = delete;
        PbModel &operator=(const PbModel &) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

        private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);

        PbDevice &pbDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;
    };
}