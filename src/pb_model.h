#pragma once

#include "pb_buffer.h"
#include "pb_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace pb{
    class PbModel {
        public:

        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();            
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();            

            bool operator==(const Vertex &other) const {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};

            void loadModel(const std::string &filepath);
        };

        PbModel(PbDevice &device, const PbModel::Builder &builder);
        ~PbModel();

        PbModel(const PbModel &) = delete;
        PbModel &operator=(const PbModel &) = delete;

        static std::unique_ptr<PbModel> createModelFromFile( PbDevice &device, const std::string &filepath);

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

        private:
        void createVertexBuffers(const std::vector<Vertex> &vertices);
        void createIndexBuffer(const std::vector<uint32_t> &indices);

        PbDevice &pbDevice;

        std::unique_ptr<PbBuffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<PbBuffer> indexBuffer;
        uint32_t indexCount;
    };
}