#pragma once
#include <vulkan/vulkan.h>
#include <glslang/Public/ShaderLang.h>
#include "renderer/Shader.hpp"

namespace mist {
    struct ShaderResource {
        VkDescriptorType type;
        uint32_t binding;
        uint32_t count;
        VkShaderStageFlags flags;

        ShaderResource(VkDescriptorType type, uint32_t binding, uint32_t count, VkShaderStageFlags flags) : type(type), binding(binding), count(count), flags(flags) {}
    };

    class VulkanShader : public Shader {
    public:
        VulkanShader(const std::string& path);
        VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~VulkanShader();

        VulkanShader(const VulkanShader& other) = delete;
        VulkanShader& operator=(const VulkanShader& other) = delete;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetUniformInt(const std::string& name, int value) override;
		virtual void SetUniformIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
		virtual void SetUniformVec4(const std::string& name, const glm::vec4& value) override;
		virtual void SetUniformVec3(const std::string& name, const glm::vec3& value) override;
		virtual void SetUniformVec2(const std::string& name, const glm::vec2& value) override;

		virtual const std::string& GetName() const override { return name; }
    private:
        std::string ReadFile(const std::string& path);
        std::vector<uint32_t> ConvertGLSLToSPIRV(const std::string& src, EShLanguage stage);
        std::unordered_map<EShLanguage, std::string> PreProcess(const std::string& src);
        void Compile(std::vector<uint32_t> spirv, EShLanguage stage);
        ShaderResource& GetResource(const std::string& name);
        
        std::string name;
        std::unordered_map<std::string, ShaderResource> shaderResources;
    };
}