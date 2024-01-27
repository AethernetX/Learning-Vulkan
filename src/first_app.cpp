#include "first_app.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace pb{

    struct SimplePushConstantData{
        glm::mat2 transform{1.f};
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp(){
        loadGameObjects();
        createPipelineLayout();
        createPipeline();
    }

    FirstApp::~FirstApp(){
        vkDestroyPipelineLayout(pbDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run(){
        while(!pbWindow.shouldClose()){
            glfwPollEvents();           

            if(auto commandBuffer = pbRenderer.beginFrame()){

                //begin offscreen shadow pass
                //render shadow casting objects
                //end offscreen shadow pass

                pbRenderer.beginSwapChainRenderPass(commandBuffer);
                renderGameObjects(commandBuffer);
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

    void FirstApp::createPipelineLayout(){

        VkPushConstantRange pushConstantRange{
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        pushConstantRange.offset = 0,
        pushConstantRange.size = sizeof(SimplePushConstantData)};


        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if(vkCreatePipelineLayout(pbDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS){
            throw std::runtime_error("Failed to create pipeline layout");
        }
    }

    void FirstApp::createPipeline(){
        assert(pipelineLayout != nullptr && "cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        PbPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = pbRenderer.getSwapChainRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pbPipeline = std::make_unique<PbPipeline>(
            pbDevice,
            "../shaders/simple_shader.vert.spv",
            "../shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer){
        pbPipeline->bind(commandBuffer);

        for(auto& obj: gameObjects) {

            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout, 
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
                0, 
                sizeof(SimplePushConstantData), 
                &push);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }

    
    
}