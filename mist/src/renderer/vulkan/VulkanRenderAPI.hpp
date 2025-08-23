#pragma once
#include "renderer/RenderAPI.hpp"

namespace mist {
    
    class VulkanRenderAPI : public RenderAPI {
    public:
        // Render API overrides
        virtual void Initialize() override;
        virtual void Shutdown() override;

        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual void SetClearColor(glm::vec4& color) override;
        virtual glm::vec4 GetClearColor() override { return clearColor; }
        virtual void Clear() override;

        virtual void BeginRenderPass() override;
        virtual void EndRenderPass() override;
        virtual void BindMeshRenderer(MeshRenderer& meshRenderer) override;
        virtual void Draw(uint32_t indexCount) override;
        virtual void Submit() override;

        virtual RenderAPI::API GetAPI() override { return RenderAPI::API::Vulkan; }
    private:
        glm::vec4 clearColor = glm::vec4(0,0,0,1);   // RGBA
    };
}