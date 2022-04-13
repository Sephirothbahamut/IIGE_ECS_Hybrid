#pragma once

#include <utils/math/geometry/transformations.h>

#include "../entt.h"

#include "../../Window.h"
#include "../../Scene.h"

#include "../components/bad_draw.h"
#include "../components/spatial.h"
#include "../components/collision.h"

namespace iige::ecs::systems
	{

	template <components::colliders::is_collider collider_t>
	void move_colliders(iige::Scene& scene)
		{
		auto moving_colliders_view{scene.ecs_registry.view<components::transform, components::colliders::source<collider_t>, collider_t, components::colliders::aabb>()};

		using namespace utils::math::geometry::transformations;
		moving_colliders_view.each([](const components::transform& transform, const components::colliders::source<collider_t>& collider_source, collider_t& collider, components::colliders::aabb& aabb)
			{
			using namespace iige::ecs::components;
			collider.data = collider_source.data * transform;
			aabb.data = static_cast<components::utmg::aabb>(collider.data);
			});
		}

	template <>
	void move_colliders<components::colliders::aabb>(iige::Scene& scene)
		{
		auto moving_colliders_view{scene.ecs_registry.view<components::transform, components::colliders::source<components::colliders::aabb>, components::colliders::aabb>()};

		using namespace utils::math::geometry::transformations;
		moving_colliders_view.each([](const components::transform& transform, const components::colliders::source<components::colliders::aabb>& collider_source, components::colliders::aabb& collider)
			{
			using namespace iige::ecs::components;
			collider.data = collider_source.data * transform;
			});
		}


	inline void move(iige::Scene& scene)
		{
		auto movement_view{scene.ecs_registry.view<components::transform, components::speed, components::transform_next, components::transform_prev>()};

		movement_view.each([](utils::math::Transform2& transform, const utils::math::Transform2& speed, utils::math::Transform2& transform_next, utils::math::Transform2& transform_prev)
			{
			transform      = transform_next;
			transform_prev = transform;
			transform_next += speed;
			});


		move_colliders<components::colliders::segment>(scene);
		move_colliders<components::colliders::aabb   >(scene);
		move_colliders<components::colliders::circle >(scene);
		move_colliders<components::colliders::polygon>(scene);
		}

	inline void interpolate(iige::Scene& scene, float interpolation)
		{
		auto movement_view{scene.ecs_registry.view<components::transform, components::transform_next, components::transform_prev>()};

		movement_view.each([interpolation](utils::math::Transform2& transform, const utils::math::Transform2& transform_next, const utils::math::Transform2& transform_prev)
			{
			transform = { utils::math::Transform2::lerp(transform_prev, transform_next, interpolation) };
			});
		}
	}