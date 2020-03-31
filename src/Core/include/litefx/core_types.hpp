#pragma once

#include <litefx/core.h>

namespace LiteFX {

	/**
	* 
	**/
	class LITEFX_CORE_API AppVersion
	{
	private:
		int m_major, m_minor, m_patch, m_revision;

	public:
		explicit AppVersion(int major = 1, int minor = 0, int patch = 0, int revision = 0);
		AppVersion(const AppVersion&) = delete;
		AppVersion(AppVersion&&) = delete;
		virtual ~AppVersion() = default;

	public:
		int getMajor() const;
		int getMinor() const;
		int getPatch() const;
		int getRevision() const;
		int getEngineMajor() const;
		int getEngineMinor() const;
		int getEngineRevision() const;
		int getEngineStatus() const;
		const String& getEngineIdentifier() const;
		const String& getEngineVersion() const;
	};

	/**
	* Base class for a LiteFX application.
	**/
	class LITEFX_CORE_API App 
	{
	public:
		App();
		App(const App&) = delete;
		App(App&&) = delete;
		virtual ~App();

	public:
		virtual const String getName() const = 0;
		virtual const AppVersion getVersion() const = 0;

	public:
		virtual void start(int argc, char** argv);
		virtual void start(const Array<String>& args);
		virtual void stop();
		virtual void work() = 0;
	};

}