#include <litefx/backends/dx12.hpp>
#include <litefx/backends/dx12_builders.hpp>

using namespace LiteFX::Rendering::Backends;

// ------------------------------------------------------------------------------------------------
// Implementation.
// ------------------------------------------------------------------------------------------------

class DirectX12RenderPipeline::DirectX12RenderPipelineImpl : public Implement<DirectX12RenderPipeline> {
public:
	friend class DirectX12RenderPipelineBuilder;
	friend class DirectX12RenderPipeline;

private:
	SharedPtr<DirectX12PipelineLayout> m_layout;
	SharedPtr<DirectX12ShaderProgram> m_program;
	SharedPtr<DirectX12InputAssembler> m_inputAssembler;
	SharedPtr<DirectX12Rasterizer> m_rasterizer;
	Vector4f m_blendFactors{ 0.f, 0.f, 0.f, 0.f };
	UInt32 m_stencilRef{ 0 };
	bool m_alphaToCoverage{ false };
	const DirectX12RenderPass& m_renderPass;

public:
	DirectX12RenderPipelineImpl(DirectX12RenderPipeline* parent, const DirectX12RenderPass& renderPass, bool alphaToCoverage, SharedPtr<DirectX12PipelineLayout> layout, SharedPtr<DirectX12ShaderProgram> shaderProgram, SharedPtr<DirectX12InputAssembler> inputAssembler, SharedPtr<DirectX12Rasterizer> rasterizer) :
		base(parent), m_renderPass(renderPass), m_alphaToCoverage(alphaToCoverage), m_layout(layout), m_program(shaderProgram), m_inputAssembler(inputAssembler), m_rasterizer(rasterizer)
	{
	}

	DirectX12RenderPipelineImpl(DirectX12RenderPipeline* parent, const DirectX12RenderPass& renderPass) :
		base(parent), m_renderPass(renderPass)
	{
	}

public:
	ComPtr<ID3D12PipelineState> initialize()
	{
		LITEFX_TRACE(DIRECTX12_LOG, "Creating render pipeline \"{1}\" for layout {0}...", fmt::ptr(reinterpret_cast<void*>(m_layout.get())), m_parent->name());

		// Validate shader stage usage.
		auto modules = m_program->modules();
		bool hasComputeShaders    = std::ranges::find_if(modules, [](const auto& module) { return LITEFX_FLAG_IS_SET(ShaderStage::Compute, module->type()); }) != modules.end();
		bool hasRayTracingShaders = std::ranges::find_if(modules, [](const auto& module) { return LITEFX_FLAG_IS_SET(ShaderStage::RayTracingPipeline, module->type()); }) != modules.end();
		bool hasMeshShaders       = std::ranges::find_if(modules, [](const auto& module) { return LITEFX_FLAG_IS_SET(ShaderStage::MeshPipeline, module->type()); }) != modules.end();
		bool hasDirectShaders     = std::ranges::find_if(modules, [](const auto& module) { return LITEFX_FLAG_IS_SET(ShaderStage::RasterizationPipeline, module->type()); }) != modules.end();
		
		if (hasComputeShaders) [[unlikely]]
			throw InvalidArgumentException("shaderProgram", "The shader program contains a compute shader, which is not supported in a graphics pipeline.");
		else if (hasComputeShaders) [[unlikely]]
			throw InvalidArgumentException("shaderProgram", "The shader program contains ray-tracing shaders, which is not supported in a graphics pipeline.");
		else if (hasMeshShaders && hasDirectShaders) [[unlikely]]
			throw InvalidArgumentException("shaderProgram", "A shader program that contains mesh shaders must not also contain vertex, geometry, domain or hull shaders.");

		// Setup rasterizer state.
		auto& rasterizer = std::as_const(*m_rasterizer.get());
		D3D12_RASTERIZER_DESC rasterizerState = {};
		rasterizerState.DepthClipEnable = FALSE;
		rasterizerState.FillMode = DX12::getPolygonMode(rasterizer.polygonMode());
		rasterizerState.CullMode = DX12::getCullMode(rasterizer.cullMode());
		rasterizerState.FrontCounterClockwise = rasterizer.cullOrder() == CullOrder::CounterClockWise;
		rasterizerState.MultisampleEnable = FALSE;
		rasterizerState.AntialiasedLineEnable = FALSE;
		rasterizerState.ForcedSampleCount = 0;
		rasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		LITEFX_TRACE(DIRECTX12_LOG, "Rasterizer state: {{ PolygonMode: {0}, CullMode: {1}, CullOrder: {2}, LineWidth: {3} }}", rasterizer.polygonMode(), rasterizer.cullMode(), rasterizer.cullOrder(), rasterizer.lineWidth());

		if (!rasterizer.depthStencilState().depthState().Enable)
			LITEFX_TRACE(DIRECTX12_LOG, "\tRasterizer depth bias disabled.");
		else
		{
			LITEFX_TRACE(DIRECTX12_LOG, "\tRasterizer depth bias: {{ Clamp: {0}, ConstantFactor: {1}, SlopeFactor: {2} }}", rasterizer.depthStencilState().depthBias().Clamp, rasterizer.depthStencilState().depthBias().ConstantFactor, rasterizer.depthStencilState().depthBias().SlopeFactor);
			rasterizerState.DepthBiasClamp = rasterizer.depthStencilState().depthBias().Clamp;
			rasterizerState.DepthBias = static_cast<Int32>(rasterizer.depthStencilState().depthBias().ConstantFactor);
			rasterizerState.SlopeScaledDepthBias = rasterizer.depthStencilState().depthBias().SlopeFactor;
		}

		// Setup input assembler state.
		Array<D3D12_INPUT_ELEMENT_DESC> inputLayoutElements;
		
		LITEFX_TRACE(DIRECTX12_LOG, "Input assembler state: {{ PrimitiveTopology: {0} }}", m_inputAssembler->topology());
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = DX12::getPrimitiveTopologyType(m_inputAssembler->topology());

		auto vertexLayouts = m_inputAssembler->vertexBufferLayouts();

		std::ranges::for_each(m_inputAssembler->vertexBufferLayouts(), [&, l = 0](const DirectX12VertexBufferLayout* layout) mutable {
			auto bufferAttributes = layout->attributes();
			auto bindingPoint = layout->binding();

			LITEFX_TRACE(DIRECTX12_LOG, "Defining vertex buffer layout {0}/{1} {{ Attributes: {2}, Size: {3} bytes, Binding: {4} }}...", ++l, vertexLayouts.size(), bufferAttributes.size(), layout->elementSize(), bindingPoint);

			std::ranges::for_each(bufferAttributes, [&](const BufferAttribute* attribute) {
				D3D12_INPUT_ELEMENT_DESC elementDescriptor = {};
				elementDescriptor.SemanticName = DX12::getSemanticName(attribute->semantic());
				elementDescriptor.SemanticIndex = attribute->semanticIndex();
				elementDescriptor.Format = DX12::getFormat(attribute->format());
				elementDescriptor.InputSlot = bindingPoint;
				elementDescriptor.InputSlotClass = D3D12_INPUT_CLASSIFICATION::D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
				elementDescriptor.AlignedByteOffset = attribute->offset();	// TODO: May not include packing, but packing is required - need to test this!
				elementDescriptor.InstanceDataStepRate = 0;

				inputLayoutElements.push_back(elementDescriptor);
			});
		});

		D3D12_INPUT_LAYOUT_DESC inputLayout = {};
		inputLayout.pInputElementDescs = inputLayoutElements.data();
		inputLayout.NumElements = static_cast<UInt32>(inputLayoutElements.size());

		// Setup multi-sampling state.
		auto samples = m_renderPass.multiSamplingLevel();
		DXGI_SAMPLE_DESC multisamplingState = samples == MultiSamplingLevel::x1 ? DXGI_SAMPLE_DESC{ 1, 0 } : DXGI_SAMPLE_DESC{ std::to_underlying(samples), DXGI_STANDARD_MULTISAMPLE_QUALITY_PATTERN };

		// Setup render target states.
		// NOTE: We assume, that the targets are returned sorted by location and the location range is contiguous.
		D3D12_BLEND_DESC blendState = {};
		D3D12_DEPTH_STENCIL_DESC depthStencilState = {};
		auto targets = m_renderPass.renderTargets();
		UInt32 renderTargets = std::ranges::count_if(targets, [](const RenderTarget& renderTarget) { return renderTarget.type() != RenderTargetType::DepthStencil; });
		UInt32 depthStencilTargets = static_cast<UInt32>(targets.size()) - renderTargets;
		DXGI_FORMAT dsvFormat { };
		std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> rtvFormats { };

		// Only 8 RTVs are allowed.
		if (renderTargets > D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT) [[unlikely]]
			throw RuntimeException("You have specified too many render targets: only 8 render targets and 1 depth/stencil target are allowed, but {0} have been specified.", renderTargets);

		// Only one DSV is allowed.
		if (depthStencilTargets > 1) [[unlikely]]
			throw RuntimeException("You have specified too many render targets: only 1 depth/stencil target is allowed, but {0} have been specified.", depthStencilTargets);

		std::ranges::for_each(targets, [&, i = 0](const RenderTarget& renderTarget) mutable {
			if (renderTarget.type() == RenderTargetType::DepthStencil)
			{
				// Setup depth/stencil format.
				dsvFormat = DX12::getFormat(renderTarget.format());

				// Setup depth/stencil state.
				// TODO: From depth/stencil state.
				depthStencilState.DepthEnable = rasterizer.depthStencilState().depthState().Enable;
				depthStencilState.DepthWriteMask = rasterizer.depthStencilState().depthState().Write ? D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK::D3D12_DEPTH_WRITE_MASK_ZERO;
				depthStencilState.DepthFunc = DX12::getCompareOp(rasterizer.depthStencilState().depthState().Operation);

				depthStencilState.StencilEnable = rasterizer.depthStencilState().stencilState().Enable;
				depthStencilState.StencilReadMask = rasterizer.depthStencilState().stencilState().ReadMask;
				depthStencilState.StencilWriteMask = rasterizer.depthStencilState().stencilState().WriteMask;
				depthStencilState.FrontFace.StencilFunc = DX12::getCompareOp(rasterizer.depthStencilState().stencilState().FrontFace.Operation);
				depthStencilState.FrontFace.StencilDepthFailOp = DX12::getStencilOp(rasterizer.depthStencilState().stencilState().FrontFace.DepthFailOp);
				depthStencilState.FrontFace.StencilFailOp = DX12::getStencilOp(rasterizer.depthStencilState().stencilState().FrontFace.StencilFailOp);
				depthStencilState.FrontFace.StencilPassOp = DX12::getStencilOp(rasterizer.depthStencilState().stencilState().FrontFace.StencilPassOp);
				depthStencilState.BackFace.StencilFunc = DX12::getCompareOp(rasterizer.depthStencilState().stencilState().BackFace.Operation);
				depthStencilState.BackFace.StencilDepthFailOp = DX12::getStencilOp(rasterizer.depthStencilState().stencilState().BackFace.DepthFailOp);
				depthStencilState.BackFace.StencilFailOp = DX12::getStencilOp(rasterizer.depthStencilState().stencilState().BackFace.StencilFailOp);
				depthStencilState.BackFace.StencilPassOp = DX12::getStencilOp(rasterizer.depthStencilState().stencilState().BackFace.StencilPassOp);
			}
			else
			{
				// Setup target formats.
				UInt32 target = i++;
				rtvFormats[target] = DX12::getFormat(renderTarget.format());

				// Setup the blend state.
				auto& targetBlendState = blendState.RenderTarget[target];
				targetBlendState.BlendEnable = renderTarget.blendState().Enable;
				targetBlendState.RenderTargetWriteMask = static_cast<D3D12_COLOR_WRITE_ENABLE>(renderTarget.blendState().WriteMask);
				targetBlendState.SrcBlend = DX12::getBlendFactor(renderTarget.blendState().SourceColor);
				targetBlendState.SrcBlendAlpha = DX12::getBlendFactor(renderTarget.blendState().SourceAlpha);
				targetBlendState.DestBlend = DX12::getBlendFactor(renderTarget.blendState().DestinationColor);
				targetBlendState.DestBlendAlpha = DX12::getBlendFactor(renderTarget.blendState().DestinationAlpha);
				targetBlendState.BlendOp = DX12::getBlendOperation(renderTarget.blendState().ColorOperation);
				targetBlendState.BlendOpAlpha = DX12::getBlendOperation(renderTarget.blendState().AlphaOperation);

				// TODO: We should also implement this, but this restricts all blend states to be equal and IndependentBlendEnable set to false.
				targetBlendState.LogicOp = D3D12_LOGIC_OP::D3D12_LOGIC_OP_COPY;
				targetBlendState.LogicOpEnable = FALSE;
			}
		});

		blendState.AlphaToCoverageEnable = m_alphaToCoverage;
		blendState.IndependentBlendEnable = TRUE;

		// Initialize the remainder depending on the pipeline type.
		if (hasMeshShaders)
			return this->initializeMeshPipeline(blendState, rasterizerState, depthStencilState, topologyType, renderTargets, rtvFormats, dsvFormat, multisamplingState);
		else
			return this->initializeGraphicsPipeline(blendState, rasterizerState, depthStencilState, inputLayout, topologyType, renderTargets, rtvFormats, dsvFormat, multisamplingState);
	}

	ComPtr<ID3D12PipelineState> initializeMeshPipeline(const D3D12_BLEND_DESC& blendState, const D3D12_RASTERIZER_DESC& rasterizerState, const D3D12_DEPTH_STENCIL_DESC& depthStencilState, D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType, UINT renderTargets, const std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT>& renderTargetFormats, DXGI_FORMAT depthStencilFormat, const DXGI_SAMPLE_DESC& multisamplingState)
	{
		// Create a pipeline state description.
		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC pipelineStateDescription = {
			.pRootSignature = std::as_const(*m_layout).handle().Get(),
			.BlendState = blendState,
			.SampleMask = std::numeric_limits<UInt32>::max(),
			.RasterizerState = rasterizerState,
			.DepthStencilState = depthStencilState,
			.PrimitiveTopologyType = topologyType,
			.NumRenderTargets = renderTargets,
			.DSVFormat = depthStencilFormat,
			.SampleDesc = multisamplingState
		};

		std::memcpy(&pipelineStateDescription.RTVFormats, renderTargetFormats.data(), sizeof(DXGI_FORMAT) * D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);

		// Setup shader stages.
		auto modules = m_program->modules();
		LITEFX_TRACE(DIRECTX12_LOG, "Using shader program {0} with {1} modules...", fmt::ptr(m_program.get()), modules.size());

		std::ranges::for_each(modules, [&, i = 0](const DirectX12ShaderModule* shaderModule) mutable {
			LITEFX_TRACE(DIRECTX12_LOG, "\tModule {0}/{1} (\"{2}\") state: {{ Type: {3}, EntryPoint: {4} }}", ++i, modules.size(), shaderModule->fileName(), shaderModule->type(), shaderModule->entryPoint());

			switch (shaderModule->type())
			{
			case ShaderStage::Fragment:
				pipelineStateDescription.PS.pShaderBytecode = shaderModule->handle()->GetBufferPointer();
				pipelineStateDescription.PS.BytecodeLength = shaderModule->handle()->GetBufferSize();
				break;
			case ShaderStage::Task:
				pipelineStateDescription.AS.pShaderBytecode = shaderModule->handle()->GetBufferPointer();
				pipelineStateDescription.AS.BytecodeLength = shaderModule->handle()->GetBufferSize();
				break;
			case ShaderStage::Mesh:
				pipelineStateDescription.MS.pShaderBytecode = shaderModule->handle()->GetBufferPointer();
				pipelineStateDescription.MS.BytecodeLength = shaderModule->handle()->GetBufferSize();
				break;
			default:
				throw InvalidArgumentException("shaderProgram", "Trying to bind shader to unsupported shader stage '{0}'.", shaderModule->type());
			}
		});

		CD3DX12_PIPELINE_STATE_STREAM2 streamDesc(pipelineStateDescription);
		D3D12_PIPELINE_STATE_STREAM_DESC pipelineDesc = {
			.SizeInBytes = sizeof(streamDesc),
			.pPipelineStateSubobjectStream = &streamDesc
		};

		// Create the pipeline state instance.
		ComPtr<ID3D12PipelineState> pipelineState;
		raiseIfFailed(m_renderPass.device().handle()->CreatePipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState)), "Unable to create render pipeline state.");
		
#ifndef NDEBUG
		pipelineState->SetName(Widen(m_parent->name()).c_str());
#endif

		return pipelineState;
	}

	ComPtr<ID3D12PipelineState> initializeGraphicsPipeline(const D3D12_BLEND_DESC& blendState, const D3D12_RASTERIZER_DESC& rasterizerState, const D3D12_DEPTH_STENCIL_DESC& depthStencilState, const D3D12_INPUT_LAYOUT_DESC& inputLayout, D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType, UINT renderTargets, const std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT>& renderTargetFormats, DXGI_FORMAT depthStencilFormat, const DXGI_SAMPLE_DESC& multisamplingState)
	{
		// Create a pipeline state description.
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDescription = {
			.pRootSignature = std::as_const(*m_layout).handle().Get(),
			.BlendState = blendState,
			.SampleMask = std::numeric_limits<UInt32>::max(),
			.RasterizerState = rasterizerState,
			.DepthStencilState = depthStencilState,
			.InputLayout = inputLayout,
			.PrimitiveTopologyType = topologyType,
			.NumRenderTargets = renderTargets,
			.DSVFormat = depthStencilFormat,
			.SampleDesc = multisamplingState
		};

		std::memcpy(&pipelineStateDescription.RTVFormats, renderTargetFormats.data(), sizeof(DXGI_FORMAT) * D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
		
		// Setup shader stages.
		auto modules = m_program->modules();
		LITEFX_TRACE(DIRECTX12_LOG, "Using shader program {0} with {1} modules...", fmt::ptr(m_program.get()), modules.size());

		std::ranges::for_each(modules, [&, i = 0](const DirectX12ShaderModule* shaderModule) mutable {
			LITEFX_TRACE(DIRECTX12_LOG, "\tModule {0}/{1} (\"{2}\") state: {{ Type: {3}, EntryPoint: {4} }}", ++i, modules.size(), shaderModule->fileName(), shaderModule->type(), shaderModule->entryPoint());

			switch (shaderModule->type())
			{
			case ShaderStage::Vertex:
				pipelineStateDescription.VS.pShaderBytecode = shaderModule->handle()->GetBufferPointer();
				pipelineStateDescription.VS.BytecodeLength = shaderModule->handle()->GetBufferSize();
				break;
			case ShaderStage::TessellationControl:		// aka. Hull Shader
				pipelineStateDescription.HS.pShaderBytecode = shaderModule->handle()->GetBufferPointer();
				pipelineStateDescription.HS.BytecodeLength = shaderModule->handle()->GetBufferSize();
				break;
			case ShaderStage::TessellationEvaluation:	// aka. Domain Shader
				pipelineStateDescription.DS.pShaderBytecode = shaderModule->handle()->GetBufferPointer();
				pipelineStateDescription.DS.BytecodeLength = shaderModule->handle()->GetBufferSize();
				break;
			case ShaderStage::Geometry:
				pipelineStateDescription.GS.pShaderBytecode = shaderModule->handle()->GetBufferPointer();
				pipelineStateDescription.GS.BytecodeLength = shaderModule->handle()->GetBufferSize();
				break;
			case ShaderStage::Fragment:
				pipelineStateDescription.PS.pShaderBytecode = shaderModule->handle()->GetBufferPointer();
				pipelineStateDescription.PS.BytecodeLength = shaderModule->handle()->GetBufferSize();
				break;
			default:
				throw InvalidArgumentException("shaderProgram", "Trying to bind shader to unsupported shader stage '{0}'.", shaderModule->type());
			}
		});

		// Create the pipeline state instance.
		ComPtr<ID3D12PipelineState> pipelineState;
		raiseIfFailed(m_renderPass.device().handle()->CreateGraphicsPipelineState(&pipelineStateDescription, IID_PPV_ARGS(&pipelineState)), "Unable to create render pipeline state.");
		
#ifndef NDEBUG
		pipelineState->SetName(Widen(m_parent->name()).c_str());
#endif

		return pipelineState;
	}
};

// ------------------------------------------------------------------------------------------------
// Interface.
// ------------------------------------------------------------------------------------------------

DirectX12RenderPipeline::DirectX12RenderPipeline(const DirectX12RenderPass& renderPass, SharedPtr<DirectX12PipelineLayout> layout, SharedPtr<DirectX12ShaderProgram> shaderProgram, SharedPtr<DirectX12InputAssembler> inputAssembler, SharedPtr<DirectX12Rasterizer> rasterizer, const bool enableAlphaToCoverage, const String& name) :
	m_impl(makePimpl<DirectX12RenderPipelineImpl>(this, renderPass, enableAlphaToCoverage, layout, shaderProgram, inputAssembler, rasterizer)), DirectX12PipelineState(nullptr)
{
	if (!name.empty())
		this->name() = name;

	this->handle() = m_impl->initialize();
}

DirectX12RenderPipeline::DirectX12RenderPipeline(const DirectX12RenderPass& renderPass, const String& name) noexcept :
	m_impl(makePimpl<DirectX12RenderPipelineImpl>(this, renderPass)), DirectX12PipelineState(nullptr)
{
	if (!name.empty())
		this->name() = name;
}

DirectX12RenderPipeline::~DirectX12RenderPipeline() noexcept = default;

SharedPtr<const DirectX12ShaderProgram> DirectX12RenderPipeline::program() const noexcept
{
	return m_impl->m_program;
}

SharedPtr<const DirectX12PipelineLayout> DirectX12RenderPipeline::layout() const noexcept
{
	return m_impl->m_layout;
}

SharedPtr<DirectX12InputAssembler> DirectX12RenderPipeline::inputAssembler() const noexcept
{
	return m_impl->m_inputAssembler;
}

SharedPtr<DirectX12Rasterizer> DirectX12RenderPipeline::rasterizer() const noexcept
{
	return m_impl->m_rasterizer;
}

bool DirectX12RenderPipeline::alphaToCoverage() const noexcept
{
	return m_impl->m_alphaToCoverage;
}

void DirectX12RenderPipeline::use(const DirectX12CommandBuffer& commandBuffer) const noexcept
{
	commandBuffer.handle()->SetPipelineState(this->handle().Get());
	commandBuffer.handle()->SetGraphicsRootSignature(std::as_const(*m_impl->m_layout).handle().Get());
	commandBuffer.handle()->IASetPrimitiveTopology(DX12::getPrimitiveTopology(m_impl->m_inputAssembler->topology()));
}

#if defined(LITEFX_BUILD_DEFINE_BUILDERS)
// ------------------------------------------------------------------------------------------------
// Builder interface.
// ------------------------------------------------------------------------------------------------

constexpr DirectX12RenderPipelineBuilder::DirectX12RenderPipelineBuilder(const DirectX12RenderPass& renderPass, const String& name) :
	RenderPipelineBuilder(UniquePtr<DirectX12RenderPipeline>(new DirectX12RenderPipeline(renderPass)))
{
	this->instance()->name() = name;
}

constexpr DirectX12RenderPipelineBuilder::~DirectX12RenderPipelineBuilder() noexcept = default;

void DirectX12RenderPipelineBuilder::build()
{
	auto instance = this->instance();
	instance->m_impl->m_layout = m_state.pipelineLayout;
	instance->m_impl->m_program = m_state.shaderProgram;
	instance->m_impl->m_inputAssembler = m_state.inputAssembler;
	instance->m_impl->m_rasterizer = m_state.rasterizer;
	instance->m_impl->m_alphaToCoverage = m_state.enableAlphaToCoverage;
	instance->handle() = instance->m_impl->initialize();
}
#endif // defined(LITEFX_BUILD_DEFINE_BUILDERS)