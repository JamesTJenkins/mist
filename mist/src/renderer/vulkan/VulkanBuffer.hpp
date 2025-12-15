#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "renderer/Buffer.hpp"

namespace mist {
	class VulkanVertexBuffer : public VertexBuffer {
	public:
		VulkanVertexBuffer(uint32_t count);
		VulkanVertexBuffer(const std::vector<Vertex> vertices);
		virtual ~VulkanVertexBuffer() override { Clear(); }

		virtual void Clear() override;
		virtual void Bind() const override;
		virtual void SetData(const std::vector<Vertex> vertices) override;

		const VkBuffer& GetBuffer() const { return vertexBuffer; }
	private:
		VkBuffer vertexBuffer;
		VmaAllocation vertexAlloc;
	};

	class VulkanIndexBuffer : public IndexBuffer {
	public:
		VulkanIndexBuffer(std::vector<uint32_t> indices);
		virtual ~VulkanIndexBuffer() override { Clear(); }

		virtual void Clear() override;
		virtual void Bind() const override;
		virtual void SetData(const std::vector<uint32_t> indices) override;

		const VkBuffer& GetBuffer() const { return indexBuffer; }
	private:
		VkBuffer indexBuffer;
		VmaAllocation indexAlloc;
	};

	class UniformBuffer {
	public:
		UniformBuffer();
		UniformBuffer(uint32_t size, void* data);
		~UniformBuffer() { Clear(); }

		void SetData(uint32_t size, void* data);
		void Clear();

		const VkBuffer& GetBuffer() const { return uboBuffer; }
		const VkDeviceSize GetSize() const { return size; }
	private:
		VkBuffer uboBuffer;
		VmaAllocation uboAlloc;
		VkDeviceSize size;
	};
}