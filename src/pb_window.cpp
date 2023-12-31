#include "pb_window.h"

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
}