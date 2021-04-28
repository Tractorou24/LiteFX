#include <litefx/backends/vulkan.hpp>

using namespace LiteFX::Rendering::Backends;

// ------------------------------------------------------------------------------------------------
// Implementation.
// ------------------------------------------------------------------------------------------------

class VulkanQueue::VulkanQueueImpl : public Implement<VulkanQueue> {
public:
	friend class VulkanQueue;

private:
	const VulkanDevice* m_device;
	VkCommandPool m_commandPool{};
	QueueType m_type;
	QueuePriority m_priority;
	UInt32 m_familyId, m_queueId;
	bool m_bound;

public:
	VulkanQueueImpl(VulkanQueue* parent, const VulkanDevice* device, const QueueType& type, const QueuePriority& priority, const UInt32& familyId, const UInt32& queueId) :
		base(parent), m_type(type), m_priority(priority), m_familyId(familyId), m_queueId(queueId), m_bound(false), m_device(device) { }

	~VulkanQueueImpl()
	{
		this->release();
	}

public:
	void release()
	{
		if (m_bound)
			::vkDestroyCommandPool(m_device->handle(), m_commandPool, nullptr);

		m_bound = false;
		m_commandPool = {};
	}

	void bind()
	{
		if (m_bound)
			return;

		// Store the queue handle, if not done in previous binds.
		if (m_parent->handle() == nullptr)
			::vkGetDeviceQueue(m_device->handle(), m_familyId, m_queueId, &m_parent->handle());

		// Create command pool.
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = m_familyId;

		// Transfer pools can be transient.
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (m_type == QueueType::Transfer)
			poolInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		if (::vkCreateCommandPool(m_device->handle(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
			throw std::runtime_error("Unable to create command pool.");

		m_bound = true;
	}
};

// ------------------------------------------------------------------------------------------------
// Shared interface.
// ------------------------------------------------------------------------------------------------

VulkanQueue::VulkanQueue(const IGraphicsDevice* device, const QueueType& type, const QueuePriority& priority, const UInt32& familyId, const UInt32& queueId) :
	IResource(nullptr), m_impl(makePimpl<VulkanQueueImpl>(this, dynamic_cast<const VulkanDevice*>(device), type, priority, familyId, queueId))
{
	if (m_impl->m_device == nullptr)
		throw std::invalid_argument("The device must be initialized.");
}

VulkanQueue::~VulkanQueue() noexcept
{
	this->release();
}

void VulkanQueue::bind()
{
	m_impl->bind();
}

void VulkanQueue::release()
{
	m_impl->release();
}

VkCommandPool VulkanQueue::getCommandPool() const noexcept
{
	return m_impl->m_commandPool;
}

UInt32 VulkanQueue::getFamilyId() const noexcept
{
	return m_impl->m_familyId;
}

UInt32 VulkanQueue::getQueueId() const noexcept
{
	return m_impl->m_queueId;
}

bool VulkanQueue::isBound() const noexcept
{
	return m_impl->m_bound;
}

QueueType VulkanQueue::getType() const noexcept
{
	return m_impl->m_type;
}

QueuePriority VulkanQueue::getPriority() const noexcept
{
	return m_impl->m_priority;
}

const IGraphicsDevice* VulkanQueue::getDevice() const noexcept
{
	return m_impl->m_device;
}

UniquePtr<ICommandBuffer> VulkanQueue::createCommandBuffer() const
{
	return makeUnique<VulkanCommandBuffer>(this);
}