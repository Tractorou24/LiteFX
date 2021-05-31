#include <litefx/backends/dx12.hpp>

using namespace LiteFX::Rendering::Backends;

// ------------------------------------------------------------------------------------------------
// Implementation.
// ------------------------------------------------------------------------------------------------

class DirectX12VertexBufferLayout::DirectX12VertexBufferLayoutImpl : public Implement<DirectX12VertexBufferLayout> {
public:
    friend class DirectX12VertexBufferLayoutBuilder;
    friend class DirectX12VertexBufferLayout;

private:
    Array<UniquePtr<BufferAttribute>> m_attributes;
    size_t m_vertexSize;
    UInt32 m_binding;
    BufferType m_bufferType{ BufferType::Vertex };

public:
    DirectX12VertexBufferLayoutImpl(DirectX12VertexBufferLayout* parent, const size_t& vertexSize, const UInt32& binding) :
        base(parent), m_vertexSize(vertexSize), m_binding(binding)
    {
    }
};

// ------------------------------------------------------------------------------------------------
// Shared interface.
// ------------------------------------------------------------------------------------------------

DirectX12VertexBufferLayout::DirectX12VertexBufferLayout(const DirectX12InputAssembler& inputAssembler, const size_t& vertexSize, const UInt32& binding) :
    m_impl(makePimpl<DirectX12VertexBufferLayoutImpl>(this, vertexSize, binding)), DirectX12RuntimeObject(inputAssembler, inputAssembler.getDevice())
{
}

DirectX12VertexBufferLayout::~DirectX12VertexBufferLayout() noexcept = default;

size_t DirectX12VertexBufferLayout::elementSize() const noexcept
{
    return m_impl->m_vertexSize;
}

const UInt32& DirectX12VertexBufferLayout::binding() const noexcept
{
    return m_impl->m_binding;
}

const BufferType& DirectX12VertexBufferLayout::type() const noexcept
{
    return m_impl->m_bufferType;
}

Array<const BufferAttribute*> DirectX12VertexBufferLayout::attributes() const noexcept
{
    return m_impl->m_attributes |
        std::views::transform([](const UniquePtr<BufferAttribute>& attribute) { return attribute.get(); }) |
        ranges::to<Array<const BufferAttribute*>>();
}

// ------------------------------------------------------------------------------------------------
// Builder interface.
// ------------------------------------------------------------------------------------------------

DirectX12VertexBufferLayoutBuilder& DirectX12VertexBufferLayoutBuilder::addAttribute(UniquePtr<BufferAttribute>&& attribute)
{
    this->instance()->m_impl->m_attributes.push_back(std::move(attribute));
    return *this;
}

DirectX12VertexBufferLayoutBuilder& DirectX12VertexBufferLayoutBuilder::addAttribute(const BufferFormat& format, const UInt32& offset, const AttributeSemantic& semantic, const UInt32& semanticIndex)
{
    return this->addAttribute(std::move(makeUnique<BufferAttribute>(static_cast<UInt32>(this->instance()->attributes().size()), offset, format, semantic, semanticIndex)));
}

DirectX12VertexBufferLayoutBuilder& DirectX12VertexBufferLayoutBuilder::addAttribute(const UInt32& location, const BufferFormat& format, const UInt32& offset, const AttributeSemantic& semantic, const UInt32& semanticIndex)
{
    return this->addAttribute(std::move(makeUnique<BufferAttribute>(location, offset, format, semantic, semanticIndex)));
}