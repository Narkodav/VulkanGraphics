#include "Shader.h"

Shader::Shader(const EngineContext& instance, const Device& device,
    const std::string& shaderPath)
{
    auto shaderCode = readFile(shaderPath);
    m_type = inferShaderType(shaderCode);
    m_shaderModule = createShaderModule(instance, device, shaderCode);

    m_initialized = true;
}

std::vector<char> Shader::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

vk::ShaderModule Shader::createShaderModule(const EngineContext& instance, 
    const Device& device, const std::vector<char>& code) {

    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    vk::ShaderModule shaderModule;

    auto result = device.getDevice().createShaderModule(
        &createInfo, nullptr, &shaderModule, instance.getDispatchLoader());

    if (result != vk::Result::eSuccess) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

Shader::Type Shader::inferShaderType(const std::vector<char>& spirvCode) {

    const uint32_t* words = reinterpret_cast<const uint32_t*>(spirvCode.data());
    size_t wordCount = spirvCode.size() / sizeof(uint32_t);

    // Verify SPIR-V magic number
    if (wordCount < 5 || words[0] != 0x07230203) {
        return Shader::Type::UNKNOWN;
    }

    // Iterate through the SPIR-V code
    for (size_t i = 5; i < wordCount;) {
        uint32_t instruction = words[i];
        uint32_t opCode = instruction & 0xFF;         // OpCode is in the lowest 8 bits
        uint32_t wordLength = instruction >> 16;      // Word count is in the upper 16 bits

        if (opCode == 15) { // OpEntryPoint
            if (i + 1 < wordCount) {
                uint32_t executionModel = words[i + 1];

                switch (executionModel) {
                case 0: return Shader::Type::VERTEX;
                case 1: return Shader::Type::TESS_CONTROL;
                case 2: return Shader::Type::TESS_EVALUATION;
                case 3: return Shader::Type::GEOMETRY;
                case 4: return Shader::Type::FRAGMENT;
                case 5: return Shader::Type::COMPUTE;
                default: return Shader::Type::UNKNOWN;
                }
            }
            else return Shader::Type::UNKNOWN;
        }

        if (wordLength == 0) { // Prevent infinite loop
            return Shader::Type::UNKNOWN;
        }

        i += wordLength; // Move to next instruction
    }

    return Shader::Type::UNKNOWN;
}
