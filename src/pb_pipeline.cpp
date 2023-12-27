#include "pb_pipeline.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace pb
{

    pbPipeline::pbPipeline(const std::string& vertFilePath, const std::string& fragFilePath){
        createGraphicsPipeline(vertFilePath, fragFilePath);
    }

    std::vector<char> pbPipeline::readFile(const std::string& filepath){
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);

        if(!file.is_open()){
            throw std::runtime_error("failed to open file: " + filepath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    } 

    void pbPipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath){
        auto vertCode = readFile(vertFilePath);
        auto fragCode = readFile(fragFilePath);

        std::cout << "Vertex shader code size: " << vertCode.size() << "\n";
        std::cout << "Fragment shader code size: " << fragCode.size() << "\n";
    }

} // namespace pb
