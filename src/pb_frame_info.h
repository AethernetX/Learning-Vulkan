#pragma once

#include "pb_camera.h"
#include "pb_game_object.h"

//lib
#include <vulkan/vulkan.h>

namespace pb {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        PbCamera &camera;
        VkDescriptorSet globalDescriptorSet;
        PbGameObject::Map &gameObjects;
    };
}