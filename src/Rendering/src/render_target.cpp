#include <litefx/rendering.hpp>

using namespace LiteFX::Rendering;

// ------------------------------------------------------------------------------------------------
// Implementation.
// ------------------------------------------------------------------------------------------------

class RenderTarget::RenderTargetImpl : public Implement<RenderTarget> {
public:
    friend class RenderTarget;

private:
    RenderTargetType m_type = RenderTargetType::Color;
    Format m_format = Format::B8G8R8A8_SRGB;
    MultiSamplingLevel m_samples = MultiSamplingLevel::x1;
    bool m_clearBuffer = false, m_clearStencil = false, m_volatile = false;
    Vector4f m_clearValues;
    UInt32 m_location;

public:
    RenderTargetImpl(RenderTarget* parent, const UInt32& location, const RenderTargetType& type, const Format& format, const bool& clearBuffer, const Vector4f& clearValues, const bool& clearStencil, const MultiSamplingLevel& samples, const bool& isVolatile) :
        base(parent), m_location(location), m_type(type), m_format(format), m_clearBuffer(clearBuffer), m_clearValues(clearValues), m_clearStencil(clearStencil), m_samples(samples), m_volatile(isVolatile)
    {
    }
};

// ------------------------------------------------------------------------------------------------
// Interface.
// ------------------------------------------------------------------------------------------------

RenderTarget::RenderTarget() noexcept :
    m_impl(makePimpl<RenderTargetImpl>(this, 0, RenderTargetType::Color, Format::None, false, Vector4f { 0.f, 0.f, 0.f, 0.f }, false, MultiSamplingLevel::x1, false))
{
}

RenderTarget::RenderTarget(const UInt32& location, const RenderTargetType& type, const Format& format, const bool& clearBuffer, const Vector4f& clearValues, const bool& clearStencil, const MultiSamplingLevel& samples, const bool& isVolatile) :
    m_impl(makePimpl<RenderTargetImpl>(this, location, type, format, clearBuffer, clearValues, clearStencil, samples, isVolatile))
{
}

RenderTarget::RenderTarget(const RenderTarget& _other) noexcept :
    m_impl(makePimpl<RenderTargetImpl>(this, _other.location(), _other.type(), _other.format(), _other.clearBuffer(), _other.clearValues(), _other.clearStencil(), _other.samples(), _other.isVolatile()))
{
}

RenderTarget::RenderTarget(RenderTarget&& _other) noexcept :
    m_impl(makePimpl<RenderTargetImpl>(this, std::move(_other.m_impl->m_location), std::move(_other.m_impl->m_type), std::move(_other.m_impl->m_format), std::move(_other.m_impl->m_clearBuffer), std::move(_other.m_impl->m_clearValues), std::move(_other.m_impl->m_clearStencil), std::move(_other.m_impl->m_samples), std::move(_other.m_impl->m_volatile)))
{
}

RenderTarget::~RenderTarget() noexcept = default;

RenderTarget& RenderTarget::operator=(const RenderTarget& _other) noexcept
{
    m_impl->m_location = _other.m_impl->m_location;
    m_impl->m_type = _other.m_impl->m_type;
    m_impl->m_format = _other.m_impl->m_format;
    m_impl->m_clearBuffer = _other.m_impl->m_clearBuffer;
    m_impl->m_clearValues = _other.m_impl->m_clearValues;
    m_impl->m_clearStencil = _other.m_impl->m_clearStencil;
    m_impl->m_samples = _other.m_impl->m_samples;
    m_impl->m_volatile = _other.m_impl->m_volatile;

    return *this;
}

RenderTarget& RenderTarget::operator=(RenderTarget&& _other) noexcept
{
    m_impl->m_location = std::move(_other.m_impl->m_location);
    m_impl->m_type = std::move(_other.m_impl->m_type);
    m_impl->m_format = std::move(_other.m_impl->m_format);
    m_impl->m_clearBuffer = std::move(_other.m_impl->m_clearBuffer);
    m_impl->m_clearValues = std::move(_other.m_impl->m_clearValues);
    m_impl->m_clearStencil = std::move(_other.m_impl->m_clearStencil);
    m_impl->m_samples = std::move(_other.m_impl->m_samples);
    m_impl->m_volatile = std::move(_other.m_impl->m_volatile);
    
    return *this;
}

const UInt32& RenderTarget::location() const noexcept
{
    return m_impl->m_location;
}

const RenderTargetType& RenderTarget::type() const noexcept
{
    return m_impl->m_type;
}

const MultiSamplingLevel& RenderTarget::samples() const noexcept
{
    return m_impl->m_samples;
}

const Format& RenderTarget::format() const noexcept
{
    return m_impl->m_format;
}

const bool& RenderTarget::clearBuffer() const noexcept
{
    return m_impl->m_clearBuffer;
}

const bool& RenderTarget::clearStencil() const noexcept
{
    return m_impl->m_clearStencil;
}

const Vector4f& RenderTarget::clearValues() const noexcept
{
    return m_impl->m_clearValues;
}

const bool& RenderTarget::isVolatile() const noexcept
{
    return m_impl->m_volatile;
}