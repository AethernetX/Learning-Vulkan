#include "first_app.h"
#include "simple_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace pb{

    FirstApp::FirstApp(){
        loadGameObjects();
    }

    FirstApp::~FirstApp(){}

    void FirstApp::run(){
        SimpleRenderSystem SimpleRenderSystem{pbDevice, pbRenderer.getSwapChainRenderPass()};

        while(!pbWindow.shouldClose()){
            glfwPollEvents();           

            if(auto commandBuffer = pbRenderer.beginFrame()){

                //begin offscreen shadow pass
                //render shadow casting objects
                //end offscreen shadow pass

                pbRenderer.beginSwapChainRenderPass(commandBuffer);
                SimpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                pbRenderer.endSwapChainRenderPass(commandBuffer);
                pbRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(pbDevice.device());
    }

    void FirstApp::loadGameObjects(){
        std::vector<PbModel::Vertex> vertices {
            {{-0.5f, -0.5f},{1.0f,0.0f,0.0f}},
            {{-0.5f, 0.5f},{0.0f,1.0f,0.0f}},
            {{0.5f, -0.5f},{0.0f,0.0f,1.0f}},
            {{0.5f, -0.5f},{0.0f,0.0f,1.0f}},
            {{0.5f, 0.5f},{1.0f,1.0f,1.0f}},
            {{-0.5f, 0.5f},{0.0f,1.0f,0.0f}}
        };

        auto pbModel = std::make_shared<PbModel>(pbDevice, vertices);
        auto square = PbGameObject::createGameObject();
        square.model = pbModel;
        square.color = {.1f, .8f, .1f};
        square.transform2d.translation.x = 0.0f;
        square.transform2d.scale = {.5f, .5f};
        square.transform2d.rotation = .25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(square));
    }
}