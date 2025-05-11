#pragma once
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>
#include "Core.hpp"

namespace mist {
	class Shader {
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUniformInt(const std::string& name, int value) = 0;
		virtual void SetUniformIntArray(const std::string& name, int* values, uint32_t count) = 0;
		virtual void SetUniformMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void SetUniformVec4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetUniformVec3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetUniformVec2(const std::string& name, const glm::vec2& value) = 0;

		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& path);
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
	};

	class ShaderLibrary {
	public:
		void Add(const Ref<Shader>& shader);
		void Add(const std::string& name, const Ref<Shader>& shader);

		Ref<Shader> Load(const std::string& path);
		Ref<Shader> Load(const std::string& name, const std::string& path);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> shaders;
	};
}