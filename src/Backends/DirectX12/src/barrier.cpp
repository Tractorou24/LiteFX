#include <litefx/backends/dx12.hpp>

using namespace LiteFX::Rendering::Backends;

using GlobalBarrier = Tuple<ResourceAccess, ResourceAccess>;
using BufferBarrier = Tuple<ResourceAccess, ResourceAccess, IDirectX12Buffer&, UInt32>;
using ImageBarrier  = Tuple<ResourceAccess, ResourceAccess, IDirectX12Image&, ImageLayout, UInt32, UInt32, UInt32, UInt32, UInt32>;

// ------------------------------------------------------------------------------------------------
// Implementation.
// ------------------------------------------------------------------------------------------------

class DirectX12Barrier::DirectX12BarrierImpl : public Implement<DirectX12Barrier> {
public:
	friend class DirectX12Barrier;

private:
	PipelineStage m_syncBefore, m_syncAfter;
	Array<GlobalBarrier> m_globalBarriers;
	Array<BufferBarrier> m_bufferBarriers;
	Array<ImageBarrier> m_imageBarriers;

public:
	DirectX12BarrierImpl(DirectX12Barrier* parent, const PipelineStage& syncBefore, const PipelineStage& syncAfter) :
		base(parent), m_syncBefore(syncBefore), m_syncAfter(syncAfter)
	{
	}
};

// ------------------------------------------------------------------------------------------------
// Shared interface.
// ------------------------------------------------------------------------------------------------

DirectX12Barrier::DirectX12Barrier(const PipelineStage& syncBefore, const PipelineStage& syncAfter) noexcept :
	m_impl(makePimpl<DirectX12BarrierImpl>(this, syncBefore, syncAfter))
{
}

DirectX12Barrier::~DirectX12Barrier() noexcept = default;

const PipelineStage& DirectX12Barrier::syncBefore() const noexcept
{
	return m_impl->m_syncBefore;
}

const PipelineStage& DirectX12Barrier::syncAfter() const noexcept
{
	return m_impl->m_syncAfter;
}

void DirectX12Barrier::wait(const ResourceAccess& accessBefore, const ResourceAccess& accessAfter) noexcept
{
	m_impl->m_globalBarriers.push_back({ accessBefore, accessAfter });
}

void DirectX12Barrier::transition(IDirectX12Buffer& buffer, const ResourceAccess& accessBefore, const ResourceAccess& accessAfter)
{
	m_impl->m_bufferBarriers.push_back({ accessBefore, accessAfter, buffer, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES });
}

void DirectX12Barrier::transition(IDirectX12Buffer& buffer, const UInt32& element, const ResourceAccess& accessBefore, const ResourceAccess& accessAfter)
{
	m_impl->m_bufferBarriers.push_back({ accessBefore, accessAfter, buffer, element });
}

void DirectX12Barrier::transition(IDirectX12Image& image, const ResourceAccess& accessBefore, const ResourceAccess& accessAfter, const ImageLayout& layout)
{
	m_impl->m_imageBarriers.push_back({ accessBefore, accessAfter, image, layout, 0, image.levels(), 0, image.layers(), 0 });
}

void DirectX12Barrier::transition(IDirectX12Image& image, const UInt32& level, const UInt32& levels, const UInt32& layer, const UInt32& layers, const UInt32& plane, const ResourceAccess& accessBefore, const ResourceAccess& accessAfter, const ImageLayout& layout)
{
	m_impl->m_imageBarriers.push_back({ accessBefore, accessAfter, image, layout, level, levels, layer, layers, plane });
}

void DirectX12Barrier::execute(const DirectX12CommandBuffer& commandBuffer) const
{
	auto syncBefore = DX12::getPipelineStage(m_impl->m_syncBefore);
	auto syncAfter  = DX12::getPipelineStage(m_impl->m_syncAfter);

	// Global barriers.
	auto globalBarriers = m_impl->m_globalBarriers | std::views::transform([this, &syncBefore, &syncAfter](auto& barrier) { 
		return CD3DX12_GLOBAL_BARRIER(syncBefore, syncAfter, DX12::getResourceAccess(std::get<0>(barrier)), DX12::getResourceAccess(std::get<1>(barrier)));
	}) | ranges::to<Array<D3D12_GLOBAL_BARRIER>>();

	// Buffer barriers.
	auto bufferBarriers = m_impl->m_bufferBarriers | std::views::transform([this, &syncBefore, &syncAfter](auto& barrier) {
		return CD3DX12_BUFFER_BARRIER(syncBefore, syncAfter, DX12::getResourceAccess(std::get<0>(barrier)), DX12::getResourceAccess(std::get<1>(barrier)), std::as_const(std::get<2>(barrier)).handle().Get());
	}) | ranges::to<Array<D3D12_BUFFER_BARRIER>>();

	// Image barriers.
	auto imageBarriers = m_impl->m_imageBarriers | std::views::transform([this, &syncBefore, &syncAfter](auto& barrier) {
		auto& image = std::get<2>(barrier);
		auto layout = image.layout(image.subresourceId(std::get<4>(barrier), std::get<6>(barrier), std::get<8>(barrier)));
		auto currentLayout = DX12::getImageLayout(layout);
		auto targetLayout = DX12::getImageLayout(std::get<3>(barrier));

		for (auto layer = std::get<6>(barrier); layer < std::get<7>(barrier); layer++)
		{
			for (auto level = std::get<4>(barrier); level < std::get<5>(barrier); level++)
			{
				auto subresource = image.subresourceId(level, layer, std::get<8>(barrier));

				if (image.layout(subresource) != layout) [[unlikely]]
					throw RuntimeException("All sub-resources in a sub-resource range need to have the same initial layout.");
				else
					image.layout(subresource) = std::get<3>(barrier);
			}
		}

		return CD3DX12_TEXTURE_BARRIER(syncBefore, syncAfter, DX12::getResourceAccess(std::get<0>(barrier)), DX12::getResourceAccess(std::get<1>(barrier)), currentLayout, targetLayout, std::as_const(image).handle().Get(), 
			CD3DX12_BARRIER_SUBRESOURCE_RANGE(std::get<4>(barrier), std::get<5>(barrier), std::get<6>(barrier), std::get<7>(barrier), std::get<8>(barrier)));
	}) | ranges::to<Array<D3D12_TEXTURE_BARRIER>>();

	// Put all into a buffer group.
	Array<D3D12_BARRIER_GROUP> barrierGroups;

	if (!globalBarriers.empty())
		barrierGroups.push_back(CD3DX12_BARRIER_GROUP(globalBarriers.size(), globalBarriers.data()));

	if (!bufferBarriers.empty())
		barrierGroups.push_back(CD3DX12_BARRIER_GROUP(bufferBarriers.size(), bufferBarriers.data()));

	if (!imageBarriers.empty())
		barrierGroups.push_back(CD3DX12_BARRIER_GROUP(imageBarriers.size(), imageBarriers.data()));

	commandBuffer.handle()->Barrier(barrierGroups.size(), barrierGroups.data());
}