#pragma once

#include <litefx/rendering_api.hpp>

namespace LiteFX::Rendering {
    using namespace LiteFX;
    using namespace LiteFX::Math;

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API BufferAttribute {
        LITEFX_IMPLEMENTATION(BufferAttributeImpl)

    public:
        BufferAttribute() noexcept;
        BufferAttribute(const UInt32& location, const UInt32& binding, const UInt32& offset, const BufferFormat& format) noexcept;
        BufferAttribute(BufferAttribute&&) noexcept;
        BufferAttribute(const BufferAttribute&) noexcept;
        virtual ~BufferAttribute() noexcept;

    public:
        virtual const UInt32& getLocation() const noexcept;
        virtual const UInt32& getBinding() const noexcept;
        virtual const BufferFormat& getFormat() const noexcept;
        virtual const UInt32& getOffset() const noexcept;
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API BufferLayout {
        LITEFX_IMPLEMENTATION(BufferLayoutImpl)

    public:
        BufferLayout() noexcept;
        BufferLayout(BufferLayout&&) = delete;
        BufferLayout(const BufferLayout&) = delete;
        virtual ~BufferLayout() noexcept;

    public:
        virtual void add(UniquePtr<BufferAttribute>&& attribute);
        virtual void remove(const BufferAttribute* attribute);
        virtual Array<const BufferAttribute*> getAttributes() const noexcept;
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API IBuffer {
    public:
        virtual ~IBuffer() noexcept = default;

    public:
        virtual const BufferLayout* getLayout() const noexcept = 0;
    };

    /// <summary>
    /// 
    /// </summary>
    template <typename T>
    class Buffer : public IBuffer {
    private:
        Array<T> m_elements;

    public:
        Buffer(Array<T>&& elements) noexcept : m_elements(std::move(_other.m_elements)) {}
        Buffer(const Buffer& _other) noexcept : m_elements(std::copy(_other.m_elements)) {}
        Buffer(Buffer&& _other) noexcept : m_elements(std::move(_other.m_elements)) {}
        virtual ~Buffer() noexcept = default;

    public:
        virtual const Array<T>& getElements() const noexcept { return m_elements; }
        virtual Array<T>& elements() noexcept = 0 { return m_elements; }
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API IRenderPipeline {
    public:
        virtual ~IRenderPipeline() noexcept = default;
    
    public:
        virtual const IRenderPipelineLayout* getLayout() const noexcept = 0;
        virtual void use(UniquePtr<IRenderPipelineLayout>&& layout) = 0;
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API IRenderPipelineLayout {
    public:
        virtual ~IRenderPipelineLayout() noexcept = default;

    public:
        virtual Array<const IViewport*> getViewports() const noexcept = 0;
        virtual void use(UniquePtr<IViewport>&& viewport) = 0;
        virtual UniquePtr<IViewport> remove(const IViewport* viewport) noexcept = 0;
        virtual const IRasterizer* getRasterizer() const noexcept = 0;
        virtual void use(UniquePtr<IRasterizer>&& rasterizer) = 0;
        virtual const IShaderProgram* getProgram() const noexcept = 0;
        virtual void use(UniquePtr<IShaderProgram>&& program) = 0;
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API IShaderModule {
    public:
        virtual ~IShaderModule() noexcept = default;

    public:
        virtual const IGraphicsDevice* getDevice() const noexcept = 0;
        virtual const ShaderType& getType() const noexcept = 0;
        virtual const String& getFileName() const noexcept = 0;
        virtual const String& getEntryPoint() const noexcept = 0;
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API IShaderProgram {
    public:
        virtual ~IShaderProgram() noexcept = default;
    
    public:
        virtual Array<const IShaderModule*> getModules() const noexcept = 0;
        virtual void use(UniquePtr<IShaderModule>&& module) = 0;
        virtual void remove(const IShaderModule* module) const noexcept = 0;
    };

    // TODO: Rename to ShaderProgramBuilder
    //class LITEFX_RENDERING_API RenderPipelineBuilder : public Builder<IRenderPipeline> {
    //    LITEFX_IMPLEMENTATION(RenderPipelineBuilderImpl)

    //public:
    //    explicit RenderPipelineBuilder(const IGraphicsDevice* device, const String& name);
    //    RenderPipelineBuilder(const RenderPipelineBuilder&&) = delete;
    //    RenderPipelineBuilder(RenderPipelineBuilder&) = delete;
    //    virtual ~RenderPipelineBuilder() noexcept;

    //public:
    //    virtual const IGraphicsDevice* getDevice() const noexcept;
    //    virtual const String& getName() const noexcept;
    //    virtual Array<UniquePtr<IShaderModule>> shaderModules() noexcept;

    //public:
    //    virtual RenderPipelineBuilder& addShaderModule(const ShaderType& type, const String& fileName, const String& entryPoint = "main");
    //    virtual RenderPipelineBuilder& addVertexShaderModule(const String& fileName, const String& entryPoint = "main");
    //    virtual RenderPipelineBuilder& addTessellationControlShaderModule(const String& fileName, const String& entryPoint = "main");
    //    virtual RenderPipelineBuilder& addTessellationEvaluationShaderModule(const String& fileName, const String& entryPoint = "main");
    //    virtual RenderPipelineBuilder& addGeometryShaderModule(const String& fileName, const String& entryPoint = "main");
    //    virtual RenderPipelineBuilder& addFragmentShaderModule(const String& fileName, const String& entryPoint = "main");
    //    virtual RenderPipelineBuilder& addComputeShaderModule(const String& fileName, const String& entryPoint = "main");
    //};

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API IInputAssembler {
    public:
        virtual ~IInputAssembler() noexcept = default;

    public:
        virtual const BufferLayout* getLayout() const = 0;
        virtual void use(UniquePtr<BufferLayout>&& layout) = 0;
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API IRasterizer {
    public:
        virtual ~IRasterizer() noexcept = default;

    public:
        virtual PolygonMode getPolygonMode() const noexcept = 0;
        virtual void setPolygonMode(const PolygonMode& mode) noexcept = 0;
        virtual CullMode getCullMode() const noexcept = 0;
        virtual void setCullMode(const CullMode& mode) noexcept = 0;
        virtual CullOrder getCullOrder() const noexcept = 0;
        virtual void setCullOrder(const CullOrder& order) noexcept = 0;
        virtual Float getLineWidth() const noexcept = 0;
        virtual void setLineWidth(const Float& width) noexcept = 0;
        virtual bool getDepthBiasEnabled() const noexcept = 0;
        virtual void setDepthBiasEnabled(const bool& enable) noexcept = 0;
        virtual float getDepthBiasClamp() const noexcept = 0;
        virtual void setDepthBiasClamp(const float& clamp) noexcept = 0;
        virtual float getDepthBiasConstantFactor() const noexcept = 0;
        virtual void setDepthBiasConstantFactor(const float& factor) noexcept = 0;
        virtual float getDepthBiasSlopeFactor() const noexcept = 0;
        virtual void setDepthBiasSlopeFactor(const float& factor) noexcept = 0;
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API IViewport {
    public:
        virtual ~IViewport() noexcept = default;

    public:
        virtual RectF getRectangle() const noexcept = 0;
        virtual void setRectangle(const RectF& rectangle) noexcept = 0;
        virtual const Array<RectF>& getScissors() const noexcept = 0;
        virtual Array<RectF>& getScissors() noexcept = 0;
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API Rasterizer : public IRasterizer {
        LITEFX_IMPLEMENTATION(RasterizerImpl)

    public:
        Rasterizer() noexcept;
        Rasterizer(Rasterizer&&) noexcept = delete;
        Rasterizer(const Rasterizer&) noexcept = delete;
        virtual ~Rasterizer() noexcept;

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

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API InputAssembler : public IInputAssembler {
        LITEFX_IMPLEMENTATION(InputAssemblerImpl)

    public:
        InputAssembler() noexcept;
        InputAssembler(UniquePtr<BufferLayout>&&) noexcept;
        InputAssembler(InputAssembler&&) noexcept = delete;
        InputAssembler(const InputAssembler&) noexcept = delete;
        virtual ~InputAssembler() noexcept;

    public:
        virtual const BufferLayout* getLayout() const override;
        virtual void use(UniquePtr<BufferLayout>&& layout) override;
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API Viewport : public IViewport {
        LITEFX_IMPLEMENTATION(ViewportImpl)

    public:
        Viewport(const RectF& clientRect = { }) noexcept;
        Viewport(Viewport&&) noexcept = delete;
        Viewport(const Viewport&) noexcept = delete;
        virtual ~Viewport() noexcept;

    public:
        virtual RectF getRectangle() const noexcept override;
        virtual void setRectangle(const RectF& rectangle) noexcept override;
        virtual const Array<RectF>& getScissors() const noexcept override;
        virtual Array<RectF>& getScissors() noexcept override;
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API RenderPipelineLayout : public IRenderPipelineLayout {
        LITEFX_IMPLEMENTATION(RenderPipelineLayoutImpl)

    public:
        RenderPipelineLayout() noexcept;
        RenderPipelineLayout(RenderPipelineLayout&&) = delete;
        RenderPipelineLayout(const RenderPipelineLayout&) = delete;
        virtual ~RenderPipelineLayout() noexcept;

    public:
        virtual Array<const IViewport*> getViewports() const noexcept override;
        virtual void use(UniquePtr<IViewport>&& viewport) override;
        virtual UniquePtr<IViewport> remove(const IViewport* viewport) noexcept override;
        virtual const IRasterizer* getRasterizer() const noexcept override;
        virtual void use(UniquePtr<IRasterizer>&& rasterizer) override;
        virtual const IShaderProgram* getProgram() const noexcept override;
        virtual void use(UniquePtr<IShaderProgram>&& program) override;
    };

    /// <summary>
    /// 
    /// </summary>
    class LITEFX_RENDERING_API RenderPipeline : public IRenderPipeline {
        LITEFX_IMPLEMENTATION(RenderPipelineImpl)

    public:
        RenderPipeline() noexcept;
        RenderPipeline(RenderPipeline&&) noexcept = delete;
        RenderPipeline(const RenderPipeline&) noexcept = delete;
        virtual ~RenderPipeline() noexcept;

    public:
        virtual const IRenderPipelineLayout* getLayout() const noexcept override;
        virtual void use(UniquePtr<IRenderPipelineLayout>&& layout) override;
    };

}