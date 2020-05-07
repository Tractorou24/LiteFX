#pragma once

#include <litefx/litefx.h>
#include <litefx/backends/vulkan.hpp>

#if (defined _WIN32 || defined WINCE)
#  define GLFW_EXPOSE_NATIVE_WIN32
#else 
#  pragma message ("Basic Rendering Sample: No supported surface platform detected.")
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <memory>

using namespace LiteFX;
using namespace LiteFX::Rendering;
using namespace LiteFX::Rendering::Backends;

struct GlfwWindowDeleter {
	void operator()(GLFWwindow* ptr) noexcept {
		::glfwDestroyWindow(ptr);
	}
};

typedef UniquePtr<GLFWwindow, GlfwWindowDeleter> GlfwWindowPtr;

class SampleApp : public LiteFX::App {
public:
	static String name() noexcept { return "LiteFX Sample: Basic Rendering"; }
	String getName() const noexcept override { return name(); }

	static AppVersion version() noexcept { return AppVersion(1, 0, 0, 0); }
	AppVersion getVersion() const noexcept override { return version(); }

private:
	GlfwWindowPtr m_window;

public:
	SampleApp(GlfwWindowPtr&& window) : App(), m_window(std::move(window)) { }

public:
	virtual const IRenderBackend* getRenderBackend() const noexcept {
		return dynamic_cast<const IRenderBackend*>(this->findBackend(BackendType::Rendering));
	}
	virtual void run() override;
	void handleEvents();
	void drawFrame(UniquePtr<VulkanRenderPipeline>& pipeline);
};