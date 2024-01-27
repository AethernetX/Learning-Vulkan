#include "pb_renderer.h"

#include <array>
#include <cassert>
#include <stdexcept>

namespace pb{

    PbRenderer::PbRenderer(PbWindow& window, PbDevice& device) : pbWindow{window}, pbDevice{device}{
        recreateSwapChain();
        createCommandBuffers();
    }

    PbRenderer::~PbRenderer(){ freeCommandBuffers(); }

    void PbRenderer::recreateSwapChain(){
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
    }

    void PbRenderer::createCommandBuffers(){
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

    void PbRenderer::freeCommandBuffers(){
        vkFreeCommandBuffers(
            pbDevice.device(),
            pbDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data()
        );
        commandBuffers.clear();
    }

    VkCommandBuffer PbRenderer::beginFrame(){
        assert(!isFrameStarted && "Can't call beginFrame while already in progress");
        
        auto result = pbSwapChain->acquireNextImage(&currentImageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR){
            recreateSwapChain();
            return nullptr;
        }

        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        return commandBuffer;
    }

    void PbRenderer::endFrame(){
        assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
        auto commandBuffer = getCurrentCommandBuffer();
        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS){
            throw std::runtime_error("failed to record command buffers!");
        }

        auto result = pbSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || pbWindow.wasWindowsResized()){
            pbWindow.resetWindowResizedFlag();
            recreateSwapChain();
        } else if(result != VK_SUCCESS){
            throw std::runtime_error("failed to present swap chain image!");
        }

        isFrameStarted = false;
    }

    void PbRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer){
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a dfferent frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pbSwapChain->getRenderPass();
        renderPassInfo.framebuffer = pbSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0,0};
        renderPassInfo.renderArea.extent = pbSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(pbSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(pbSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, pbSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }
 
    void PbRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer){
        assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a dfferent frame");
        vkCmdEndRenderPass(commandBuffer);
    }
    
}