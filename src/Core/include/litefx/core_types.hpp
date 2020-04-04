#pragma once

#include <litefx/core.h>

#ifndef LITEFX_DEFINE_FLAGS
#  define LITEFX_DEFINE_FLAGS(T) \
	inline T operator| (const T lhs, const T rhs) { using _base_t = std::underlying_type_t<T>; return static_cast<T>(static_cast<_base_t>(lhs) | static_cast<_base_t>(rhs)); } \
	inline T& operator|= (T& lhs, const T& rhs) { lhs = lhs | rhs; return lhs; } \
	inline T operator& (const T lhs, const T rhs) { using _base_t = std::underlying_type_t<T>; return static_cast<T>(static_cast<_base_t>(lhs) & static_cast<_base_t>(rhs)); } \
	inline T& operator&= (T& lhs, const T& rhs) { lhs = lhs & rhs; return lhs; }
#endif

#ifndef LITEFX_FLAG_IS_SET
#  define LITEFX_FLAG_IS_SET(val, flag) static_cast<int>(val & flag) != 0
#endif

namespace LiteFX {

	enum class LITEFX_CORE_API Platform {
		None = 0x00000000,
		Win32 = 0x00000001,
		// TODO: ...
		Other = 0xFFFFFFFF
	};

#if (defined(BUILD_LITEFX_PIMPL) && BUILD_LITEFX_PIMPL) || (!defined(BUILD_LITEFX_PIMPL)) && !defined(LITEFX_IMPLEMENTATION)
	template <class pImpl>
	class PimplPtr {
	private:
		UniquePtr<pImpl> m_ptr;

	public:
		PimplPtr() noexcept = default;
		PimplPtr(const PimplPtr& src) noexcept : m_ptr(new pImpl(*src.m_ptr)) {}
		PimplPtr(PimplPtr&&) noexcept = default;
		PimplPtr& operator= (const PimplPtr& src) noexcept { m_ptr.reset(new pImpl(*src.m_ptr)); return *this; }
		PimplPtr& operator= (PimplPtr&&) noexcept = default;
		~PimplPtr() noexcept = default;

	private:
		PimplPtr(pImpl* pimpl) : m_ptr(pimpl) { }

	public:
		pImpl& operator* () const noexcept { return *m_ptr; }
		pImpl* operator-> () const noexcept { return m_ptr.get(); }

	public:
		template <class T, class... Arg>
		friend PimplPtr<T> makePimpl(Arg&&... arg);
	};

	template <class T, class... Arg>
	PimplPtr<T> makePimpl(Arg&&... arg) {
		return PimplPtr<T>(new T(std::forward<Arg>(arg)...));
	}

#  define LITEFX_IMPLEMENTATION(impl) private: \
	class impl; \
	PimplPtr<impl> m_impl;
#endif

	template <class THandle>
	class IResource {
	private:
		THandle m_handle;

	protected:
		IResource(const THandle handle) noexcept : m_handle(handle) { }

	public:
		IResource(const IResource&) = delete;
		IResource(IResource&&) = delete;
		virtual ~IResource() noexcept = default;

	protected:
		THandle& handle() noexcept { return m_handle; }

	public:
		THandle handle() const noexcept { return m_handle; }
	};

	/**
	* 
	**/
	class LITEFX_CORE_API AppVersion {
		LITEFX_IMPLEMENTATION(AppVersionImpl)

	public:
		explicit AppVersion(int major = 1, int minor = 0, int patch = 0, int revision = 0) noexcept;
		AppVersion(const AppVersion&) = delete;
		AppVersion(AppVersion&&) = delete;
		virtual ~AppVersion() noexcept;

	public:
		int getMajor() const noexcept;
		int getMinor() const noexcept;
		int getPatch() const noexcept;
		int getRevision() const noexcept;
		int getEngineMajor() const noexcept;
		int getEngineMinor() const noexcept;
		int getEngineRevision() const noexcept;
		int getEngineStatus() const noexcept;
		String getEngineIdentifier() const noexcept;
		String getEngineVersion() const noexcept;
	};

	/**
	* Base class for a LiteFX application.
	**/
	class LITEFX_CORE_API App {
		LITEFX_IMPLEMENTATION(AppImpl)

	public:
		App(const Platform& platform) noexcept;
		App(const App&) = delete;
		App(App&&) = delete;
		virtual ~App() noexcept;

	public:
		virtual String getName() const noexcept = 0;
		virtual AppVersion getVersion() const noexcept = 0;
		virtual Platform getPlatform() const noexcept;

	public:
		virtual int start(const int argc, const char** argv);
		virtual int start(const Array<String>& args) = 0;
		virtual void stop() = 0;
		virtual void work() = 0;
	};

}