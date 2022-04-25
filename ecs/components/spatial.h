#pragma once

#include <utils/math/geometry/transformations.h>
#include <utils/definitions.h>

#include "../../types/core.h"

#include "collision.h"
#include "component.h"

namespace iige::ecs::components
	{
	using namespace entt::literals;

	namespace details
		{
		template <typename component_t, entt::id_type index>
		struct spatial_constraints : component<component_t, index>
			{
			using component<component_t, index>::component;
			};

		template <typename entt::id_type index>
		struct transform_component : component<utils::math::transform2, index>
			{
			using component<utils::math::transform2, index>::component;
			using x_min         = details::spatial_constraints<float     , (index * 10) + 0>;
			using x_max         = details::spatial_constraints<float     , (index * 10) + 1>;
			using y_min         = details::spatial_constraints<float     , (index * 10) + 2>;
			using y_max         = details::spatial_constraints<float     , (index * 10) + 3>;
			using magnitude_min = details::spatial_constraints<float     , (index * 10) + 4>;
			using magnitude_max = details::spatial_constraints<float     , (index * 10) + 5>;
			using angle_min     = details::spatial_constraints<angle::deg, (index * 10) + 0>;
			using angle_max     = details::spatial_constraints<angle::deg, (index * 10) + 1>;
			using scale_min     = details::spatial_constraints<float     , (index * 10) + 6>;
			using scale_max     = details::spatial_constraints<float     , (index * 10) + 7>;
			};
		}

	namespace relative
		{
		using transform      = details::transform_component      <"relative_transform"_hs>;
		using transform_next = component<utils::math::transform2, "relative_transform_next"_hs>;
		using transform_prev = component<utils::math::transform2, "relative_transform_prev"_hs>;
		}
	namespace absolute
		{
		using transform      = details::transform_component      <"absolute_transform"_hs>;
		using transform_next = component<utils::math::transform2, "absolute_transform_next"_hs>;
		using transform_prev = component<utils::math::transform2, "absolute_transform_prev"_hs>;
		}
	using speed        = details::transform_component<"speed"_hs>;
	using acceleration = details::transform_component<"acceleration"_hs>;

	inline void in_world(entt::registry& registry, entt::entity entity, iige::transform transform = {})
		{
		const auto& current_transform{registry.get_or_emplace<components::absolute::transform>(entity, transform)};
		}
	inline void add_movement(entt::registry& registry, entt::entity entity, iige::transform speed = utils::math::transform2::zero(), iige::transform transform = {})
		{
		const auto& current_transform{registry.get_or_emplace<components::absolute::transform>(entity, transform)};
		utils::discard(registry.get_or_emplace<absolute::transform_next>(entity, current_transform.value()));
		utils::discard(registry.get_or_emplace<absolute::transform_prev>(entity, current_transform.value()));
		utils::discard(registry.get_or_emplace<components::speed>(entity, speed));
		}
	inline void add_acceleration(entt::registry& registry, entt::entity entity, iige::transform acceleration = iige::transform::zero(), iige::transform speed = iige::transform::zero(), iige::transform transform = {})
		{
		const auto& current_transform{registry.get_or_emplace<components::absolute::transform>(entity, transform)};
		utils::discard(registry.get_or_emplace<absolute::transform_next>(entity, current_transform.value()));
		utils::discard(registry.get_or_emplace<absolute::transform_prev>(entity, current_transform.value()));
		utils::discard(registry.get_or_emplace<components::speed       >(entity, speed));
		utils::discard(registry.get_or_emplace<components::acceleration>(entity, acceleration));
		}

	template <typename constraint_t>
	inline void add_constraints(entt::registry& registry, entt::entity entity, typename constraint_t::value_type value)
		{
		utils::discard(registry.get_or_emplace<constraint_t>(entity, value));
		}
	}

