#pragma once

#include <utils/math/geometry/transformations.h>
#include <utils/definitions.h>

#include "../../types/core.h"

#include "collision.h"
#include "component.h"

namespace iige::ecs::components
	{
	/*using transform = utils::math::transform2;

	using transform_next = component<transform, 1>;
	using transform_prev = component<transform, 2>;
	using speed          = component<transform, 3>;
	using acceleration   = component<transform, 4>;*/

	namespace details
		{
		template <typename component_t, size_t index>
		struct spatial_constraints : component<component_t, index>
			{
			using component<component_t, index>::component;
			};

		template <typename size_t index>
		struct transform_component : component<utils::math::transform2>
			{
			using component<utils::math::transform2>::component;
			using x_min         = details::spatial_constraints<float     , (index * 100) + 0>;
			using x_max         = details::spatial_constraints<float     , (index * 100) + 1>;
			using y_min         = details::spatial_constraints<float     , (index * 100) + 2>;
			using y_max         = details::spatial_constraints<float     , (index * 100) + 3>;
			using magnitude_min = details::spatial_constraints<float     , (index * 100) + 4>;
			using magnitude_max = details::spatial_constraints<float     , (index * 100) + 5>;
			using angle_min     = details::spatial_constraints<angle::deg, (index * 100) + 0>;
			using angle_max     = details::spatial_constraints<angle::deg, (index * 100) + 1>;
			using scale_min     = details::spatial_constraints<float     , (index * 100) + 6>;
			using scale_max     = details::spatial_constraints<float     , (index * 100) + 7>;
			};
		}

	using transform      = details::transform_component<0>;
	using transform_next = details::transform_component<1>;
	using transform_prev = details::transform_component<2>;
	using speed          = details::transform_component<3>;
	using acceleration   = details::transform_component<4>;

	inline void in_world(entt::registry& registry, entt::entity entity, iige::transform transform = {})
		{
		const auto& current_transform{registry.get_or_emplace<components::transform>(entity, transform)};
		}
	inline void add_movement(entt::registry& registry, entt::entity entity, iige::transform speed = utils::math::transform2::zero(), iige::transform transform = {})
		{
		const auto& current_transform{registry.get_or_emplace<components::transform>(entity, transform)};
		utils::discard(registry.get_or_emplace<transform_next>(entity, current_transform.value()));
		utils::discard(registry.get_or_emplace<transform_prev>(entity, current_transform.value()));
		utils::discard(registry.get_or_emplace<components::speed>(entity, speed));
		}
	inline void add_acceleration(entt::registry& registry, entt::entity entity, iige::transform acceleration = iige::transform::zero(), iige::transform speed = iige::transform::zero(), iige::transform transform = {})
		{
		const auto& current_transform{registry.get_or_emplace<components::transform>(entity, transform)};
		utils::discard(registry.get_or_emplace<transform_next>(entity, current_transform.value()));
		utils::discard(registry.get_or_emplace<transform_prev>(entity, current_transform.value()));
		utils::discard(registry.get_or_emplace<components::speed       >(entity, speed));
		utils::discard(registry.get_or_emplace<components::acceleration>(entity, acceleration));
		}

	template <typename constraint_t>
	inline void add_constraints(entt::registry& registry, entt::entity entity, typename constraint_t::value_type value)
		{
		utils::discard(registry.get_or_emplace<constraint_t>(entity, value));
		}
	}

