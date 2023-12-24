#include "first_app.h"

namespace pb{
    void FirstApp::run(){
        while(!pbWindow.shouldClose()){
            glfwPollEvents();            
        }
    }
}