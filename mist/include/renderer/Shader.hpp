#pragma once
#include <string>
#include <Math.hpp>
#include <unordered_map>
#include "Core.hpp"

namespace mist {
	class Shader {
	public:
		virtual ~Shader() {}

		virtual void Clear() = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUniformData(const std::string& name, const void* value) = 0;

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