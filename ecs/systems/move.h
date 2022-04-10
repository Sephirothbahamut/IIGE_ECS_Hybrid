#pragma once

#include <utils/math/geometry/transformations.h>

#include <entt.h>

#include "../../Window.h"
#include "../../Scene.h"

#include "../components/bad_draw.h"
#include "../components/spatial.h"
#include "../components/collision.h"

namespace iige::ecs::systems
	{
	void move(iige::Scene& scene)
		{
		auto movement_view{scene.ecs_registry.view<components::transform, components::speed, components::transform_next, components::transform_prev>()};

		movement_view.each([](utils::math::Transform2& transform, const utils::math::Transform2& speed, utils::math::Transform2& transform_next, utils::math::Transform2& transform_prev)
			{
			transform = transform_next;
			transform_prev = transform;
			transform_next += speed;
			});

		auto moving_colliders_view{scene.ecs_registry.view<components::transform, components::collider_source, components::collider>()};

		using namespace utils::math::geometry::transformations;
		moving_colliders_view.each([](const components::transform& transform, const components::collider& collider_source, components::collider& collider)
			{
			using namespace iige::ecs::components;
			collider = collider_source & transform;
			});
		}

	void interpolate(iige::Scene& scene, float interpolation)
		{
		auto movement_view{scene.ecs_registry.view<components::transform, components::transform_next, components::transform_prev>()};

		movement_view.each([interpolation](utils::math::Transform2& transform, const utils::math::Transform2& transform_next, const utils::math::Transform2& transform_prev)
			{
			transform = { utils::math::Transform2::lerp(transform_prev, transform_next, interpolation) };
			});
		}
	}