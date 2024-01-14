#include "first_app.h"

#include <stdexcept>
#include <array>

namespace pb{

    FirstApp::FirstApp(){
        loadModels();
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

    void FirstApp::loadModels(){
        std::vector<PbModel::Vertex> vertices {
            {{-0.5f, -0.5f},{1.0f,0.0f,0.0f}},
            {{-0.5f, 0.5f},{0.0f,1.0f,0.0f}},
            {{0.5f, -0.5f},{0.0f,0.0f,1.0f}},
            {{0.5f, -0.5f},{0.0f,0.0f,1.0f}},
            {{0.5f, 0.5f},{1.0f,1.0f,1.0f}},
            {{-0.5f, 0.5f},{0.0f,1.0f,0.0f}}
        };

        pbModel = std::make_unique<PbModel>(pbDevice, vertices);
    }

    void FirstApp::createPipelineLayout(){
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
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
        pbSwapChain = std::make_unique<PbSwapChain>(pbDevice, extent);
        createPipeline();
    }

    void FirstApp::createPipeline(){
        auto PipelineConfig = PbPipeline::defaultPipelineConfigInfo(pbSwapChain->width(), pbSwapChain->height());
        PipelineConfig.renderPass = pbSwapChain->getRenderPass();
        PipelineConfig.pipelineLayout = pipelineLayout;
        pbPipeline = std::make_unique<PbPipeline>(
            pbDevice,
            "../shaders/simple_shader.vert.spv",
            "../shaders/simple_shader.frag.spv",
            PipelineConfig);
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
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        
        pbPipeline->bind(commandBuffers[imageIndex]);
        pbModel->bind(commandBuffers[imageIndex]);
        pbModel->draw(commandBuffers[imageIndex]);


        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS){
            throw std::runtime_error("failed to record command buffers!");
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