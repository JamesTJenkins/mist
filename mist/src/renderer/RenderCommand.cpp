#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanRenderAPI.hpp"

namespace mist {
    Scope<RenderAPI> RenderCommand::renderAPI = CreateScope<VulkanRenderAPI>(new VulkanRenderAPI());
}