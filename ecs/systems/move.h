#pragma once

#include <execution>

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
		if constexpr (components::colliders::is_discrete_collider<collider_t>)
			{
			auto moving_colliders_view{scene.ecs_registry.view<components::transform, collider_t::discrete_source, collider_t, components::colliders::aabb>()};

			using namespace iige::shapes::transformations;
			moving_colliders_view.each([](const iige::transform& transform, const collider_t::discrete_source& collider_source, collider_t& collider, components::colliders::aabb& aabb)
				{
				using namespace iige::ecs::components;//TODO check if necessary
				collider.value() = collider_source.value() * transform;
				aabb.value() = static_cast<shapes::aabb>(collider.value());
				});
			}
		else if constexpr (components::colliders::is_continuous_collider<collider_t>)
			{
			auto moving_colliders_view{scene.ecs_registry.view<components::transform_prev, components::transform_next, typename collider_t::discrete_source, collider_t, components::colliders::aabb>()};

			using namespace iige::shapes::transformations;
			moving_colliders_view.each([]
					(
					const iige::transform& transform_prev, const iige::transform& transform_next,
					const typename collider_t::discrete_source& collider_source, collider_t& collider, components::colliders::aabb& aabb
					)
				{
				using namespace iige::ecs::components;//TODO check if necessary
				collider.value() = collider_t{collider_source.value() * transform_prev, collider_source.value() * transform_next};
				aabb.value() = static_cast<shapes::aabb>(collider.value());
				});
			}
		}

	template <>
	void move_colliders<components::colliders::aabb>(iige::Scene& scene)
		{
		auto moving_colliders_view{scene.ecs_registry.view<components::transform, components::colliders::aabb::discrete_source, components::colliders::aabb>()};

		using namespace iige::shapes::transformations;
		moving_colliders_view.each([](const iige::transform& transform, const components::colliders::aabb::discrete_source& collider_source, components::colliders::aabb& collider)
			{
			using namespace iige::ecs::components;
			collider.value() = collider_source.value() * transform;
			});
		}


	namespace details
		{
		template <typename transform_type, typename constraints_type = transform_type>
		void apply_constraints(iige::Scene& scene)
			{
			auto     x_min{scene.ecs_registry.view<transform_type, typename constraints_type::    x_min>()}; 
			auto     x_max{scene.ecs_registry.view<transform_type, typename constraints_type::    x_max>()}; 
			auto     y_min{scene.ecs_registry.view<transform_type, typename constraints_type::    y_min>()}; 
			auto     y_max{scene.ecs_registry.view<transform_type, typename constraints_type::    y_max>()}; 
			auto angle_min{scene.ecs_registry.view<transform_type, typename constraints_type::angle_min>()}; 
			auto angle_max{scene.ecs_registry.view<transform_type, typename constraints_type::angle_max>()}; 
			auto scale_min{scene.ecs_registry.view<transform_type, typename constraints_type::scale_min>()}; 
			auto scale_max{scene.ecs_registry.view<transform_type, typename constraints_type::scale_max>()}; 

			    x_min.each([](iige::transform& transform, const float&            constraint) { transform.position.x        = std::max(transform.position.x, constraint); });
			    x_max.each([](iige::transform& transform, const float&            constraint) { transform.position.x        = std::min(transform.position.x, constraint); });
			    y_min.each([](iige::transform& transform, const float&            constraint) { transform.position.y        = std::max(transform.position.y, constraint); });
			    y_max.each([](iige::transform& transform, const float&            constraint) { transform.position.y        = std::min(transform.position.y, constraint); });
			angle_min.each([](iige::transform& transform, const iige::angle::deg& constraint) { transform.orientation.value = std::max(transform.orientation.value, constraint.value); });
			angle_max.each([](iige::transform& transform, const iige::angle::deg& constraint) { transform.orientation.value = std::min(transform.orientation.value, constraint.value); });
			scale_min.each([](iige::transform& transform, const float&            constraint) { transform.size              = std::max(transform.size, constraint); });
			scale_max.each([](iige::transform& transform, const float&            constraint) { transform.size              = std::min(transform.size, constraint); });
			}
		}


	inline void move(iige::Scene& scene, float delta_time)
		{


		details::apply_constraints<components::acceleration>(scene);
		auto accelerate_view{scene.ecs_registry.view<components::speed, components::acceleration>()};
		accelerate_view.each([&](iige::transform& speed, const iige::transform& acceleration) { speed += acceleration * delta_time; });

		details::apply_constraints<components::speed>(scene);

		auto movement_view{scene.ecs_registry.view<components::transform, components::speed, components::transform_next, components::transform_prev>()};


		/*std::for_each(std::execution::par, movement_view.begin(), movement_view.end(), [&](const auto entity)
			{
				  utils::math::transform2& transform     {movement_view.get<components::transform     >(entity)};
			const utils::math::transform2& speed         {movement_view.get<components::speed         >(entity)};
				  utils::math::transform2& transform_next{movement_view.get<components::transform_next>(entity)};
				  utils::math::transform2& transform_prev{movement_view.get<components::transform_prev>(entity)};
			/**/
		movement_view.each([&](utils::math::transform2& transform, const utils::math::transform2& speed, utils::math::transform2& transform_next, utils::math::transform2& transform_prev)
			{
			transform      = transform_next;
			transform_prev = transform;
			transform_next += speed * delta_time;
			});

		details::apply_constraints<components::transform_next, components::transform>(scene);

		move_colliders<components::colliders::point           >(scene);
		move_colliders<components::colliders::segment         >(scene);
		move_colliders<components::colliders::aabb            >(scene);
		move_colliders<components::colliders::circle          >(scene);
		move_colliders<components::colliders::polygon         >(scene);
		move_colliders<components::colliders::convex_polygon  >(scene);
		move_colliders<components::colliders::continuous_point>(scene);
		}

	inline void interpolate(iige::Scene& scene, float interpolation)
		{
		auto movement_view{scene.ecs_registry.view<components::transform, components::transform_next, components::transform_prev>()};

		/*std::for_each(std::execution::par, movement_view.begin(), movement_view.end(), [&movement_view, &interpolation](const auto entity)
			{
			      utils::math::transform2& transform     {movement_view.get<components::transform     >(entity)};
			const utils::math::transform2& transform_next{movement_view.get<components::transform_next>(entity)};
			const utils::math::transform2& transform_prev{movement_view.get<components::transform_prev>(entity)};
			/**/
		movement_view.each([interpolation](utils::math::transform2& transform, const utils::math::transform2& transform_next, const utils::math::transform2& transform_prev)
			{
			transform = {utils::math::transform2::lerp(transform_prev, transform_next, interpolation)};
			});
		}
	}