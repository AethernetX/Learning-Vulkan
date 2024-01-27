#pragma once

#include "pb_device.h"
#include "pb_pipeline.h"
#include "pb_game_object.h"

#include <memory>
#include <vector>

namespace pb{
    class SimpleRenderSystem{
        public:

        SimpleRenderSystem(PbDevice &device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<PbGameObject> &gameObjects);

        private:
        void createPipelineLayout();
        void createPipeline(VkRenderPass renderPass);

        PbDevice& pbDevice;

        std::unique_ptr<PbPipeline> pbPipeline;
        VkPipelineLayout pipelineLayout;
    };
}