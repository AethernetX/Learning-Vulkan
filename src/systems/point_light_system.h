#pragma once

#include "pb_camera.h"
#include "pb_device.h"
#include "pb_frame_info.h"
#include "pb_pipeline.h"
#include "pb_game_object.h"

#include <memory>
#include <vector>

namespace pb{
    class PointLightSystem{
        public:

        PointLightSystem(PbDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem &) = delete;
        PointLightSystem &operator=(const PointLightSystem &) = delete;

        void render(FrameInfo &frameInfo);

        private:
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline(VkRenderPass renderPass);

        PbDevice& pbDevice;

        std::unique_ptr<PbPipeline> pbPipeline;
        VkPipelineLayout pipelineLayout;
    };
}