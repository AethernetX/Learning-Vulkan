#pragma once

#include "pb_camera.h"
#include "pb_game_object.h"

//lib
#include <vulkan/vulkan.h>

namespace pb {

    const int MAX_LIGHTS{10};

    struct PointLight {
        glm::vec4 position{}; //ignore w
        glm::vec4 color{}; // w is intensity
    };

    struct GlobalUbo {
        glm::mat4 projection{1.f};
        glm::mat4 view{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f}; // w is intensity
        PointLight PointLights[MAX_LIGHTS];
        int numLights;
    };

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        PbCamera &camera;
        VkDescriptorSet globalDescriptorSet;
        PbGameObject::Map &gameObjects;
    };
}