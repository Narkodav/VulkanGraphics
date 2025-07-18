#pragma once
#include "../Common.h"
#include "Context.h"
#include "Device.h"

namespace Graphics {

    class Shader
    {
    public:
        enum class Type {
            Vertex,
            Fragment,
            Geometry,
            TessellationControl,
            TessellationEvaluation,
            Compute,
            Unknown
        };

    private:
        vk::ShaderModule m_shaderModule = nullptr;
        Type m_type = Type::Unknown;

        bool m_initialized = false;
    public:
        Shader() {};
        Shader(const Context& instance, const Device& device,
            const std::string& shaderPath);

        Shader(Shader&& other) noexcept {

            m_shaderModule = std::exchange(other.m_shaderModule, nullptr);
            m_type = std::exchange(other.m_type, Type::Unknown);
            m_initialized = std::exchange(other.m_initialized, false);

        };

        //moving to an initialized device is undefined behavior, destroy before moving
        Shader& operator=(Shader&& other) noexcept
        {
            if (this == &other)
                return *this;

            assert(!m_initialized && "Shader::operator=() - Shader already initialized");

            m_shaderModule = std::exchange(other.m_shaderModule, nullptr);
            m_type = std::exchange(other.m_type, Type::Unknown);
            m_initialized = std::exchange(other.m_initialized, false);

            return *this;
        };

        Shader(const Shader& other) noexcept = delete;
        Shader& operator=(const Shader& other) noexcept = delete;

        ~Shader() { assert(!m_initialized && "Shader was not destroyed!"); };

        void destroy(const Context& instance, const Device& device) {
            if (!m_initialized)
                return;

            device.getDevice().destroyShaderModule(m_shaderModule, nullptr, instance.getDispatchLoader());
#ifdef _DEBUG
            std::cout << "Destroyed Shader" << std::endl;
#endif
            m_initialized = false;
        }

        static std::vector<char> readFile(const std::string& filename);
        static vk::ShaderModule createShaderModule(const Context& instance,
            const Device& device, const std::vector<char>& code);

        const vk::ShaderModule& getModule() const { return m_shaderModule; };
        const Type& getType() const { return m_type; };

        static Shader::Type inferShaderType(const std::vector<char>& spirvCode);
    };

}