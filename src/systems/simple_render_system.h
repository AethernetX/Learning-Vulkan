#pragma once

#include "pb_camera.h"
#include "pb_device.h"
#include "pb_frame_info.h"
#include "pb_pipeline.h"
#include "pb_game_object.h"

#include <memory>
#include <vector>

namespace pb{
    class SimpleRenderSystem{
        public:

        SimpleRenderSystem(PbDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;
        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void renderGameObjects(FrameInfo &frameInfo);

        private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        PbDevice& pbDevice;

        std::unique_ptr<PbPipeline> pbPipeline;
        VkPipelineLayout pipelineLayout;
    };
}