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
        VkExtent2D getExtent(){ return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; };
        bool wasWindowsResized(){ return frameBufferResized; }
        void resetWindowResizedFlag(){ frameBufferResized = false; };

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

        private:
        static void frameBufferResizeCallback(GLFWwindow *window, int width, int height);
        void initWindow();
        
        int width;
        int height;
        bool frameBufferResized = false;

        std::string windowName;
        GLFWwindow *window;
    };
    
}