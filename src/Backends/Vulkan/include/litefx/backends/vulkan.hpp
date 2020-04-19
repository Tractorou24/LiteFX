#pragma once

#include <litefx/rendering.hpp>

#include "vulkan_api.hpp"
#include "vulkan_platform.hpp"
#include "vulkan_builders.hpp"

namespace LiteFX::Rendering::Backends {
	using namespace LiteFX::Math;
	using namespace LiteFX::Rendering;

	// Class definitions.
	class LITEFX_VULKAN_API VulkanRasterizer : public IRasterizer {
		LITEFX_IMPLEMENTATION(VulkanRasterizerImpl)

	public:
		VulkanRasterizer() noexcept;
		VulkanRasterizer(VulkanRasterizer&&) noexcept = delete;
		VulkanRasterizer(const VulkanRasterizer&) noexcept = delete;
		virtual ~VulkanRasterizer() noexcept;

	public:
		virtual PolygonMode getPolygonMode() const noexcept override;
		virtual void setPolygonMode(const PolygonMode& mode) noexcept override;
		virtual CullMode getCullMode() const noexcept override;
		virtual void setCullMode(const CullMode& mode) noexcept override;
		virtual CullOrder getCullOrder() const noexcept override;
		virtual void setCullOrder(const CullOrder& order) noexcept override;
		virtual Float getLineWidth() const noexcept override;
		virtual void setLineWidth(const Float& width) noexcept override;
		virtual bool getDepthBiasEnabled() const noexcept override;
		virtual void setDepthBiasEnabled(const bool& enable) noexcept override;
		virtual float getDepthBiasClamp() const noexcept override;
		virtual void setDepthBiasClamp(const float& clamp) noexcept override;
		virtual float getDepthBiasConstantFactor() const noexcept override;
		virtual void setDepthBiasConstantFactor(const float& factor) noexcept override;
		virtual float getDepthBiasSlopeFactor() const noexcept override;
		virtual void setDepthBiasSlopeFactor(const float& factor) noexcept override;
	};

	class LITEFX_VULKAN_API VulkanInputAssembler : public IInputAssembler {
		LITEFX_IMPLEMENTATION(VulkanInputAssemblerImpl)

	public:
		VulkanInputAssembler() noexcept;
		VulkanInputAssembler(UniquePtr<BufferLayout>&&) noexcept;
		VulkanInputAssembler(VulkanInputAssembler&&) noexcept = delete;
		VulkanInputAssembler(const VulkanInputAssembler&) noexcept = delete;
		virtual ~VulkanInputAssembler() noexcept;

	public:
		virtual const BufferLayout* getLayout() const override;
		virtual void use(UniquePtr<BufferLayout>&& layout) override;
	};

	class LITEFX_VULKAN_API VulkanViewport : public IViewport {
		LITEFX_IMPLEMENTATION(VulkanViewportImpl)

	public:
		VulkanViewport(const RectF& clientRect = { }) noexcept;
		VulkanViewport(VulkanViewport&&) noexcept = delete;
		VulkanViewport(const VulkanViewport&) noexcept = delete;
		virtual ~VulkanViewport() noexcept;

	public:
		virtual RectF getRectangle() const noexcept override;
		virtual void setRectangle(const RectF& rectangle) noexcept override;
		virtual const Array<RectF>& getScissors() const noexcept override;
		virtual Array<RectF>& getScissors() noexcept override;
	};

	class LITEFX_VULKAN_API VulkanRenderPipelineLayout : public IRenderPipelineLayout {
		LITEFX_IMPLEMENTATION(VulkanRenderPipelineLayoutImpl)

	public:
		VulkanRenderPipelineLayout() noexcept;
		VulkanRenderPipelineLayout(VulkanRenderPipelineLayout&&) = delete;
		VulkanRenderPipelineLayout(const VulkanRenderPipelineLayout&) = delete;
		virtual ~VulkanRenderPipelineLayout() noexcept;

	public:
		virtual Array<const IViewport*> getViewports() const noexcept override;
		virtual void use(UniquePtr<IViewport>&& viewport) override;
		virtual UniquePtr<IViewport> remove(const IViewport* viewport) noexcept override;
		virtual const IRasterizer* getRasterizer() const noexcept override;
		virtual void use(UniquePtr<IRasterizer>&& rasterizer) override;
		virtual const IShaderProgram* getProgram() const noexcept override;
		virtual void use(UniquePtr<IShaderProgram>&& program) override;
	};

	class LITEFX_VULKAN_API VulkanRenderPipeline : public IRenderPipeline {
		LITEFX_IMPLEMENTATION(VulkanRenderPipelineImpl)

	public:
		VulkanRenderPipeline() noexcept;
		VulkanRenderPipeline(VulkanRenderPipeline&&) noexcept = delete;
		VulkanRenderPipeline(const VulkanRenderPipeline&) noexcept = delete;
		virtual ~VulkanRenderPipeline() noexcept;

	public:
		virtual const IRenderPipelineLayout* getLayout() const noexcept override;
		virtual void use(UniquePtr<IRenderPipelineLayout>&& layout) override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_VULKAN_API VulkanShaderModule : public IShaderModule, public IResource<VkShaderModule> {
		LITEFX_IMPLEMENTATION(VulkanShaderModuleImpl)

	public:
		VulkanShaderModule(const VulkanDevice* device, const ShaderType& type, const String& fileName, const String& entryPoint = "main");
		virtual ~VulkanShaderModule() noexcept;

	public:
		virtual const IGraphicsDevice* getDevice() const noexcept override;
		virtual const String& getFileName() const noexcept override;
		virtual const String& getEntryPoint() const noexcept override;
		virtual const ShaderType& getType() const noexcept override;

	public:
		virtual VkPipelineShaderStageCreateInfo getShaderStageDefinition() const;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_VULKAN_API VulkanTexture : public ITexture, public IResource<VkImage> {
		LITEFX_IMPLEMENTATION(VulkanTextureImpl)

	public:
		//VulkanTexture() noexcept = default;
		VulkanTexture(const VulkanDevice* device, VkImage image, const Format& format, const Size2d& size);
		virtual ~VulkanTexture() noexcept;

	public:
		virtual Size2d getSize() const noexcept override;
		virtual Format getFormat() const noexcept override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_VULKAN_API VulkanSwapChain : public ISwapChain, public IResource<VkSwapchainKHR> {
		LITEFX_IMPLEMENTATION(VulkanSwapChainImpl)

	public:
		VulkanSwapChain(const VulkanDevice* device, const Format& format = Format::B8G8R8A8_UNORM_SRGB);
		virtual ~VulkanSwapChain() noexcept;

	public:
		virtual const IGraphicsDevice* getDevice() const noexcept override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_VULKAN_API VulkanQueue : public ICommandQueue, public IResource<VkQueue> {
		LITEFX_IMPLEMENTATION(VulkanQueueImpl)
	
	public:
		VulkanQueue(const QueueType& type, const uint32_t id) noexcept;
		virtual ~VulkanQueue() noexcept;

	public:
		virtual uint32_t getId() const noexcept;

	public:
		virtual void initDeviceQueue(const VulkanDevice* device);

	public:
		virtual QueueType getType() const noexcept override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_VULKAN_API VulkanDevice : public GraphicsDevice, public IResource<VkDevice> {
		LITEFX_IMPLEMENTATION(VulkanDeviceImpl)

	public:
		using builder = VulkanDeviceBuilder;
		friend class VulkanDeviceBuilder;

	public:
		VulkanDevice(const IGraphicsAdapter* adapter, const ISurface* surface, const Array<String>& extensions = { });
		VulkanDevice(const IGraphicsAdapter* adapter, const ISurface* surface, ICommandQueue* deviceQueue, const Format& format, const Array<String>& extensions = { });
		VulkanDevice(const VulkanDevice&) = delete;
		VulkanDevice(VulkanDevice&&) = delete;
		virtual ~VulkanDevice() noexcept;

	public:
		virtual const Array<String>& getExtensions() const noexcept;
		virtual Array<Format> getSurfaceFormats() const override;
		virtual const ISwapChain* getSwapChain() const noexcept override;

	public:
		//virtual UniquePtr<ITexture> createTexture2d(const Format& format = Format::B8G8R8A8_UNORM_SRGB, const Size2d& size = Size2d(0)) const override;
		virtual UniquePtr<ITexture> makeTexture2d(VkImage image, const Format& format = Format::B8G8R8A8_UNORM_SRGB, const Size2d& size = Size2d(0)) const;
		virtual UniquePtr<IShaderModule> loadShaderModule(const ShaderType& type, const String& fileName, const String& entryPoint = "main") const override;

	public:
		virtual VkImageView vkCreateImageView(const VkImage& image, const Format& format) const;

	public:
		virtual bool validateDeviceExtensions(const Array<String>& extensions) const noexcept;
		virtual Array<String> getAvailableDeviceExtensions() const noexcept;

	protected:
		virtual void create(const Format& format, ICommandQueue* queue);
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_VULKAN_API VulkanGraphicsAdapter : public IGraphicsAdapter, public IResource<VkPhysicalDevice> {
		LITEFX_IMPLEMENTATION(VulkanGraphicsAdapterImpl)

	public:
		VulkanGraphicsAdapter(VkPhysicalDevice adapter);
		VulkanGraphicsAdapter(const VulkanGraphicsAdapter&) = delete;
		VulkanGraphicsAdapter(VulkanGraphicsAdapter&&) = delete;
		virtual ~VulkanGraphicsAdapter() noexcept;

	public:
		virtual String getName() const noexcept override;
		virtual uint32_t getVendorId() const noexcept override;
		virtual uint32_t getDeviceId() const noexcept override;
		virtual GraphicsAdapterType getType() const noexcept override;
		virtual uint32_t getDriverVersion() const noexcept override;
		virtual uint32_t getApiVersion() const noexcept override;

	public:
		virtual UniquePtr<IGraphicsDevice> createDevice(const ISurface* surface, const Format& format = Format::B8G8R8A8_UNORM_SRGB, const Array<String>& extensions = { }) const override;
		virtual ICommandQueue* findQueue(const QueueType& queueType) const override;
		virtual ICommandQueue* findQueue(const QueueType& queueType, const ISurface* forSurface) const override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_VULKAN_API VulkanBackend : public RenderBackend, public IResource<VkInstance> {
		LITEFX_IMPLEMENTATION(VulkanBackendImpl);

	public:
		using builder = VulkanBackendBuilder;

	public:
		explicit VulkanBackend(const App& app, const Array<String>& extensions = { }, const Array<String>& validationLayers = { });
		VulkanBackend(const VulkanBackend&) noexcept = delete;
		VulkanBackend(VulkanBackend&&) noexcept = delete;
		virtual ~VulkanBackend();

	public:
		virtual Array<const IGraphicsAdapter*> listAdapters() const override;
		virtual const IGraphicsAdapter* findAdapter(const Optional<uint32_t>& adapterId = std::nullopt) const override;
		virtual const ISurface* getSurface() const noexcept override;
		virtual const IGraphicsAdapter* getAdapter() const noexcept override;

	public:
		virtual void use(const IGraphicsAdapter* adapter) override;
		virtual void use(UniquePtr<ISurface>&& surface) override;

	public:
		static bool validateExtensions(const Array<String>& extensions) noexcept;
		static Array<String> getAvailableExtensions() noexcept;
		static bool validateLayers(const Array<String>& validationLayers) noexcept;
		static Array<String> getValidationLayers() noexcept;
	};

}