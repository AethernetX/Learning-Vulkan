#pragma once

#include "pb_device.h"
#include "pb_pipeline.h"
#include "pb_window.h"
#include "pb_game_object.h"
#include "pb_renderer.h"

#include <memory>
#include <vector>

namespace pb{
    class FirstApp{
        public:
        static constexpr int WIDTH{600};
        static constexpr int HEIGHT{600};

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp &) = delete;
        FirstApp &operator=(const FirstApp &) = delete;

        void run();

        private:
        void loadGameObjects();
        void createPipelineLayout();
        void createPipeline();
        void renderGameObjects(VkCommandBuffer commandBuffer);

        PbWindow pbWindow{WIDTH, HEIGHT, "Hello window"};
        PbDevice pbDevice{pbWindow};
        PbRenderer pbRenderer{pbWindow, pbDevice};

        std::unique_ptr<PbPipeline> pbPipeline;
        VkPipelineLayout pipelineLayout;
        std::vector<PbGameObject> gameObjects;
    };
}