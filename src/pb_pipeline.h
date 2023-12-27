#pragma once

#include <string>
#include <vector>

namespace pb{
    class pbPipeline {
        public:
        pbPipeline(const std::string& vertFilePath, const std::string& fragFilePath);

        private:
        static std::vector<char> readFile(const std::string& filepath);

        void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath);
    };
}