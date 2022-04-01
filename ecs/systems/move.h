#pragma once

#include <entt.h>

#include "../../Window.h"
#include "../../Scene.h"

#include "../components/bad_draw.h"
#include "../components/transform.h"
#include "../components/speed.h"

namespace iige::ecs::systems
	{
	void move(iige::Scene& scene)
		{
		auto movement_view{scene.ecs_registry.view<components::transform, components::speed, components::transform_next>()};

		movement_view.each([](components::transform& transform, const components::speed& speed, components::transform_next& transform_next)
			{
			transform.t = transform_next.t;
			transform_next.t += speed.t;
			/*auto tmp{transform_next};
			transform_next.t = transform.t + speed.t;
			transform.t = tmp.t;*/
			});
		}
	//template <typename Scene_t>
	void interpolate(iige::Scene& scene, float interpolation)
		{
		auto movement_view{scene.ecs_registry.view<components::transform, components::transform_next, components::interpolated>()};

		movement_view.each([interpolation](const components::transform& transform, const components::transform_next& transform_next, components::interpolated& interpolated)
			{
			interpolated.t = { utils::math::Transform2::lerp(transform.t, transform_next.t, interpolation) };
			});
		}
	}