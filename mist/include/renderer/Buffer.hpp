#pragma once
#include "Core.hpp"
#include <vector>
#include <string>

namespace mist {
	enum class ShaderDataType {
		None = 0,
		Float,
		Vec2, Vec3, Vec4,
		Mat3, Mat4,
		Int, Int2, Int3, Int4,
		Bool
	};

	static uint32_t GetDataTypeSize(ShaderDataType type) {
		switch (type) {
			case ShaderDataType::Float:	return 4;
			case ShaderDataType::Vec2:	return 4 * 2;
			case ShaderDataType::Vec3:	return 4 * 3;
			case ShaderDataType::Vec4:	return 4 * 4;
			case ShaderDataType::Mat3:	return 4 * 3 * 3;
			case ShaderDataType::Mat4:	return 4 * 4 * 4;
			case ShaderDataType::Int:	return 4;
			case ShaderDataType::Int2:	return 4 * 2;
			case ShaderDataType::Int3:	return 4 * 3;
			case ShaderDataType::Int4:	return 4 * 4;
			case ShaderDataType::Bool:	return 1;
			default: 					return 0;
		}
	}

	struct MIST_API BufferElement {
		ShaderDataType type;
		const char* name;
		uint32_t size;
		uint32_t offset;
		bool normalized;

		BufferElement() : type(ShaderDataType::None), name(""), size(0), offset(0), normalized(false) {}
		BufferElement(ShaderDataType type, const char* name, bool normalized = false) : type(type), name(name), size(GetDataTypeSize(type)), offset(0), normalized(normalized) {}

		uint32_t GetComponentCount() const {
			switch (type) {
				case ShaderDataType::Float:	return 1;
				case ShaderDataType::Vec2:	return 2;
				case ShaderDataType::Vec3:	return 3;
				case ShaderDataType::Vec4:	return 4;
				case ShaderDataType::Mat3:	return 3 * 3;
				case ShaderDataType::Mat4:	return 4 * 4;
				case ShaderDataType::Int:	return 1;
				case ShaderDataType::Int2:	return 2;
				case ShaderDataType::Int3:	return 3;
				case ShaderDataType::Int4:	return 4;
				case ShaderDataType::Bool:	return 1;
				default:					return 0;
			}
		}
	};
	
	class MIST_API BufferLayout {
	public:
		BufferLayout();
		BufferLayout(BufferElement** elements, uint32_t elementCount);

		void Push(BufferElement* element);
		void Pop(BufferElement* element);

		inline uint32_t GetStride() const { return stride; }
		inline BufferElement** Data() { return elements; }
		
		inline BufferElement** begin() { return elements; }			// Lowercase to allow for(BufferElement* element : elements) {}
		inline BufferElement** end() { return elements + size; }	// Lowercase to allow for(BufferElement* element : elements) {}
		inline uint32_t Size() const { return elementCount; }
		
		void Clear();
		void Resize(uint32_t newCapacity);
	private:
		void CalculateOffsetAndStride();
	private:
		BufferElement** elements;
		uint32_t elementCount;
		uint32_t capacity;
		uint32_t size;
		uint32_t stride;
	};

	class MIST_API VertexBuffer {
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
	
		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
	};

	class MIST_API IndexBuffer {
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}