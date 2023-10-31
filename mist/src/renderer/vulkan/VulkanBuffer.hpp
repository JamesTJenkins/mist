#pragma once
#include "renderer/Buffer.hpp"

namespace mist {
    class VulkanVertexBuffer : public VertexBuffer {
    public:
        VulkanVertexBuffer(uint32_t size);
        VulkanVertexBuffer(float* vertices, uint32_t size);
        virtual ~VulkanVertexBuffer() override;

        VulkanVertexBuffer(const VulkanVertexBuffer& other) = delete;
        VulkanVertexBuffer& operator=(const VulkanVertexBuffer& other) = delete;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual const BufferLayout& GetLayout() const override { return layout; }
        virtual void SetLayout(const BufferLayout& layout) override { this->layout = layout; }

        virtual void SetData(const void* data, uint32_t size) override;
    private:
        uint32_t rendererID;
        BufferLayout layout;
    };

    class VulkanIndexBuffer : public IndexBuffer {
    public:
        VulkanIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~VulkanIndexBuffer() override;

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32_t GetCount() const override { return count; }
    private:
        uint32_t rendererID;
        uint32_t count;
    };
}