#include "first_app.h"

#include "keyboard_movement_controller.hpp"
#include "pb_camera.h"
#include "simple_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <chrono>
#include <cassert>
#include <stdexcept>

namespace pb{

    FirstApp::FirstApp(){
        loadGameObjects();
    }

    FirstApp::~FirstApp(){}

    void FirstApp::run(){
        SimpleRenderSystem SimpleRenderSystem{pbDevice, pbRenderer.getSwapChainRenderPass()};
        PbCamera camera{};
        camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        
        auto viewObject = PbGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        while(!pbWindow.shouldClose()){
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //NB: if you move whilst resizing the window, with the current implementation, the camera will jump somewhere far
            // this can be fixed by adding a max frame time variable. Only implement if necessary
            // let max frame tim be some predefined const value
            // frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZY(pbWindow.getGLFWwindow(), frameTime, viewObject);
            camera.setViewYXZ(viewObject.transform.translation, viewObject.transform.rotation);
            float aspect = pbRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
            if(auto commandBuffer = pbRenderer.beginFrame()){

                //begin offscreen shadow pass
                //render shadow casting objects
                //end offscreen shadow pass

                pbRenderer.beginSwapChainRenderPass(commandBuffer);
                SimpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                pbRenderer.endSwapChainRenderPass(commandBuffer);
                pbRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(pbDevice.device());
    }

    

    void FirstApp::loadGameObjects(){
        std::shared_ptr<PbModel> pbModel = PbModel::createModelFromFile(pbDevice, "../models/teapot_smooth.obj"); 

        auto gameObj = PbGameObject::createGameObject();
        gameObj.model = pbModel;
        gameObj.transform.translation = {.0f, .0f, 1.5f};
        gameObj.transform.scale = {.5f, .5f, .5f};
        gameObjects.push_back(std::move(gameObj));
    }
}