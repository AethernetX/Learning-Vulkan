#pragma once

#include "pb_device.h"
#include "pb_swap_chain.h"
#include "pb_window.h"

#include <cassert>
#include <memory>
#include <vector>

namespace pb{
    class PbRenderer{
        public:

        PbRenderer(PbWindow &window, PbDevice &device);
        ~PbRenderer();

        PbRenderer(const PbRenderer &) = delete;
        PbRenderer &operator=(const PbRenderer &) = delete;
        
        VkRenderPass getSwapChainRenderPass() const {return pbSwapChain->getRenderPass();}
        bool isFrameInProgress() const { return isFrameStarted;};

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
            return commandBuffers[currentFrameIndex];
        }

        int getFrameIndex() const {
            assert(isFrameStarted && "Cannot get frame index when frame not in progress");
            return currentFrameIndex;
        }

        VkCommandBuffer beginFrame();
        void endFrame();
        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        private:
        void createCommandBuffers();
        void freeCommandBuffers();
        void recreateSwapChain();

        PbWindow& pbWindow;
        PbDevice& pbDevice;
        std::unique_ptr<PbSwapChain> pbSwapChain;
        std::vector<VkCommandBuffer> commandBuffers;

        uint32_t currentImageIndex;
        int currentFrameIndex;
        bool isFrameStarted;
    };
}