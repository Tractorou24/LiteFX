#pragma once

#include <litefx/version.h>
#include <litefx/config.h>
#include <litefx/platforms.h>

#ifdef LITEFX_OS_WINDOWS
#  define NOMINMAX
#  include <Windows.h>
#endif

#include <litefx/containers.hpp>
#include <litefx/traits.hpp>