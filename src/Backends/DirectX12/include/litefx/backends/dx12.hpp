#pragma once

#include <litefx/rendering.hpp>

#include "dx12_api.hpp"
#include "dx12_builders.hpp"
#include "dx12_formatters.hpp"

namespace LiteFX::Rendering::Backends {
	using namespace LiteFX::Math;
	using namespace LiteFX::Rendering;

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12Backend : public RenderBackend, public IComResource<IDXGIFactory7> {
		LITEFX_IMPLEMENTATION(DirectX12BackendImpl);
		LITEFX_BUILDER(DirectX12BackendBuilder);

	public:
		explicit DirectX12Backend(const App& app, const bool& advancedSoftwareRasterizer = false);
		DirectX12Backend(const DirectX12Backend&) noexcept = delete;
		DirectX12Backend(DirectX12Backend&&) noexcept = delete;
		virtual ~DirectX12Backend();

	public:
		virtual Array<const IGraphicsAdapter*> listAdapters() const override;
		virtual const IGraphicsAdapter* findAdapter(const Optional<uint32_t>& adapterId = std::nullopt) const override;
		virtual const ISurface* getSurface() const noexcept override;
		virtual const IGraphicsAdapter* getAdapter() const noexcept override;

	public:
		virtual void use(const IGraphicsAdapter* adapter) override;
		virtual void use(UniquePtr<ISurface>&& surface) override;

	public:
		virtual void enableAdvancedSoftwareRasterizer(const bool& enable = false);
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12GraphicsAdapter : public IGraphicsAdapter, public IComResource<IDXGIAdapter4> {
		LITEFX_IMPLEMENTATION(DirectX12GraphicsAdapterImpl);

	public:
		explicit DirectX12GraphicsAdapter(ComPtr<IDXGIAdapter4> adapter);
		DirectX12GraphicsAdapter(const DirectX12GraphicsAdapter&) = delete;
		DirectX12GraphicsAdapter(DirectX12GraphicsAdapter&&) = delete;
		virtual ~DirectX12GraphicsAdapter() noexcept;

	public:
		virtual String getName() const noexcept override;
		virtual uint32_t getVendorId() const noexcept override;
		virtual uint32_t getDeviceId() const noexcept override;
		virtual GraphicsAdapterType getType() const noexcept override;
		
		/// <inheritdoc />
		/// <remarks>
		/// This property is not supported by DirectX 12. The method always returns `0`.
		/// </remarks>
		virtual uint32_t getDriverVersion() const noexcept override;

		/// <inheritdoc />
		/// <remarks>
		/// This property is not supported by DirectX 12. The method always returns `0`.
		/// </remarks>
		virtual uint32_t getApiVersion() const noexcept override;
		virtual uint32_t getDedicatedMemory() const noexcept override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12Surface : public ISurface, public IResource<HWND> {
	public:
		DirectX12Surface(const HWND& hwnd) noexcept;
		DirectX12Surface(const DirectX12Surface&) = delete;
		DirectX12Surface(DirectX12Surface&&) = delete;
		virtual ~DirectX12Surface() noexcept;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12Device : public GraphicsDevice, public IComResource<ID3D12Device5> {
		LITEFX_IMPLEMENTATION(DirectX12DeviceImpl);

	public:
		explicit DirectX12Device(const IRenderBackend* backend, const Format& format, const Size2d& frameBufferSize, const UInt32& frameBuffers);
		DirectX12Device(const DirectX12Device&) = delete;
		DirectX12Device(DirectX12Device&&) = delete;
		virtual ~DirectX12Device() noexcept;

	public:
		virtual size_t getBufferWidth() const noexcept override;
		virtual size_t getBufferHeight() const noexcept override;
		virtual const ICommandQueue* graphicsQueue() const noexcept override;
		virtual const ICommandQueue* transferQueue() const noexcept override;
		virtual const ICommandQueue* bufferQueue() const noexcept override;
		virtual void wait() override;
		virtual void resize(int width, int height) override;
		virtual UniquePtr<IBuffer> createBuffer(const BufferType& type, const BufferUsage& usage, const size_t& size, const UInt32& elements = 1) const override;
		virtual UniquePtr<IVertexBuffer> createVertexBuffer(const IVertexBufferLayout* layout, const BufferUsage& usage, const UInt32& elements = 1) const override;
		virtual UniquePtr<IIndexBuffer> createIndexBuffer(const IIndexBufferLayout* layout, const BufferUsage& usage, const UInt32& elements) const override;
		virtual UniquePtr<IConstantBuffer> createConstantBuffer(const IDescriptorLayout* layout, const BufferUsage& usage, const UInt32& elements) const override;
		virtual UniquePtr<IImage> createImage(const Format& format, const Size2d& size, const UInt32& levels = 1, const MultiSamplingLevel& samples = MultiSamplingLevel::x1) const override;
		virtual UniquePtr<IImage> createAttachment(const Format& format, const Size2d& size, const MultiSamplingLevel& samples = MultiSamplingLevel::x1) const override;
		virtual UniquePtr<ITexture> createTexture(const IDescriptorLayout* layout, const Format& format, const Size2d& size, const UInt32& levels = 1, const MultiSamplingLevel& samples = MultiSamplingLevel::x1) const override;
		virtual UniquePtr<ISampler> createSampler(const IDescriptorLayout* layout, const FilterMode& magFilter = FilterMode::Nearest, const FilterMode& minFilter = FilterMode::Nearest, const BorderMode& borderU = BorderMode::Repeat, const BorderMode& borderV = BorderMode::Repeat, const BorderMode& borderW = BorderMode::Repeat, const MipMapMode& mipMapMode = MipMapMode::Nearest, const Float& mipMapBias = 0.f, const Float& maxLod = std::numeric_limits<Float>::max(), const Float& minLod = 0.f, const Float& anisotropy = 0.f) const override;
		virtual UniquePtr<IShaderModule> loadShaderModule(const ShaderStage& type, const String& fileName, const String& entryPoint = "main") const override;
		virtual Array<Format> getSurfaceFormats() const override;
		virtual const ISwapChain* getSwapChain() const noexcept override;

	public:
		DirectX12RenderPassBuilder buildRenderPass() const;
		//DirectX12ComputePassBuilder buildComputePass() const;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12Queue : public ICommandQueue, public IComResource<ID3D12CommandQueue> {
		LITEFX_IMPLEMENTATION(DirectX12QueueImpl);

	public:
		explicit DirectX12Queue(const IGraphicsDevice* device, const QueueType& type, const QueuePriority& priority);
		virtual ~DirectX12Queue() noexcept;

	public:
		virtual bool isBound() const noexcept override;
		virtual QueuePriority getPriority() const noexcept override;
		virtual QueueType getType() const noexcept override;
		virtual const IGraphicsDevice* getDevice() const noexcept override;

	public:
		virtual void bind() override;
		virtual void release() override;
		virtual UniquePtr<ICommandBuffer> createCommandBuffer() const override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12CommandBuffer : public ICommandBuffer, public IComResource<ID3D12GraphicsCommandList4> {
		LITEFX_IMPLEMENTATION(DirectX12CommandBufferImpl);

	public:
		explicit DirectX12CommandBuffer(const DirectX12Queue* queue);
		DirectX12CommandBuffer(const DirectX12CommandBuffer&) = delete;
		DirectX12CommandBuffer(DirectX12CommandBuffer&&) = delete;
		virtual ~DirectX12CommandBuffer() noexcept;

	public:
		virtual const ICommandQueue* getQueue() const noexcept override;

	public:
		virtual void begin() const override;
		virtual void end() const override;
		virtual void submit(const bool& waitForQueue = false) const override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12SwapChain : public ISwapChain, public IComResource<IDXGISwapChain4> {
		LITEFX_IMPLEMENTATION(DirectX12SwapChainImpl);

	public:
		explicit DirectX12SwapChain(const DirectX12Device* device, const Size2d& frameBufferSize, const UInt32& frameBuffers, const Format& format = Format::B8G8R8A8_SRGB);
		virtual ~DirectX12SwapChain() noexcept;

	public:
		virtual const Size2d& getBufferSize() const noexcept override;
		virtual size_t getWidth() const noexcept override;
		virtual size_t getHeight() const noexcept override;
		virtual const Format& getFormat() const noexcept override;
		virtual UInt32 swapBackBuffer() const override;
		virtual void reset(const Size2d& frameBufferSize, const UInt32& frameBuffers) override;
		virtual UInt32 getBuffers() const noexcept override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12RenderPass : public IRenderPass {
		LITEFX_IMPLEMENTATION(DirectX12RenderPassImpl);
		LITEFX_BUILDER(DirectX12RenderPassBuilder);

	public:
		explicit DirectX12RenderPass(const IGraphicsDevice* device);	// Adapter for builder interface.
		explicit DirectX12RenderPass(const DirectX12Device* device);
		DirectX12RenderPass(const DirectX12RenderPass&) = delete;
		DirectX12RenderPass(DirectX12RenderPass&&) = delete;
		virtual ~DirectX12RenderPass() noexcept;

	public:
		virtual const DirectX12Device* getDevice() const noexcept;
		virtual const DirectX12CommandBuffer* getDXCommandBuffer() const noexcept;

	public:
		virtual const ICommandBuffer* getCommandBuffer() const noexcept override;
		virtual const UInt32 getCurrentBackBuffer() const override;
		virtual void addTarget(UniquePtr<IRenderTarget>&& target) override;
		virtual const Array<const IRenderTarget*> getTargets() const noexcept override;
		virtual UniquePtr<IRenderTarget> removeTarget(const IRenderTarget* target) override;
		virtual Array<const IRenderPipeline*> getPipelines() const noexcept override;
		virtual const IRenderPipeline* getPipeline(const UInt32& id) const noexcept override;
		virtual void addPipeline(UniquePtr<IRenderPipeline>&& pipeline) override;
		virtual void removePipeline(const UInt32& id) override;
		virtual void setDependency(const IRenderPass* renderPass = nullptr) override;
		virtual const IRenderPass* getDependency() const noexcept override;
		virtual void begin() const override;
		virtual void end(const bool& present = false) override;
		virtual void draw(const UInt32& vertices, const UInt32& instances = 1, const UInt32& firstVertex = 0, const UInt32& firstInstance = 0) const override;
		virtual void drawIndexed(const UInt32& indices, const UInt32& instances = 1, const UInt32& firstIndex = 0, const Int32& vertexOffset = 0, const UInt32& firstInstance = 0) const override;
		virtual const IImage* getAttachment(const UInt32& attachmentId) const override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12RenderPipeline : public IRenderPipeline, public IComResource<ID3D12PipelineState> {
		LITEFX_IMPLEMENTATION(DirectX12RenderPipelineImpl);
		LITEFX_BUILDER(DirectX12RenderPipelineBuilder);

	public:
		explicit DirectX12RenderPipeline(const DirectX12RenderPass& renderPass, const UInt32& id, const String& name = "");
		DirectX12RenderPipeline(DirectX12RenderPipeline&&) noexcept = delete;
		DirectX12RenderPipeline(const DirectX12RenderPipeline&) noexcept = delete;
		virtual ~DirectX12RenderPipeline() noexcept;

		// IRequiresInitialization
	public:
		virtual bool isInitialized() const noexcept override;

		// IRenderPipeline
	public:
		/// <inheritdoc />
		virtual const IRenderPass& renderPass() const noexcept override;

		/// <inheritdoc />
		virtual const String& name() const noexcept override;

		/// <inheritdoc />
		virtual const UInt32& id() const noexcept override;

	public:
		virtual void initialize(UniquePtr<IRenderPipelineLayout>&& layout, SharedPtr<IInputAssembler> inputAssembler, SharedPtr<IRasterizer> rasterizer, Array<SharedPtr<IViewport>>&& viewports, Array<SharedPtr<IScissor>>&& scissors) override;

	public:
		virtual const IRenderPipelineLayout* getLayout() const noexcept override;
		virtual SharedPtr<IInputAssembler> getInputAssembler() const noexcept override;
		virtual SharedPtr<IRasterizer> getRasterizer() const noexcept override;
		virtual Array<const IViewport*> getViewports() const noexcept override;
		virtual Array<const IScissor*> getScissors() const noexcept override;

	public:
		virtual UniquePtr<IVertexBuffer> makeVertexBuffer(const BufferUsage& usage, const UInt32& elements, const UInt32& binding = 0) const override;
		virtual UniquePtr<IIndexBuffer> makeIndexBuffer(const BufferUsage& usage, const UInt32& elements, const IndexType& indexType) const override;
		virtual UniquePtr<IDescriptorSet> makeBufferPool(const UInt32& bufferSet) const override;
		virtual void bind(const IVertexBuffer* buffer) const override;
		virtual void bind(const IIndexBuffer* buffer) const override;
		virtual void bind(IDescriptorSet* buffer) const override;
		virtual void use() const override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12RenderPipelineLayout : public IRenderPipelineLayout, public IComResource<ID3D12RootSignature> {
		LITEFX_IMPLEMENTATION(DirectX12RenderPipelineLayoutImpl);
		LITEFX_BUILDER(DirectX12RenderPipelineLayoutBuilder);

	public:
		explicit DirectX12RenderPipelineLayout(const DirectX12RenderPipeline& pipeline);
		DirectX12RenderPipelineLayout(DirectX12RenderPipelineLayout&&) noexcept = delete;
		DirectX12RenderPipelineLayout(const DirectX12RenderPipelineLayout&) noexcept = delete;
		virtual ~DirectX12RenderPipelineLayout() noexcept;

		// IRequiresInitialization
	public:
		virtual bool isInitialized() const noexcept override;

		// IRenderPipelineLayout
	public:
		virtual void initialize(UniquePtr<IShaderProgram>&& shaderProgram, Array<UniquePtr<IDescriptorSetLayout>>&& descriptorLayouts) override;

	public:
		virtual const IShaderProgram* getProgram() const noexcept override;
		virtual Array<const IDescriptorSetLayout*> getDescriptorSetLayouts() const noexcept override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12InputAssembler : public InputAssembler {
		LITEFX_BUILDER(DirectX12InputAssemblerBuilder);

	public:
		explicit DirectX12InputAssembler(const DirectX12RenderPipeline& pipeline) noexcept;
		DirectX12InputAssembler(DirectX12InputAssembler&&) noexcept = delete;
		DirectX12InputAssembler(const DirectX12InputAssembler&) noexcept = delete;
		virtual ~DirectX12InputAssembler() noexcept;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12Rasterizer : public Rasterizer {
		LITEFX_BUILDER(DirectX12RasterizerBuilder);

	public:
		explicit DirectX12Rasterizer(const DirectX12RenderPipeline& pipeline) noexcept;
		DirectX12Rasterizer(DirectX12Rasterizer&&) noexcept = delete;
		DirectX12Rasterizer(const DirectX12Rasterizer&) noexcept = delete;
		virtual ~DirectX12Rasterizer() noexcept;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12VertexBufferLayout : public IVertexBufferLayout {
		LITEFX_IMPLEMENTATION(DirectX12VertexBufferLayoutImpl);
		LITEFX_BUILDER(DirectX12VertexBufferLayoutBuilder);

	public:
		explicit DirectX12VertexBufferLayout(const DirectX12InputAssembler& inputAssembler, const size_t& vertexSize, const UInt32& binding = 0);
		DirectX12VertexBufferLayout(DirectX12VertexBufferLayout&&) = delete;
		DirectX12VertexBufferLayout(const DirectX12VertexBufferLayout&) = delete;
		virtual ~DirectX12VertexBufferLayout() noexcept;

	public:
		virtual size_t getElementSize() const noexcept override;
		virtual UInt32 getBinding() const noexcept override;
		virtual BufferType getType() const noexcept override;

	public:
		virtual Array<const BufferAttribute*> getAttributes() const noexcept override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12IndexBufferLayout : public IIndexBufferLayout {
		LITEFX_IMPLEMENTATION(DirectX12IndexBufferLayoutImpl);

	public:
		explicit DirectX12IndexBufferLayout(const DirectX12InputAssembler& inputAssembler, const IndexType& type);
		DirectX12IndexBufferLayout(DirectX12IndexBufferLayout&&) = delete;
		DirectX12IndexBufferLayout(const DirectX12IndexBufferLayout&) = delete;
		virtual ~DirectX12IndexBufferLayout() noexcept;

	public:
		virtual size_t getElementSize() const noexcept override;
		virtual UInt32 getBinding() const noexcept override;
		virtual BufferType getType() const noexcept override;

	public:
		virtual const IndexType& getIndexType() const noexcept override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12ShaderModule : public IShaderModule, public IResource<D3D12_SHADER_BYTECODE> {
		LITEFX_IMPLEMENTATION(DirectX12ShaderModuleImpl);

	public:
		explicit DirectX12ShaderModule(const ShaderStage& type, const String& fileName, const String& entryPoint = "main");
		virtual ~DirectX12ShaderModule() noexcept;

	public:
		virtual const String& getFileName() const noexcept override;
		virtual const String& getEntryPoint() const noexcept override;
		virtual const ShaderStage& getType() const noexcept override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12ShaderProgram : public IShaderProgram {
		LITEFX_IMPLEMENTATION(DirectX12ShaderProgramImpl);
		LITEFX_BUILDER(DirectX12ShaderProgramBuilder);

	public:
		explicit DirectX12ShaderProgram(const DirectX12RenderPipelineLayout& pipelineLayout);
		DirectX12ShaderProgram(DirectX12ShaderProgram&&) noexcept = delete;
		DirectX12ShaderProgram(const DirectX12ShaderProgram&) noexcept = delete;
		virtual ~DirectX12ShaderProgram() noexcept;

	public:
		virtual Array<const IShaderModule*> getModules() const noexcept override;
		virtual void use(UniquePtr<IShaderModule>&& module) override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12DescriptorSetLayout : public IDescriptorSetLayout {
		LITEFX_IMPLEMENTATION(DirectX12DescriptorSetLayoutImpl);
		LITEFX_BUILDER(DirectX12DescriptorSetLayoutBuilder);

	public:
		explicit DirectX12DescriptorSetLayout(const DirectX12RenderPipelineLayout& pipelineLayout, const UInt32& id, const ShaderStage& stages);
		DirectX12DescriptorSetLayout(DirectX12DescriptorSetLayout&&) = delete;
		DirectX12DescriptorSetLayout(const DirectX12DescriptorSetLayout&) = delete;
		virtual ~DirectX12DescriptorSetLayout() noexcept;

	public:
		virtual Array<const IDescriptorLayout*> getLayouts() const noexcept override;
		virtual const IDescriptorLayout* getLayout(const UInt32& binding) const noexcept override;
		virtual const UInt32& getSetId() const noexcept override;
		virtual const ShaderStage& getShaderStages() const noexcept override;
		virtual UniquePtr<IDescriptorSet> createBufferPool() const noexcept override;

	public:
		virtual UInt32 uniforms() const noexcept override;
		virtual UInt32 storages() const noexcept override;
		virtual UInt32 images() const noexcept override;
		virtual UInt32 samplers() const noexcept override;
		virtual UInt32 inputAttachments() const noexcept override;
	};

	class LITEFX_DIRECTX12_API DirectX12DescriptorSet : public IDescriptorSet, IComResource<ID3D12DescriptorHeap> {
		LITEFX_IMPLEMENTATION(DirectX12DescriptorSetImpl);

	public:
		explicit DirectX12DescriptorSet(const DirectX12DescriptorSetLayout& bufferSet);
		DirectX12DescriptorSet(DirectX12DescriptorSet&&) = delete;
		DirectX12DescriptorSet(const DirectX12DescriptorSet&) = delete;
		virtual ~DirectX12DescriptorSet() noexcept;

	public:
		virtual const IDescriptorSetLayout* getDescriptorSetLayout() const noexcept override;
		virtual UniquePtr<IConstantBuffer> makeBuffer(const UInt32& binding, const BufferUsage& usage, const UInt32& elements = 1) const noexcept override;
		virtual UniquePtr<ITexture> makeTexture(const UInt32& binding, const Format& format, const Size2d& size, const UInt32& levels = 1, const MultiSamplingLevel& samples = MultiSamplingLevel::x1) const noexcept override;
		virtual UniquePtr<ISampler> makeSampler(const UInt32& binding, const FilterMode& magFilter = FilterMode::Nearest, const FilterMode& minFilter = FilterMode::Nearest, const BorderMode& borderU = BorderMode::Repeat, const BorderMode& borderV = BorderMode::Repeat, const BorderMode& borderW = BorderMode::Repeat, const MipMapMode& mipMapMode = MipMapMode::Nearest, const Float& mipMapBias = 0.f, const Float& maxLod = std::numeric_limits<Float>::max(), const Float& minLod = 0.f, const Float& anisotropy = 0.f) const noexcept override;

		/// <inheritdoc />
		virtual void update(const IConstantBuffer* buffer) const override;

		/// <inheritdoc />
		virtual void update(const ITexture* texture) const override;

		/// <inheritdoc />
		virtual void update(const ISampler* sampler) const override;

		/// <inheritdoc />
		virtual void updateAll(const IConstantBuffer* buffer) const override;

		/// <inheritdoc />
		virtual void updateAll(const ITexture* texture) const override;

		/// <inheritdoc />
		virtual void updateAll(const ISampler* sampler) const override;

		/// <inheritdoc />
		virtual void attach(const UInt32& binding, const IRenderPass* renderPass, const UInt32& attachmentId) const override;

		/// <inheritdoc />
		virtual void attach(const UInt32& binding, const IImage* image) const override;
	};

	/// <summary>
	/// 
	/// </summary>
	class LITEFX_DIRECTX12_API DirectX12DescriptorLayout : public IDescriptorLayout {
		LITEFX_IMPLEMENTATION(DirectX12DescriptorLayoutImpl);

	public:
		explicit DirectX12DescriptorLayout(const DirectX12DescriptorSetLayout& descriptorSetLayout, const DescriptorType& type, const UInt32& binding, const size_t& elementSize);
		DirectX12DescriptorLayout(DirectX12DescriptorLayout&&) = delete;
		DirectX12DescriptorLayout(const DirectX12DescriptorLayout&) = delete;
		virtual ~DirectX12DescriptorLayout() noexcept;

	public:
		virtual size_t getElementSize() const noexcept override;
		virtual UInt32 getBinding() const noexcept override;
		virtual BufferType getType() const noexcept override;

	public:
		virtual const IDescriptorSetLayout* getDescriptorSet() const noexcept override;
		virtual DescriptorType getDescriptorType() const noexcept override;
	};

}