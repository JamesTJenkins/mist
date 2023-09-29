#include "renderer/Renderer.hpp"
#include "renderer/vulkan/VulkanRendererAPI.hpp"

namespace mist {
    RendererAPI* Renderer::rendererAPI = new VulkanRendererAPI();

    void Renderer::Initialize() {
        rendererAPI->Initialize();
    }

    void Renderer::Shutdown() {
        rendererAPI->Shutdown();
    }
}