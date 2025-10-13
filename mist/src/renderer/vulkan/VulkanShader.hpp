#pragma once
#include <vulkan/vulkan.h>
#include <glslang/Public/ShaderLang.h>
#include <spirv_cross/spirv_cross.hpp>
#include "renderer/Shader.hpp"

namespace mist {
	struct InputShaderResource {
		uint32_t binding;
		uint32_t location;
		uint32_t offset;
		uint32_t stride;
		VkFormat format;
		VkVertexInputRate inputRate;
		VkShaderStageFlags flags;
		VkShaderModule shaderModule;
	};

	struct UBOShaderResource {
        VkDescriptorType type;
        uint32_t binding;
        uint32_t offset;
        uint32_t size;
        uint32_t count;
        VkShaderStageFlags flags;
        VkShaderModule shaderModule;
    };

	struct SampledImageShaderResources {
		VkDescriptorType type;
		uint32_t binding;
		uint32_t count;
		VkShaderStageFlags flags;
        VkShaderModule shaderModule;
	};

	class VulkanShader : public Shader {
	public:
		VulkanShader(const std::string& path);
		VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~VulkanShader();

		VulkanShader(const VulkanShader& other) = delete;
		VulkanShader& operator=(const VulkanShader& other) = delete;

		virtual void Clear() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetUniformInt(const std::string& name, int value) override;
		virtual void SetUniformIntArray(const std::string& name, int* values, uint32_t count) override;
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) override;
		virtual void SetUniformVec4(const std::string& name, const glm::vec4& value) override;
		virtual void SetUniformVec3(const std::string& name, const glm::vec3& value) override;
		virtual void SetUniformVec2(const std::string& name, const glm::vec2& value) override;

		virtual const std::string& GetName() const override { return name; }
		
		const std::unordered_map<std::string, InputShaderResource>& GetInputResources() const { return shaderInputs; }
		const std::unordered_map<std::string, UBOShaderResource>& GetUboResources() const { return shaderUbos; }
		const std::unordered_map<std::string, SampledImageShaderResources>& GetSampledImageResources() const { return shaderSampledImages; }
	private:
		std::string ReadFile(const std::string& path);
		std::vector<uint32_t> ConvertGLSLToSPIRV(const std::string& src, EShLanguage stage);
		std::unordered_map<EShLanguage, std::string> PreProcess(const std::string& src);
		uint32_t CalculateSize(const spirv_cross::Compiler& compiler, const spirv_cross::SPIRType& type);
		VkFormat GetDescriptionFormat(spirv_cross::SPIRType type);
		VkShaderModule CreateShaderModule(const std::vector<uint32_t>& spirv);
		void Compile(std::vector<uint32_t> spirv, EShLanguage stage);

		std::string name;
		// Add additional shader resources for push constants, storage, etc.
		std::unordered_map<std::string, InputShaderResource> shaderInputs;
		std::unordered_map<std::string, UBOShaderResource> shaderUbos;
		std::unordered_map<std::string, SampledImageShaderResources> shaderSampledImages;
	};
}