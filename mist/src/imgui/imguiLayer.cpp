#define IMGUI_IMPL_VULKAN_USE_LOADER

#include "imgui/imguiLayer.hpp"
#include <imgui_impl_sdl2.h>
#include "Application.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"

namespace mist {
	VkCommandBuffer buffer = VK_NULL_HANDLE;

    ImguiLayer::ImguiLayer(const char* name) : Layer(name) {}

    ImguiLayer::~ImguiLayer() {}

    void ImguiLayer::OnAttach() {
		IMGUI_CHECKVERSION();
		imguiContext = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
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
		window.Surface = context.GetSurface();
		const VkFormat requestSurfaceFormats[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		window.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(context.GetPhysicalDevice(), context.GetSurface(), requestSurfaceFormats, (size_t)IM_ARRAYSIZE(requestSurfaceFormats), requestSurfaceColorSpace);
		window.PresentMode = VK_PRESENT_MODE_FIFO_KHR;
		window.Swapchain = context.GetSwapchain()->GetSwapchain();
		ImGui_ImplVulkanH_CreateOrResizeWindow(
			context.GetInstance(),
			context.GetPhysicalDevice(),
			context.GetDevice(),
			&window,
			context.FindQueueFamilies().graphicsFamily.value(),
			context.GetAllocationCallbacks(),
			Application::Get().GetWindow().GetWidth(),
			Application::Get().GetWindow().GetHeight(),
			context.GetSwapchain()->GetSwapchainMinImageCount()
		);

		// Setup Platform/Renderer backends (May need to change from current windows and context in future with multiple windows)
		ImGui_ImplSDL2_InitForVulkan(Application::Get().GetWindow().GetNativeWindow());
		ImGui_ImplVulkan_InitInfo info {};
		info.Instance = context.GetInstance();
		info.PhysicalDevice = context.GetPhysicalDevice();
		info.Device = context.GetDevice();
		info.QueueFamily = context.FindQueueFamilies().graphicsFamily.value();
		info.Queue = context.GetGraphicsQueue();
		info.PipelineCache = VK_NULL_HANDLE;
		context.descriptors.CreateDescriptorPool();
		info.DescriptorPool = context.descriptors.GetDescriptorPool(0);
		info.RenderPass = window.RenderPass;
		info.Subpass = 0;
		info.MinImageCount = context.GetSwapchain()->GetSwapchainMinImageCount();
		info.ImageCount = context.GetSwapchain()->GetSwapchainImageCount();
		info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		info.Allocator = context.GetAllocationCallbacks();
		info.CheckVkResultFn = CheckVkResult;
		ImGui_ImplVulkan_Init(&info);

		// Fonts
		io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Bold.ttf", 16);
		//io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/roboto/Roboto-Regular.ttf", 16);
 
		ImGui_ImplVulkan_CreateFontsTexture();
	}

    void ImguiLayer::OnDetach() {
		VulkanContext& context = VulkanContext::GetContext();
		vkDeviceWaitIdle(context.GetDevice());
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		ImGui_ImplVulkanH_DestroyWindow(context.GetInstance(), context.GetDevice(), &window, context.GetAllocationCallbacks());
    }

    void ImguiLayer::OnUpdate() {

    }

    void ImguiLayer::OnEvent(const SDL_Event* e) {
		ImGui_ImplSDL2_ProcessEvent(e);
    }

	void ImguiLayer::OnRender() {

	}

    void ImguiLayer::Begin() {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
    }

    void ImguiLayer::End() {
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		Application& application = Application::Get();
		uint32_t width = application.GetWindow().GetWidth();
		uint32_t height = application.GetWindow().GetHeight();
		io.DisplaySize = ImVec2((float)width, (float)height);
		
		// When window is minimized or closed just simply bail out and dont bother rendering
		if (width == 0 || height == 0)
			return;

		VulkanContext& context = VulkanContext::GetContext();
		buffer = context.commands.AllocateCommandBuffers(1)[0];
		context.commands.BeginCommandBuffer(buffer);
		ImGui::Render();
		context.GetSwapchain()->BeginImGuiRenderPass(buffer);
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		context.GetSwapchain()->EndRenderPass(buffer);
		context.commands.EndCommandBuffer(buffer);
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