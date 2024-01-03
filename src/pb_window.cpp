#include "pb_window.h"

#include <stdexcept>

namespace pb{

    PbWindow::PbWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name}{
        initWindow();        
    }

    PbWindow::~PbWindow(){

    }

    void PbWindow::initWindow(){
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }

    void PbWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
            throw std::runtime_error("failed to create a window surface");
        }
    }
}