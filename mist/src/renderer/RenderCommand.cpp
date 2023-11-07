#include "renderer/RenderCommand.hpp"
#include "renderer/vulkan/VulkanRenderAPI.hpp"

namespace mist {
    RenderAPI* RenderCommand::renderAPI = new VulkanRenderAPI();
}