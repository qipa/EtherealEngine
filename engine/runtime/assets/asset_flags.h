#pragma once
#include <memory>
#include <string>

#include "core/subsystem/tasks.hpp"
#include "core/signals/event.hpp"
#include "asset_handle.h"

namespace runtime
{

enum class load_mode
{
	sync,
	async
};

enum class load_flags
{
	default,
	reload,
	do_not_unload
};

}
