#include "renderer/vulkan/VulkanBuffer.hpp"
#include "VulkanDebug.hpp"
#include "Log.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "Debug.hpp"

namespace mist {
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer, VmaAllocation& bufferAlloc, VmaMemoryUsage allocUsage, VmaAllocationCreateFlags allocFlags, VmaAllocationInfo& info) {
		VulkanContext& context = VulkanContext::GetContext();

		VkBufferCreateInfo bufferInfo {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocCreateInfo {};
		allocCreateInfo.usage = allocUsage;
		allocCreateInfo.flags = allocFlags;

		CheckVkResult(vmaCreateBuffer(context.GetAllocator(), &bufferInfo, &allocCreateInfo, &buffer, &bufferAlloc, &info));
	}

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		VulkanContext& context = VulkanContext::GetContext();
		context.BeginSingleTimeCommands();
		
		VkBufferCopy copyRegion {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer(context.GetTempCommandBuffer(), srcBuffer, dstBuffer, 1, &copyRegion);
		
		context.EndSingleTimeCommands();
	}
	
	void SetBufferData(const void* data, VkDeviceSize size, VkBuffer& buffer) {
		VulkanContext& context = VulkanContext::GetContext();
		
		VkBuffer stagingBuffer;
		VmaAllocation stagingAlloc;
		VmaAllocationInfo info {};
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer, stagingAlloc, VMA_MEMORY_USAGE_CPU_ONLY, VMA_ALLOCATION_CREATE_MAPPED_BIT, info);
		
		memcpy(info.pMappedData, data, size);
		vmaFlushAllocation(context.GetAllocator(), stagingAlloc, 0, size);

		CopyBuffer(stagingBuffer, buffer, size);

		vmaDestroyBuffer(context.GetAllocator(), stagingBuffer, stagingAlloc);
	}
	
	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t count) {
		const VkDeviceSize size = sizeof(float) * count;
		VmaAllocationInfo info {};
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer, vertexAlloc, VMA_MEMORY_USAGE_GPU_ONLY, 0, info);
		MIST_INFO("Created new vertex buffer but no data set");
	}
	
	VulkanVertexBuffer::VulkanVertexBuffer(std::vector<Vertex> vertices) {
		const VkDeviceSize size = sizeof(Vertex) * vertices.size();
		VmaAllocationInfo info {};
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer, vertexAlloc, VMA_MEMORY_USAGE_GPU_ONLY, 0, info);
		SetData(vertices);
		MIST_INFO("Created new vertex buffer and set data");
	}
	
	VulkanVertexBuffer::~VulkanVertexBuffer() {
		Clear();
		MIST_INFO("Destroyed vertex buffer");
	}

	VulkanVertexBuffer::VulkanVertexBuffer(VulkanVertexBuffer&& other) noexcept : vertexBuffer(other.vertexBuffer), vertexAlloc(other.vertexAlloc) {
		other.Clear();
	}

	VulkanVertexBuffer& VulkanVertexBuffer::operator=(VulkanVertexBuffer&& other) noexcept {
		if (this != &other) {
			this->Clear();
			this->vertexBuffer = other.vertexBuffer;
			this->vertexAlloc = other.vertexAlloc;
			other.Clear();
		}
		
		return *this;
	}

	void VulkanVertexBuffer::Clear() {
		VulkanContext& context = VulkanContext::GetContext();
		
		if (vertexBuffer != VK_NULL_HANDLE)
			vmaDestroyBuffer(context.GetAllocator(), vertexBuffer, vertexAlloc);
	}
	
	void VulkanVertexBuffer::Bind() const {
		VulkanContext& context = VulkanContext::GetContext();
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(context.GetCurrentFrameCommandBuffer(), 0, 1, &vertexBuffer, offsets);
	}
	
	void VulkanVertexBuffer::SetData(const std::vector<Vertex> vertices) {
		const VkDeviceSize size = sizeof(Vertex) * vertices.size();
		SetBufferData(vertices.data(), size, vertexBuffer);
	}
	
	VulkanIndexBuffer::VulkanIndexBuffer(std::vector<uint32_t> indices) {
		const VkDeviceSize size = indices.size() * sizeof(uint32_t);
		VmaAllocationInfo info {};
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer, indexAlloc, VMA_MEMORY_USAGE_GPU_ONLY, 0, info);
		SetData(indices);
		MIST_INFO("Created new index buffer and set data");
	}

	VulkanIndexBuffer::~VulkanIndexBuffer() {
		Clear();
		MIST_INFO("Destroyed index buffer");
	}

	VulkanIndexBuffer::VulkanIndexBuffer(VulkanIndexBuffer&& other) noexcept : indexBuffer(other.indexBuffer), indexAlloc(other.indexAlloc) {
		other.Clear();
	}

	VulkanIndexBuffer& VulkanIndexBuffer::operator=(VulkanIndexBuffer&& other) noexcept {
		if (this != &other) {
			this->Clear();
			this->indexBuffer = other.indexBuffer;
			this->indexAlloc = other.indexAlloc;
			other.Clear();
		}
		
		return *this;
	}

	void VulkanIndexBuffer::Clear() {
		VulkanContext& context = VulkanContext::GetContext();

		if (indexBuffer != VK_NULL_HANDLE)
			vmaDestroyBuffer(context.GetAllocator(), indexBuffer, indexAlloc);
	}

	void VulkanIndexBuffer::Bind() const {
		VulkanContext& context = VulkanContext::GetContext();
		vkCmdBindIndexBuffer(context.GetCurrentFrameCommandBuffer(), indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}
	
	void VulkanIndexBuffer::SetData(const std::vector<uint32_t> indices) {
		const VkDeviceSize size = indices.size() * sizeof(uint32_t);
		SetBufferData(indices.data(), size, indexBuffer);
	}
	
	UniformBuffer::UniformBuffer() : size(0) {
		MIST_WARN("Created new uniform buffer but no buffer allocated yet");
	}

	UniformBuffer::UniformBuffer(uint32_t size, void* data) : size(size) {
		VmaAllocationInfo info {};
		CreateBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, uboBuffer, uboAlloc, VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT, info);
		SetData(size, data);
		MIST_INFO("Created new uniform buffer and set data");
	}
	
	UniformBuffer::~UniformBuffer() {
		Clear();
		MIST_INFO("Destroyed uniform buffer");
	}

	UniformBuffer::UniformBuffer(UniformBuffer&& other) noexcept : uboBuffer(other.uboBuffer), uboAlloc(other.uboAlloc), size(other.size) {
		other.Clear();
	}

	UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) noexcept {
		if (this != &other) {
			this->Clear();
			this->uboBuffer = other.uboBuffer;
			this->uboAlloc = other.uboAlloc;
			this->size = other.size;
			other.Clear();
		}
		
		return *this;
	}

	void UniformBuffer::SetData(uint32_t size, void* data) {
		VulkanContext& context = VulkanContext::GetContext();
		
		void* mappedData;
		vmaMapMemory(context.GetAllocator(), uboAlloc, &mappedData);
		memcpy(mappedData, data, size);
		vmaUnmapMemory(context.GetAllocator(), uboAlloc);
	}

	void UniformBuffer::Clear() {
		VulkanContext& context = VulkanContext::GetContext();

		if (uboBuffer != VK_NULL_HANDLE)
			vmaDestroyBuffer(context.GetAllocator(), uboBuffer, uboAlloc);
	}
}