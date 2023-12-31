#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace pb{
    class PbWindow {
        public:
        PbWindow(int w, int h, std::string name);
        ~PbWindow();

        PbWindow(const PbWindow &) = delete;
        PbWindow &operator=(const PbWindow &) = delete; 
        
        bool shouldClose(){ return glfwWindowShouldClose(window); };

        private:
        void initWindow();
        
        const int width;
        const int height;

        std::string windowName;
        GLFWwindow *window;
    };
    
}