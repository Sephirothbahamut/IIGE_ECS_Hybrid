#pragma once

#include <utils/math/Transform2.h>
#include <utils/math/geometry/transformations.h>
#include <utils/definitions.h>
#include "collision.h"

namespace iige::ecs::components
	{
	using transform = utils::math::Transform2;

	struct transform_next : transform
		{
		using transform::Transform2;
		transform_next           (const transform& copy) noexcept : transform{copy} {}
		transform_next& operator=(const transform& copy) noexcept { transform::operator= (copy); return *this; }
		};
	struct transform_prev : transform
		{
		using transform::Transform2;
		transform_prev           (const transform& copy) noexcept : transform{copy} {}
		transform_prev& operator=(const transform& copy) noexcept { transform::operator= (copy); return *this; }
		};
	struct speed          : transform
		{
		using transform::Transform2;
		speed                    (const transform& copy) noexcept : transform{copy} {}
		speed         & operator=(const transform& copy) noexcept { transform::operator= (copy); return *this; }
		};

	void add_movement(entt::registry& registry, entt::entity entity, components::transform transform = {}, components::transform speed = {});
	}
