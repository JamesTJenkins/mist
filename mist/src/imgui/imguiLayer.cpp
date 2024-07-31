#include "imgui/imguiLayer.hpp"
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#include "Application.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"

namespace mist {
	VkCommandBuffer buffer = VK_NULL_HANDLE;

    ImguiLayer::ImguiLayer() : Layer("imguiLayer") {

    }

    ImguiLayer::~ImguiLayer() {

    }

    void ImguiLayer::OnAttach() {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();

		VulkanContext& context = VulkanContext::GetContext();
		// Setup Platform/Renderer backends (May need to change from current windows and context in future with multiple windows)
		ImGui_ImplSDL2_InitForVulkan(SDL_GL_GetCurrentWindow());
		ImGui_ImplVulkan_InitInfo info {};
		info.Instance = context.GetInstance();
		info.PhysicalDevice = context.GetPhysicalDevice();
		info.Device = context.GetDevice();
		info.QueueFamily = context.FindQueueFamilies().graphicsFamily.value();
		info.Queue = context.GetGraphicsQueue();
		info.PipelineCache = VK_NULL_HANDLE;
		info.DescriptorPool = context.descriptors.GetDescriptorPool();
		info.Subpass = 0;
		info.MinImageCount = 2;
		info.ImageCount = context.GetSwapchainInstance(0).get()->GetSwapchainImageCount();
		info.RenderPass = context.GetSwapchainInstance(0).get()->GetRenderPass();
		info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		info.Allocator = context.GetAllocationCallbacks();
		info.CheckVkResultFn = CheckVkResult;
		ImGui_ImplVulkan_Init(&info);

		// Fonts
		io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Bold.ttf", 16);
		//io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Regular.ttf", 16);
    }

    void ImguiLayer::OnDetach() {
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
    }

    void ImguiLayer::OnUpdate() {

    }

    void ImguiLayer::OnEvent(const SDL_Event* e) {
		ImGui_ImplSDL2_ProcessEvent(e);
    }

    void ImguiLayer::Begin() {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		VulkanContext& context = VulkanContext::GetContext();
		buffer = context.commands.AllocateCommandBuffers(1)[0];
		context.commands.BeginCommandBuffer(buffer);
    }

    void ImguiLayer::End() {
		ImGuiIO& io = ImGui::GetIO();
		Application& application = Application::Get();
		io.DisplaySize = ImVec2((float)application.GetWindow().GetWidth(), (float)application.GetWindow().GetHeight());

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
			SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
			//ImGui::UpdatePlatformWindows();
			//ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
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