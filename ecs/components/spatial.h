#pragma once

#include <utils/math/geometry/transformations.h>
#include <utils/definitions.h>
#include <utils/string.h>

#include "../../types/core.h"

#include "collision.h"
#include "component.h"

namespace iige::ecs::components::transform
	{
	namespace details
		{
		template <utils::compile_time::template_string name>
		struct base : component<iige::transform, utils::compile_time::concat<name, "_transform">()>
			{
			using component<iige::transform, utils::compile_time::concat<name, "_transform">()>::component;

			using x             = component<float, utils::compile_time::concat<name, "_x"    >()>;
			using y             = component<float, utils::compile_time::concat<name, "_y"    >()>;
			using angle         = component<float, utils::compile_time::concat<name, "_angle">()>;
			using scale         = component<float, utils::compile_time::concat<name, "_scale">()>;

			using x_min         = component<float           , utils::compile_time::concat<name, "_x_min"        >()>;
			using x_max         = component<float           , utils::compile_time::concat<name, "_x_max"        >()>;
			using y_min         = component<float           , utils::compile_time::concat<name, "_y_min"        >()>;
			using y_max         = component<float           , utils::compile_time::concat<name, "_y_max"        >()>;
			using magnitude_min = component<float           , utils::compile_time::concat<name, "_magnitude_min">()>;
			using magnitude_max = component<float           , utils::compile_time::concat<name, "_magnitude_max">()>;
			using angle_min     = component<iige::angle::deg, utils::compile_time::concat<name, "_angle_min"    >()>;
			using angle_max     = component<iige::angle::deg, utils::compile_time::concat<name, "_angle_max"    >()>;
			using scale_min     = component<float           , utils::compile_time::concat<name, "_scale_min"    >()>;
			using scale_max     = component<float           , utils::compile_time::concat<name, "_scale_max"    >()>;
			};
		}

	using relative     = details::base<"relative">;
	using absolute     = details::base<"absolute">;
	using speed        = details::base<"speed">;
	using acceleration = details::base<"acceleration">;
	}

