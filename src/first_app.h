#pragma once

#include "pb_window.h"

namespace pb{
    class FirstApp{
        public:
        static constexpr int WIDTH{600};
        static constexpr int HEIGHT{600};

        void run();

        private:
        pbWindow pbWindow{WIDTH, HEIGHT, "Haiiii :3"};

    };
}