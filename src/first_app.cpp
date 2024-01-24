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
        recreateSwapChain();
        createCommandBuffers();
    }

    FirstApp::~FirstApp(){
        vkDestroyPipelineLayout(pbDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run(){
        while(!pbWindow.shouldClose()){
            glfwPollEvents();
            drawFrame();            
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

    void FirstApp::recreateSwapChain(){
        auto extent = pbWindow.getExtent();
        while (extent.width == 0 || extent.height == 0){
            extent = pbWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(pbDevice.device());

        if(pbSwapChain == nullptr){
            pbSwapChain = std::make_unique<PbSwapChain>(pbDevice, extent);
        } else {
            pbSwapChain = std::make_unique<PbSwapChain>(pbDevice, extent, std::move(pbSwapChain));
            if(pbSwapChain->imageCount() != commandBuffers.size()){
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        // Optimisation: if render pass is compatible do nothing else
        createPipeline();
    }

    void FirstApp::createPipeline(){
        PipelineConfigInfo pipelineConfig{};
        PbPipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = pbSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pbPipeline = std::make_unique<PbPipeline>(
            pbDevice,
            "../shaders/simple_shader.vert.spv",
            "../shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void FirstApp::createCommandBuffers(){
        commandBuffers.resize(pbSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = pbDevice.getCommandPool();
        allocateInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if(vkAllocateCommandBuffers(pbDevice.device(), &allocateInfo, commandBuffers.data()) != VK_SUCCESS){
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void FirstApp::freeCommandBuffers(){
        vkFreeCommandBuffers(
            pbDevice.device(),
            pbDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data()
        );
        commandBuffers.clear();
    }

    void FirstApp::recordCommandBuffer(int imageIndex){

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pbSwapChain->getRenderPass();
        renderPassInfo.framebuffer = pbSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0,0};
        renderPassInfo.renderArea.extent = pbSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(pbSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(pbSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, pbSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        renderGameObjects(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS){
            throw std::runtime_error("failed to record command buffers!");
        }


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

    void FirstApp::drawFrame(){
        uint32_t imageIndex;
        auto result = pbSwapChain->acquireNextImage(&imageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR){
            recreateSwapChain();
            return;
        }

        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        recordCommandBuffer(imageIndex);
        result = pbSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || pbWindow.wasWindowsResized()){
            pbWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        if(result != VK_SUCCESS){
            throw std::runtime_error("failed to present swap chain image!");
        }
    }
    
}