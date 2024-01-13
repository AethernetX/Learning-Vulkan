#include "pb_model.h"

#include <cassert>
#include <cstring>

namespace pb{ 
    PbModel::PbModel(PbDevice &device, const std::vector<Vertex> &vertices) : pbDevice{device}{
        createVertexBuffers(vertices);
    }

    PbModel::~PbModel(){
        vkDestroyBuffer(pbDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(pbDevice.device(), vertexBufferMemory, nullptr);
    }

    void PbModel::createVertexBuffers(const std::vector<Vertex> &vertices){
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        pbDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertexBuffer,
            vertexBufferMemory);

        void *data;
        vkMapMemory(pbDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(pbDevice.device(), vertexBufferMemory);
    }


    void PbModel::bind(VkCommandBuffer commandBuffer){
        VkBuffer buffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void PbModel::draw(VkCommandBuffer commandBuffer){
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }
    
    std::vector<VkVertexInputBindingDescription> PbModel::Vertex::getBindingDescriptions(){
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);   
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    //NB: the alternative for the above is as followed, where you use brace construction
    //the above is done for readability
    /*
    std::vector<VkVertexInputBindingDescription> PbModel::Vertex::getBindingDescriptions(){
        return {{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX}};
    }   
    */


    std::vector<VkVertexInputAttributeDescription> PbModel::Vertex::getAttributeDescriptions(){
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }

    //NB the above can be implemented alternatively like so
/*
    std::vector<VkVertexInputAttributeDescription> PbModel::Vertex::getAttributeDescriptions(){
        return {
            {0,0,VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position)},
            {0,0,VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)};
        }
    }
*/

    
}