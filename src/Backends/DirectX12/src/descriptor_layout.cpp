#include <litefx/backends/dx12.hpp>
#include <array>

using namespace LiteFX::Rendering::Backends;

// ------------------------------------------------------------------------------------------------
// Implementation.
// ------------------------------------------------------------------------------------------------

class DirectX12DescriptorSetLayout::DirectX12DescriptorSetLayoutImpl : public Implement<DirectX12DescriptorSetLayout> {
public:
    friend class DirectX12DescriptorSetLayoutBuilder;
    friend class DirectX12DescriptorSetLayout;

private:
    UInt32 m_setId;
    ShaderStage m_stages;
    Dictionary<DescriptorType, UInt32> m_poolSizes = {
        { DescriptorType::Uniform, 0 },
        { DescriptorType::Storage, 0 },
        { DescriptorType::Image, 0 },
        { DescriptorType::Sampler, 0 },
        { DescriptorType::InputAttachment, 0 }
    };
    Array<UniquePtr<IDescriptorLayout>> m_layouts;

public:
    DirectX12DescriptorSetLayoutImpl(DirectX12DescriptorSetLayout* parent, const UInt32& id, const ShaderStage& stages) : 
        base(parent), m_setId(id), m_stages(stages) { }

public:
    void initialize()
    {
        throw;
        //LITEFX_TRACE(VULKAN_LOG, "Defining layout for descriptor set {0} {{ Stages: {1} }}...", m_setId, m_stages);

        //// Parse the shader stage descriptor.
        //VkShaderStageFlags shaderStages = {};

        //if ((m_stages & ShaderStage::Vertex) == ShaderStage::Vertex)
        //    shaderStages |= VK_SHADER_STAGE_VERTEX_BIT;
        //if ((m_stages & ShaderStage::Geometry) == ShaderStage::Geometry)
        //    shaderStages |= VK_SHADER_STAGE_GEOMETRY_BIT;
        //if ((m_stages & ShaderStage::Fragment) == ShaderStage::Fragment)
        //    shaderStages |= VK_SHADER_STAGE_FRAGMENT_BIT;
        //if ((m_stages & ShaderStage::TessellationEvaluation) == ShaderStage::TessellationEvaluation)
        //    shaderStages |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        //if ((m_stages & ShaderStage::TessellationControl) == ShaderStage::TessellationControl)
        //    shaderStages |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        //if ((m_stages & ShaderStage::Compute) == ShaderStage::Compute)
        //    shaderStages |= VK_SHADER_STAGE_COMPUTE_BIT;

        //// Parse descriptor set layouts.
        //Array<VkDescriptorSetLayoutBinding> bindings;

        //std::for_each(std::begin(m_layouts), std::end(m_layouts), [&, i = 0](const UniquePtr<IDescriptorLayout>& layout) mutable {
        //    auto bindingPoint = layout->getBinding();
        //    auto type = layout->getDescriptorType();

        //    LITEFX_TRACE(VULKAN_LOG, "\tWith descriptor {0}/{1} {{ Type: {2}, Element size: {3} bytes, Offset: {4}, Binding point: {5} }}...", ++i, m_layouts.size(), type, layout->getElementSize(), 0, bindingPoint);

        //    VkDescriptorSetLayoutBinding binding = {};
        //    binding.binding = bindingPoint;
        //    binding.descriptorCount = 1;		// TODO: Implement support for uniform buffer arrays.
        //    binding.pImmutableSamplers = nullptr;
        //    binding.stageFlags = shaderStages;

        //    switch (type)
        //    {
        //    case DescriptorType::Uniform:         binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;   break;
        //    case DescriptorType::Storage:         binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;   break;
        //    case DescriptorType::Image:           binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;    break;
        //    case DescriptorType::Sampler:         binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;          break;
        //    case DescriptorType::InputAttachment: binding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; break;
        //    default: LITEFX_WARNING(VULKAN_LOG, "The descriptor type is unsupported. Binding will be skipped.");
        //    }

        //    m_poolSizes[type]++;
        //    bindings.push_back(binding);
        //});

        //LITEFX_TRACE(VULKAN_LOG, "Creating descriptor set {0} layout with {1} bindings {{ Uniform: {2}, Storage: {3}, Images: {4}, Sampler: {5}, Input attachments: {6} }}...", m_setId, m_layouts.size(), m_poolSizes[DescriptorType::Uniform], m_poolSizes[DescriptorType::Storage], m_poolSizes[DescriptorType::Image], m_poolSizes[DescriptorType::Sampler], m_poolSizes[DescriptorType::InputAttachment]);

        //VkDescriptorSetLayoutCreateInfo uniformBufferLayoutInfo{};
        //uniformBufferLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        //uniformBufferLayoutInfo.bindingCount = bindings.size();
        //uniformBufferLayoutInfo.pBindings = bindings.data();

        //VkDescriptorSetLayout layout;

        //if (::vkCreateDescriptorSetLayout(m_parent->getDevice()->handle(), &uniformBufferLayoutInfo, nullptr, &layout) != VK_SUCCESS)
        //    throw std::runtime_error("Unable to create descriptor set layout.");

        //return layout;
    }

public:
    Array<const IDescriptorLayout*> getLayouts() const noexcept
    {
        Array<const IDescriptorLayout*> layouts(m_layouts.size());
        std::generate(std::begin(layouts), std::end(layouts), [&, i = 0]() mutable { return m_layouts[i++].get(); });

        return layouts;
    }

    const IDescriptorLayout* getLayout(const UInt32& binding) const noexcept
    {
        auto layout = std::find_if(std::begin(m_layouts), std::end(m_layouts), [&](const UniquePtr<IDescriptorLayout>& layout) { return layout->getBinding() == binding; });

        return layout == m_layouts.end() ? nullptr : layout->get();
    }
};

// ------------------------------------------------------------------------------------------------
// Shared interface.
// ------------------------------------------------------------------------------------------------

DirectX12DescriptorSetLayout::DirectX12DescriptorSetLayout(const DirectX12RenderPipelineLayout& pipelineLayout, const UInt32& id, const ShaderStage& stages) :
    m_impl(makePimpl<DirectX12DescriptorSetLayoutImpl>(this, id, stages))
{
}

DirectX12DescriptorSetLayout::~DirectX12DescriptorSetLayout() noexcept = default;

Array<const IDescriptorLayout*> DirectX12DescriptorSetLayout::getLayouts() const noexcept
{
    return m_impl->getLayouts();
}

const IDescriptorLayout* DirectX12DescriptorSetLayout::getLayout(const UInt32& binding) const noexcept
{
    return m_impl->getLayout(binding);
}

const UInt32& DirectX12DescriptorSetLayout::getSetId() const noexcept
{
    return m_impl->m_setId;
}

const ShaderStage& DirectX12DescriptorSetLayout::getShaderStages() const noexcept
{
    return m_impl->m_stages;
}

UniquePtr<IDescriptorSet> DirectX12DescriptorSetLayout::createBufferPool() const noexcept
{
    //return makeUnique<DirectX12DescriptorSet>(*this);
    throw;
}

UInt32 DirectX12DescriptorSetLayout::uniforms() const noexcept
{
    return m_impl->m_poolSizes[DescriptorType::Uniform];
}

UInt32 DirectX12DescriptorSetLayout::storages() const noexcept
{
    return m_impl->m_poolSizes[DescriptorType::Storage];
}

UInt32 DirectX12DescriptorSetLayout::images() const noexcept
{
    return m_impl->m_poolSizes[DescriptorType::Image];
}

UInt32 DirectX12DescriptorSetLayout::samplers() const noexcept
{
    return m_impl->m_poolSizes[DescriptorType::Sampler];
}

UInt32 DirectX12DescriptorSetLayout::inputAttachments() const noexcept
{
    return m_impl->m_poolSizes[DescriptorType::InputAttachment];
}

// ------------------------------------------------------------------------------------------------
// Builder interface.
// ------------------------------------------------------------------------------------------------

DirectX12RenderPipelineLayoutBuilder& DirectX12DescriptorSetLayoutBuilder::go()
{
    this->instance()->m_impl->initialize();

    return DescriptorSetLayoutBuilder::go();
}


DirectX12DescriptorSetLayoutBuilder& DirectX12DescriptorSetLayoutBuilder::addDescriptor(UniquePtr<IDescriptorLayout>&& layout)
{
    if (this->instance()->getShaderStages() != ShaderStage::Fragment && layout->getDescriptorType() == DescriptorType::InputAttachment)
        throw std::invalid_argument("Input attachments must only be used from the fragment stage.");

    this->instance()->m_impl->m_layouts.push_back(std::move(layout));
    return *this;
}

DirectX12DescriptorSetLayoutBuilder& DirectX12DescriptorSetLayoutBuilder::addDescriptor(const DescriptorType& type, const UInt32& binding, const UInt32& descriptorSize)
{
    //UniquePtr<IDescriptorLayout> layout = makeUnique<DirectX12DescriptorLayout>(*(this->instance()), type, binding, descriptorSize);
    //return this->addDescriptor(std::move(layout));
    throw;
}