#pragma once

#include <litefx/rendering_api.hpp>
#include <litefx/rendering_backends.hpp>
#include <litefx/rendering_pipelines.hpp>

#include <litefx/math.hpp>

namespace LiteFX::Rendering {
	using namespace LiteFX;
	using namespace LiteFX::Math;

	// Define interfaces.
	class LITEFX_RENDERING_API ITexture {
	public:
		virtual ~ITexture() noexcept = default;

	public:
		virtual Size2d getSize() const noexcept = 0;
		virtual Format getFormat() const noexcept = 0;
	};

	class LITEFX_RENDERING_API ISwapChain {
	public:
		virtual ~ISwapChain() noexcept = default;

	public:
		virtual const IGraphicsDevice* getDevice() const noexcept = 0;
	};

	class LITEFX_RENDERING_API ISurface {
	public:
		virtual ~ISurface() noexcept = default;
	};

	class LITEFX_RENDERING_API ICommandPool {
	public:
		virtual ~ICommandPool() noexcept = default;
	};

	class LITEFX_RENDERING_API ICommandQueue {
	public:
		virtual ~ICommandQueue() noexcept = default;

	public:
		virtual QueueType getType() const noexcept = 0;
	};

	class LITEFX_RENDERING_API IGraphicsDevice {
	public:
		virtual ~IGraphicsDevice() noexcept = default;

	public:
		virtual const IGraphicsAdapter* getAdapter() const noexcept= 0;
		virtual const ISurface* getSurface() const noexcept = 0;
		virtual const ISwapChain* getSwapChain() const noexcept = 0;
		virtual Array<Format> getSurfaceFormats() const = 0;

	public:
		//virtual UniquePtr<ITexture> createTexture2d(const Format& format = Format::B8G8R8A8_UNORM_SRGB, const Size2d& size = Size2d(0)) const = 0;
		virtual UniquePtr<IShaderModule> loadShaderModule(const ShaderType& type, const String& fileName, const String& entryPoint = "main") const = 0;
	};

	class LITEFX_RENDERING_API IGraphicsAdapter {
	public:
		virtual ~IGraphicsAdapter() noexcept = default;

	public:
		virtual String getName() const noexcept = 0;
		virtual uint32_t getVendorId() const noexcept = 0;
		virtual uint32_t getDeviceId() const noexcept = 0;
		virtual GraphicsAdapterType getType() const noexcept = 0;
		virtual uint32_t getDriverVersion() const noexcept = 0;
		virtual uint32_t getApiVersion() const noexcept = 0;

	public:
		virtual UniquePtr<IGraphicsDevice> createDevice(const ISurface* surface, const Format& format = Format::B8G8R8A8_UNORM_SRGB, const Array<String>& extensions = { }) const = 0;
		virtual const ICommandQueue* findQueue(const QueueType& queueType) const = 0;
	};

	// Base classes.
	class LITEFX_RENDERING_API GraphicsDevice : public IGraphicsDevice {
		LITEFX_IMPLEMENTATION(GraphicsDeviceImpl)

	public:
		GraphicsDevice(const IGraphicsAdapter* adapter, const ISurface* surface);
		GraphicsDevice(const GraphicsDevice&) noexcept = delete;
		GraphicsDevice(GraphicsDevice&&) noexcept = delete;
		virtual ~GraphicsDevice() noexcept;

	public:
		virtual const IGraphicsAdapter* getAdapter() const noexcept override;
		virtual const ISurface* getSurface() const noexcept override;
	};

}