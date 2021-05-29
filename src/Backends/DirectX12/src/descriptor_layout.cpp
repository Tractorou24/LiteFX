#include <litefx/backends/dx12.hpp>

using namespace LiteFX::Rendering::Backends;

// ------------------------------------------------------------------------------------------------
// Implementation.
// ------------------------------------------------------------------------------------------------

class DirectX12DescriptorLayout::DirectX12DescriptorLayoutImpl : public Implement<DirectX12DescriptorLayout> {
public:
    friend class DirectX12DescriptorLayout;

private:
    size_t m_elementSize;
    UInt32 m_binding;
    DescriptorType m_descriptorType;
    BufferType m_bufferType;

public:
    DirectX12DescriptorLayoutImpl(DirectX12DescriptorLayout* parent, const DescriptorType& type, const UInt32& binding, const size_t& elementSize) :
        base(parent), m_descriptorType(type), m_binding(binding), m_elementSize(elementSize)
    {
        switch (m_descriptorType)
        {
        case DescriptorType::Uniform: m_bufferType = BufferType::Uniform; break;
        case DescriptorType::Storage: m_bufferType = BufferType::Storage; break;
        default: m_bufferType = BufferType::Other; break;
        }
    }
};

// ------------------------------------------------------------------------------------------------
// Shared interface.
// ------------------------------------------------------------------------------------------------

DirectX12DescriptorLayout::DirectX12DescriptorLayout(const DirectX12DescriptorSetLayout& descriptorSetLayout, const DescriptorType& type, const UInt32& binding, const size_t& elementSize) :
    m_impl(makePimpl<DirectX12DescriptorLayoutImpl>(this, type, binding, elementSize)), DirectX12RuntimeObject(descriptorSetLayout, descriptorSetLayout.getDevice())
{
}

DirectX12DescriptorLayout::~DirectX12DescriptorLayout() noexcept = default;

size_t DirectX12DescriptorLayout::elementSize() const noexcept
{
    return m_impl->m_elementSize;
}

const UInt32& DirectX12DescriptorLayout::binding() const noexcept
{
    return m_impl->m_binding;
}

const BufferType& DirectX12DescriptorLayout::type() const noexcept
{
    return m_impl->m_bufferType;
}

const DescriptorType& DirectX12DescriptorLayout::descriptorType() const noexcept
{
    return m_impl->m_descriptorType;
}