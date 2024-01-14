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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
    }

    void PbWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
            throw std::runtime_error("failed to create a window surface");
        }
    }

    void PbWindow::frameBufferResizeCallback(GLFWwindow *window, int width, int height){
        auto pbWindow = reinterpret_cast<PbWindow *>(glfwGetWindowUserPointer(window));
        pbWindow->frameBufferResized = true;
        pbWindow->width = width;
        pbWindow->height = height;
    }
}