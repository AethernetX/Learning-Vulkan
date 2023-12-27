#pragma once

#include "pb_pipeline.h"
#include "pb_window.h"

namespace pb{
    class FirstApp{
        public:
        static constexpr int WIDTH{600};
        static constexpr int HEIGHT{600};

        void run();

        private:
        pbWindow pbWindow{WIDTH, HEIGHT, "Hello window"};
        pbPipeline pipeline{"../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv"};

    };
}