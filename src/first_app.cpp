#include "first_app.h"

#include "keyboard_movement_controller.hpp"
#include "pb_buffer.h"
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
    //directional lighting
    //struct GlobalUbo {
        //glm::mat4 projectionView{1.f};
        //glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
    //};
    
    //point lighting
    struct GlobalUbo {
        glm::mat4 projectionView{1.f};
        glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .02f}; // w is intensity
        glm::vec3 lightPosition{-1.f};
        alignas(16) glm::vec4 lightColor{1.f}; //w is light intensity
    };
    
    FirstApp::FirstApp(){
        globalPool = 
            PbDescriptorPool::Builder(pbDevice)
                .setMaxSets(PbSwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, PbSwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();

        loadGameObjects();
    }

    FirstApp::~FirstApp(){}

    void FirstApp::run(){

        // just cause we set instance count to 1 doesn't mean we won't use it
        std::vector<std::unique_ptr<PbBuffer>> uboBuffers(PbSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++){
            uboBuffers[i] = std::make_unique<PbBuffer>(
                pbDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            uboBuffers[i]->map();
        }

        auto globalSetLayout = PbDescriptorSetLayout::Builder(pbDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();
        
        std::vector<VkDescriptorSet> globalDescriptorSets(PbSwapChain::MAX_FRAMES_IN_FLIGHT);
        for(int i = 0; i < globalDescriptorSets.size(); i++){
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            PbDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem SimpleRenderSystem{pbDevice, pbRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        
        PbCamera camera{};
        camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        
        auto viewerObject = PbGameObject::createGameObject();
        viewerObject.transform.translation.z = -2.5f;

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

            cameraController.moveInPlaneXZY(pbWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
            float aspect = pbRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);
            if(auto commandBuffer = pbRenderer.beginFrame()){
                int frameIndex = pbRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex]
                };

                // update
                GlobalUbo ubo{};      
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                //begin offscreen shadow pass
                //render shadow casting objects
                //end offscreen shadow pass

                // render
                pbRenderer.beginSwapChainRenderPass(commandBuffer);
                SimpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                pbRenderer.endSwapChainRenderPass(commandBuffer);
                pbRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(pbDevice.device());
    }

    

    void FirstApp::loadGameObjects(){
        std::shared_ptr<PbModel> pbModel = PbModel::createModelFromFile(pbDevice, "../models/teapot_smooth.obj"); 
        std::shared_ptr<PbModel> floorModel = PbModel::createModelFromFile(pbDevice, "../models/quad.obj"); 
        
        auto gameObj = PbGameObject::createGameObject();
        gameObj.model = pbModel;
        gameObj.transform.translation = {.0f, 1.f, .0f};
        gameObj.transform.scale = {.25f, .25f, .25f};
        gameObj.transform.rotation = {3.14f, .0f, .0f};
        gameObjects.push_back(std::move(gameObj));

        auto floor = PbGameObject::createGameObject();
        floor.model = floorModel;
        floor.transform.translation = {.0f, 1.f, .0f};
        floor.transform.scale = {3.f, 3.f, 3.f};
        gameObjects.push_back(std::move(floor));
    }
}