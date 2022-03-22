#pragma once

#include <entt.h>

#include "../../Window.h"
#include "../../Scene.h"

#include "../../sfml_interfacing/vec.h"

#include "../components/bad_draw.h"
#include "../components/transform.h"
#include "../components/speed.h"

namespace iige::ecs::systems
	{
	void move(const Scene& scene)
		{
		auto movement_view{scene.ecs_registry.view<components::transform, components::speed, components::transform_next>()};

		movement_view.each([](const auto& transform, const auto& speed, auto& transform_next)
			{
			/*auto tmp{transform_next};
			transform_next = transform + speed;
			transform = tmp;*/
			});
		}
	void interpolate(const Scene& scene, float interpolation)
		{
		auto movement_view{scene.ecs_registry.view<components::transform, components::transform_next, components::interpolated>()};

		/*movement_view.each([interpolation](const components::transform& transform, const components::transform_next& transform_next, components::interpolated& interpolated)
			{
			interpolated = {utils::math::Transform2::lerp(transform.t, transform_next.t, interpolation)};
			});*/
		}
	}