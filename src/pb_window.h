#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace pb{
    class pbWindow {
        public:
        pbWindow(int w, int h, std::string name);
        ~pbWindow();

        pbWindow(const pbWindow &) = delete;
        pbWindow &operator=(const pbWindow &) = delete; 
        
        bool shouldClose(){ return glfwWindowShouldClose(window); };

        private:
        void initWindow();
        
        const int width;
        const int height;

        std::string windowName;
        GLFWwindow *window;
    };
    
}