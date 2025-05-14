#pragma once
#include <vulkan/vulkan.h>
#include "renderer/Buffer.hpp"

namespace mist {
    class VulkanVertexBuffer : public VertexBuffer {
    public:
        VulkanVertexBuffer(uint32_t count);
        VulkanVertexBuffer(const std::vector<Vertex> vertices);
        virtual ~VulkanVertexBuffer() override;

        VulkanVertexBuffer(const VulkanVertexBuffer& other) = delete;
        VulkanVertexBuffer& operator=(const VulkanVertexBuffer& other) = delete;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual const BufferLayout& GetLayout() const override { return vertexBufferLayout; }
        virtual void SetLayout(const BufferLayout& newLayout) override { vertexBufferLayout = newLayout; }

        virtual void SetData(const std::vector<Vertex> vertices) override;
    private:
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        BufferLayout vertexBufferLayout;
    };

    class VulkanIndexBuffer : public IndexBuffer {
    public:
        VulkanIndexBuffer(std::vector<uint32_t> indices);
        virtual ~VulkanIndexBuffer() override;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32_t GetCount() const override { return indexCount; }
    private:
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
    };
}