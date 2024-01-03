#pragma once

#include "pb_device.h"

#include <string>
#include <vector>

namespace pb{
    struct PipelineConfigInfo{};

    class PbPipeline {
        public:
        PbPipeline(
            PbDevice& device,
            const std::string& vertFilePath,
            const std::string& fragFilePath,
            const PipelineConfigInfo& configInfo);
        ~PbPipeline() {}

        PbPipeline(const PbPipeline&) = delete;
        void operator=(const PbPipeline&) = delete;

        static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

        private:
        static std::vector<char> readFile(const std::string& filepath);

        void createGraphicsPipeline(
            const std::string& vertFilePath,
            const std::string& fragFilePath,
            const PipelineConfigInfo& configInfo);

            void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

            PbDevice& pbDevice;
            VkPipeline graphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;  
    };
}