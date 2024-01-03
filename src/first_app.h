#pragma once

#include "pb_pipeline.h"
#include "pb_window.h"
#include "pb_device.h"

namespace pb{
    class FirstApp{
        public:
        static constexpr int WIDTH{600};
        static constexpr int HEIGHT{600};

        void run();

        private:
        PbWindow pbWindow{WIDTH, HEIGHT, "Hello window"};
        PbDevice pbDevice{pbWindow};
        PbPipeline pbPipeline{pbDevice, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv", PbPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};

    };
}