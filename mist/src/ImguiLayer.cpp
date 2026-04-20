#include "imgui/ImguiLayer.hpp"
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include "Application.hpp"
#include "Log.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"

namespace mist {
	ImguiLayer::ImguiLayer() : Layer("ImguiLayer") {}

	ImguiLayer::~ImguiLayer() {}

	void ImguiLayer::OnAttach() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags = 
			ImGuiConfigFlags_NavEnableKeyboard |
			ImGuiConfigFlags_DockingEnable |
			ImGuiConfigFlags_ViewportsEnable;
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Bold.ttf", 16);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Regular.ttf", 16);

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		SetDarkThemeColors();

		if (Application::Get().GetRenderAPI()->GetAPI() == RenderAPI::Vulkan) {
			ImGui_ImplSDL3_InitForVulkan(Application::Get().GetWindow()->GetNativeWindow());
			
			VulkanContext& context = VulkanContext::GetContext();
			QueueFamilyIndices indicies = context.FindQueueFamilies();
	
			VkSurfaceCapabilitiesKHR capabilities;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.GetPhysicalDevice(), context.GetSurface(), &capabilities);
			uint32_t swapchainImageCount = capabilities.minImageCount;
			if (capabilities.maxImageCount > 0 && swapchainImageCount > capabilities.maxImageCount)
				swapchainImageCount = capabilities.maxImageCount;

			ImGui_ImplVulkan_InitInfo info = {};
			info.Instance = context.GetInstance();
			info.PhysicalDevice = context.GetPhysicalDevice();
			info.Device = context.GetDevice();
			info.QueueFamily = indicies.graphicsFamily.value();
			info.Queue = context.GetGraphicsQueue();
			info.PipelineCache = VK_NULL_HANDLE;
			info.DescriptorPool = context.descriptors.GetImGuiDescriptorPool();
			info.MinImageCount = capabilities.minImageCount;
			info.ImageCount = swapchainImageCount;
			info.Allocator = context.GetAllocationCallbacks();
			info.PipelineInfoMain.RenderPass = context.GetRenderPass();
			info.PipelineInfoMain.Subpass = 0;
			info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			info.CheckVkResultFn = CheckVkResult;
			ImGui_ImplVulkan_Init(&info);
			ImGui_ImplVulkan_CreateMainPipeline(&info.PipelineInfoMain);
		} else {
			MIST_ERROR("Not implemented imgui backends for this render API");
		}
	}

	void ImguiLayer::OnDetach() {
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}

	void ImguiLayer::OnUpdate() {}

	void ImguiLayer::OnRender() {}

	void ImguiLayer::OnImguiRender() {}

	void ImguiLayer::OnEvent(const SDL_Event* e) {
		ImGui_ImplSDL3_ProcessEvent(e);
	}

	void ImguiLayer::Begin() {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		//ImGuizmo::BeginFrame(); This is not been added to the cmake and vcpkg yet but will mostly likely use this later
	}

	void ImguiLayer::End() {
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(
			(float)Application::Get().GetWindow()->GetWidth(), 
			(float)Application::Get().GetWindow()->GetHeight()
		);

		VulkanContext& context = VulkanContext::GetContext();
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), context.GetCurrentFrameCommandBuffer());
	
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void ImguiLayer::SetDarkThemeColors() {
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title BG
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.95f, 0.1505f, 0.951f, 1.0f };
	}
}