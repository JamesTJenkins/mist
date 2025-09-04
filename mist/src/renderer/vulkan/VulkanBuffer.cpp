#include "renderer/vulkan/VulkanBuffer.hpp"
#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanCommand.hpp"
#include "VulkanDebug.hpp"
#include "Log.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "Debug.hpp"

namespace mist {
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags flags, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VulkanContext& context = VulkanContext::GetContext();

		VkBufferCreateInfo bufferInfo {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		CheckVkResult(vkCreateBuffer(context.GetDevice(), &bufferInfo, context.GetAllocationCallbacks(), &buffer));

		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(context.GetDevice(), buffer, &requirements);

		VkMemoryAllocateInfo memoryAllocInfo {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = requirements.size;
		memoryAllocInfo.memoryTypeIndex = context.FindMemoryType(requirements.memoryTypeBits, flags);

		CheckVkResult(vkAllocateMemory(context.GetDevice(), &memoryAllocInfo, context.GetAllocationCallbacks(), &bufferMemory));
		CheckVkResult(vkBindBufferMemory(context.GetDevice(), buffer, bufferMemory, 0));
	}

	void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		VulkanContext& context = VulkanContext::GetContext();

		VkCommandBuffer commandBuffer = context.commands.BeginSingleTimeCommand();
		
		VkBufferCopy copyRegion {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		context.commands.EndCommandBuffer(commandBuffer);
		context.commands.SubmitCommandBuffersImmediately(commandBuffer, 1);

		context.commands.FreeCommandBuffer(commandBuffer);
	}

	void SetBufferData(const void* data, VkDeviceSize size, VkBuffer& buffer) {
		VulkanContext& context = VulkanContext::GetContext();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingMemory;
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);

		void* mappedData;
		vkMapMemory(context.GetDevice(), stagingMemory, 0, size, 0, &mappedData);
		memcpy(mappedData, data, (size_t)size);
		vkUnmapMemory(context.GetDevice(), stagingMemory);

		CopyBuffer(stagingBuffer, buffer, size);

		vkDestroyBuffer(context.GetDevice(), stagingBuffer, context.GetAllocationCallbacks());
		vkFreeMemory(context.GetDevice(), stagingMemory, context.GetAllocationCallbacks());
	}

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t count) {
		const VkDeviceSize size = sizeof(float) * count;
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(std::vector<Vertex> vertices) {
		const VkDeviceSize size = sizeof(Vertex) * vertices.size();
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
		SetData(vertices);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer() {
		Clear();
	}

	void VulkanVertexBuffer::Clear() {
		if (vertexBuffer != VK_NULL_HANDLE) {
			VulkanContext& context = VulkanContext::GetContext();
			vkDeviceWaitIdle(context.GetDevice());
			vkDestroyBuffer(context.GetDevice(), vertexBuffer, context.GetAllocationCallbacks());
			vkFreeMemory(context.GetDevice(), vertexBufferMemory, context.GetAllocationCallbacks());
		}
	}

	void VulkanVertexBuffer::Bind() const {
		VulkanContext& context = VulkanContext::GetContext();
		VkDeviceSize offsets[] = {0};
		uint8_t currentFrame = context.GetSwapchain()->GetCurrentFrameIndex();
		vkCmdBindVertexBuffers(context.commands.GetRenderBuffer(currentFrame), 0, 1, &vertexBuffer, offsets);
	}

	void VulkanVertexBuffer::Unbind() const {
		VkBuffer nullBuffer = VK_NULL_HANDLE;
		VkDeviceSize nullOffsets[] = {0};
		VulkanContext& context = VulkanContext::GetContext();
		uint8_t currentFrame = context.GetSwapchain()->GetCurrentFrameIndex();
		vkCmdBindVertexBuffers(context.commands.GetRenderBuffer(currentFrame), 0, 1, &nullBuffer, nullOffsets);
	}

	void VulkanVertexBuffer::SetData(const std::vector<Vertex> vertices) {
		const VkDeviceSize size = sizeof(Vertex) * vertices.size();
		SetBufferData(vertices.data(), size, vertexBuffer);
	}

	VulkanIndexBuffer::VulkanIndexBuffer(std::vector<uint32_t> indices) {
		const VkDeviceSize size = indices.size() * sizeof(uint32_t);
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
		SetBufferData(indices.data(), size, indexBuffer);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer() {
		Clear();
	}

	void VulkanIndexBuffer::Clear() {
		if (indexBuffer != VK_NULL_HANDLE) {
			VulkanContext& context = VulkanContext::GetContext();
			vkDeviceWaitIdle(context.GetDevice());
			vkDestroyBuffer(context.GetDevice(), indexBuffer, context.GetAllocationCallbacks());
			vkFreeMemory(context.GetDevice(), indexBufferMemory, context.GetAllocationCallbacks());
		}
	}

	void VulkanIndexBuffer::Bind() const {
		VulkanContext& context = VulkanContext::GetContext();
		uint8_t currentFrame = context.GetSwapchain()->GetCurrentFrameIndex();
		vkCmdBindIndexBuffer(context.commands.GetRenderBuffer(currentFrame), indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanIndexBuffer::Unbind() const {
		VkBuffer nullBuffer = VK_NULL_HANDLE;
		VulkanContext& context = VulkanContext::GetContext();
		uint8_t currentFrame = context.GetSwapchain()->GetCurrentFrameIndex();
		vkCmdBindIndexBuffer(context.commands.GetRenderBuffer(currentFrame), nullBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	UniformBuffer::UniformBuffer() : size(0) {}

	UniformBuffer::UniformBuffer(uint32_t size, void* data) : size(size) {
		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, uboBuffer, uboMemory);
		if (data != nullptr)
			SetBufferData(data, size, uboBuffer);
	}

	UniformBuffer::~UniformBuffer() {
		Clear();
	}

	void UniformBuffer::SetData(uint32_t newSize, void* newData) {
		size = newSize;
		SetBufferData(newData, newSize, uboBuffer);
	}

	void UniformBuffer::Clear() {
		VulkanContext& context = VulkanContext::GetContext();
		if (uboBuffer != VK_NULL_HANDLE) {
			vkDeviceWaitIdle(context.GetDevice());
			vkDestroyBuffer(context.GetDevice(), uboBuffer, context.GetAllocationCallbacks());
		}

		if (uboMemory != VK_NULL_HANDLE) {
			//vkUnmapMemory(context.GetDevice(), uboMemory);
			vkFreeMemory(context.GetDevice(), uboMemory, context.GetAllocationCallbacks());
		}
	}
}