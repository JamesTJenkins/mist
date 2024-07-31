#include "VulkanShader.hpp"
#include <glslang/SPIRV/GlslangToSpv.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include "Debug.hpp"

namespace mist {
	static TBuiltInResource GetDefaultResources() {
		TBuiltInResource resources = {};
		resources.maxLights = 32;
		resources.maxClipPlanes = 6;
		resources.maxTextureUnits = 32;
		resources.maxTextureCoords = 32;
		resources.maxVertexAttribs = 64;
		resources.maxVertexUniformComponents = 4096;
		resources.maxVaryingFloats = 64;
		resources.maxVertexTextureImageUnits = 32;
		resources.maxCombinedTextureImageUnits = 80;
		resources.maxTextureImageUnits = 32;
		resources.maxFragmentUniformComponents = 4096;
		resources.maxDrawBuffers = 32;
		resources.maxVertexUniformVectors = 128;
		resources.maxVaryingVectors = 8;
		resources.maxFragmentUniformVectors = 16;
		resources.maxVertexOutputVectors = 16;
		resources.maxFragmentInputVectors = 15;
		resources.minProgramTexelOffset = -8;
		resources.maxProgramTexelOffset = 7;
		resources.maxClipDistances = 8;
		resources.maxComputeWorkGroupCountX = 65535;
		resources.maxComputeWorkGroupCountY = 65535;
		resources.maxComputeWorkGroupCountZ = 65535;
		resources.maxComputeWorkGroupSizeX = 1024;
		resources.maxComputeWorkGroupSizeY = 1024;
		resources.maxComputeWorkGroupSizeZ = 64;
		resources.maxComputeUniformComponents = 1024;
		resources.maxComputeTextureImageUnits = 16;
		resources.maxComputeImageUniforms = 8;
		resources.maxComputeAtomicCounters = 8;
		resources.maxComputeAtomicCounterBuffers = 1;
		resources.maxVaryingComponents = 60;
		resources.maxVertexOutputComponents = 64;
		resources.maxGeometryInputComponents = 64;
		resources.maxGeometryOutputComponents = 128;
		resources.maxFragmentInputComponents = 128;
		resources.maxImageUnits = 8;
		resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
		resources.maxCombinedShaderOutputResources = 8;
		resources.maxImageSamples = 0;
		resources.maxVertexImageUniforms = 0;
		resources.maxTessControlImageUniforms = 0;
		resources.maxTessEvaluationImageUniforms = 0;
		resources.maxGeometryImageUniforms = 0;
		resources.maxFragmentImageUniforms = 8;
		resources.maxCombinedImageUniforms = 8;
		resources.maxGeometryTextureImageUnits = 16;
		resources.maxGeometryOutputVertices = 256;
		resources.maxGeometryTotalOutputComponents = 1024;
		resources.maxGeometryUniformComponents = 1024;
		resources.maxGeometryVaryingComponents = 64;
		resources.maxTessControlInputComponents = 128;
		resources.maxTessControlOutputComponents = 128;
		resources.maxTessControlTextureImageUnits = 16;
		resources.maxTessControlUniformComponents = 1024;
		resources.maxTessControlTotalOutputComponents = 4096;
		resources.maxTessEvaluationInputComponents = 128;
		resources.maxTessEvaluationOutputComponents = 128;
		resources.maxTessEvaluationTextureImageUnits = 16;
		resources.maxTessEvaluationUniformComponents = 1024;
		resources.maxTessPatchComponents = 120;
		resources.maxPatchVertices = 32;
		resources.maxTessGenLevel = 64;
		resources.maxViewports = 16;
		resources.maxVertexAtomicCounters = 0;
		resources.maxTessControlAtomicCounters = 0;
		resources.maxTessEvaluationAtomicCounters = 0;
		resources.maxGeometryAtomicCounters = 0;
		resources.maxFragmentAtomicCounters = 8;
		resources.maxCombinedAtomicCounters = 8;
		resources.maxAtomicCounterBindings = 1;
		resources.maxVertexAtomicCounterBuffers = 0;
		resources.maxTessControlAtomicCounterBuffers = 0;
		resources.maxTessEvaluationAtomicCounterBuffers = 0;
		resources.maxGeometryAtomicCounterBuffers = 0;
		resources.maxFragmentAtomicCounterBuffers = 1;
		resources.maxCombinedAtomicCounterBuffers = 1;
		resources.maxAtomicCounterBufferSize = 16384;
		resources.maxTransformFeedbackBuffers = 4;
		resources.maxTransformFeedbackInterleavedComponents = 64;
		resources.maxCullDistances = 8;
		resources.maxCombinedClipAndCullDistances = 8;
		resources.maxSamples = 4;
		resources.limits.nonInductiveForLoops = 1;
		resources.limits.whileLoops = 1;
		resources.limits.doWhileLoops = 1;
		resources.limits.generalUniformIndexing = 1;
		resources.limits.generalAttributeMatrixVectorIndexing = 1;
		resources.limits.generalVaryingIndexing = 1;
		resources.limits.generalSamplerIndexing = 1;
		resources.limits.generalVariableIndexing = 1;
		resources.limits.generalConstantMatrixVectorIndexing = 1;

		return resources;
	}

	static EShLanguage ShaderTypeFromString(const std::string& type) {
		// TODO: add all types
		if (type == "vertex" || type == "vert") return EShLangVertex;
		if (type == "fragment" || type == "frag" || type == "pixel") return EShLangFragment;
		if (type == "comp" || type == "compute") return EShLangCompute;

		MIST_ASSERT(false, "Unknown shader type, defaulting to vertex");
		return EShLangVertex;
	}

	static VkShaderStageFlagBits EShLanguageToVkStageFlags(EShLanguage stage) {
		switch (stage) {
		case EShLangVertex:         return VK_SHADER_STAGE_VERTEX_BIT;
		case EShLangTessControl:    return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case EShLangTessEvaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case EShLangGeometry:       return VK_SHADER_STAGE_GEOMETRY_BIT;
		case EShLangFragment:       return VK_SHADER_STAGE_FRAGMENT_BIT;
		case EShLangCompute:        return VK_SHADER_STAGE_COMPUTE_BIT;
		case EShLangRayGen:         return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		case EShLangIntersect:      return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		case EShLangAnyHit:         return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		case EShLangClosestHit:     return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		case EShLangMiss:           return VK_SHADER_STAGE_MISS_BIT_KHR;
		case EShLangCallable:       return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		case EShLangTask:           return VK_SHADER_STAGE_TASK_BIT_EXT;
		case EShLangMesh:           return VK_SHADER_STAGE_MESH_BIT_EXT;
		default:
			MIST_ASSERT(false, "Can not convert EShLanguage enum to Vulkan shader stage");
			return VK_SHADER_STAGE_ALL;
		}
	}

	VulkanShader::VulkanShader(const std::string& path) {
		std::string src = ReadFile(path);
		std::unordered_map<EShLanguage, std::string> shaderSources = PreProcess(src);

		glslang::InitializeProcess();
		for (std::pair<EShLanguage, std::string> src : shaderSources) {
			std::vector<uint32_t> spirv = ConvertGLSLToSPIRV(src.second, src.first);
			Compile(spirv, src.first);
		}
		glslang::FinalizeProcess();
	}

	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) {
		std::unordered_map<EShLanguage, std::string> shaderSources;
		shaderSources[EShLangVertex] = vertexSrc;
		shaderSources[EShLangFragment] = fragmentSrc;

		glslang::InitializeProcess();
		for (std::pair<EShLanguage, std::string> src : shaderSources) {
			std::vector<uint32_t> spirv = ConvertGLSLToSPIRV(src.second, src.first);
			Compile(spirv, src.first);
		}
		glslang::FinalizeProcess();
	}

	VulkanShader::~VulkanShader() {}

	std::string VulkanShader::ReadFile(const std::string& path) {
		std::string result;
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (in) {
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else {
			MIST_ERROR("Failed to open file at: {0}", path);
		}

		return result;
	}

	std::unordered_map<EShLanguage, std::string> VulkanShader::PreProcess(const std::string& src) {
		std::unordered_map<EShLanguage, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = src.find(typeToken, 0);
		while (pos != std::string::npos) {
			size_t eol = src.find_first_of("\r\n", pos);
			MIST_ASSERT(eol != std::string::npos, "Syntax Error.");

			size_t begin = pos + typeTokenLength + 1;
			std::string type = src.substr(begin, eol - begin);

			size_t nextLinePos = src.find_first_not_of("\r\n", eol);
			pos = src.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = src.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? src.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	std::vector<uint32_t> VulkanShader::ConvertGLSLToSPIRV(const std::string& src, EShLanguage stage) {
		const char* shaderStrings[1];
		shaderStrings[0] = src.c_str();

		glslang::TShader shader(stage);
		shader.setStrings(shaderStrings, 1);

		TBuiltInResource resources = GetDefaultResources();
		EShMessages messages = EShMsgDefault;

		if (!shader.parse(&resources, 100, false, messages)) {
			MIST_ASSERT(false, "Failed to parse GLSL: %s", shader.getInfoLog());
			return {};
		}

		glslang::TProgram program;
		program.addShader(&shader);

		if (!program.link(messages)) {
			MIST_ASSERT(false, "Failed to parse GLSL: %s", shader.getInfoLog());
			return {};
		}

		std::vector<uint32_t> spirv;
		glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);

		return spirv;
	}

	void VulkanShader::Compile(std::vector<uint32_t> spirv, EShLanguage stage) {
		//spirv_cross::CompilerGLSL compiler(spirv);
		//spirv_cross::ShaderResources resources = compiler.get_shader_resources();
		//
		//for (const spirv_cross::Resource& ubo : resources.uniform_buffers) {
		//	ShaderResource res;
		//	res.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//	res.binding = compiler.get_decoration(ubo.id, spv::DecorationBinding);
		//	res.count = 1;
		//	res.flags = EShLanguageToVkStageFlags(stage);
		//
		//	shaderResources[ubo.name] = res;
		//}
		//
		//for (const spirv_cross::Resource& sampled : resources.sampled_images) {
		//	ShaderResource res;
		//	res.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		//	res.binding = compiler.get_decoration(sampled.id, spv::DecorationBinding);
		//	res.count = 1;
		//	res.flags = EShLanguageToVkStageFlags(stage);
		//
		//	shaderResources[sampled.name] = res;
		//}
	}

	ShaderResource& VulkanShader::GetResource(const std::string& name) {
		return shaderResources[name];
	}

	void VulkanShader::Bind() const {}
	void VulkanShader::Unbind() const {}
	void VulkanShader::SetUniformInt(const std::string& name, int value) {}
	void VulkanShader::SetUniformIntArray(const std::string& name, int* values, uint32_t count) {}
	void VulkanShader::SetUniformMat4(const std::string& name, const glm::mat4& value) {}
	void VulkanShader::SetUniformVec4(const std::string& name, const glm::vec4& value) {}
	void VulkanShader::SetUniformVec3(const std::string& name, const glm::vec3& value) {}
	void VulkanShader::SetUniformVec2(const std::string& name, const glm::vec2& value) {}
}