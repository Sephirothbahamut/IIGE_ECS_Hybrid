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
		auto reset{scene.ecs_registry.view<collider_t::discrete_source, collider_t>()};
		reset.each([](const collider_t::discrete_source& collider_source, collider_t& collider) 
			{
			if constexpr (components::colliders::is_discrete_collider<collider_t>)
				{
				collider.value() = collider_source.value();
				}
			else if constexpr (components::colliders::is_continuous_collider<collider_t>)
				{
				collider.value().a = collider_source.value();
				collider.value().b = collider_source.value();
				}
			});
		
		if constexpr (components::colliders::is_discrete_collider<collider_t>)
			{
			auto scaling{scene.ecs_registry.view<components::transform::absolute::scale, collider_t>()};
			scaling.each([](const float& scale, collider_t& collider)
				{
				utils::math::geometry::transformations::scale_self(collider.value(), scale);
				});
			auto rotating{scene.ecs_registry.view<components::transform::absolute::angle, collider_t>()};
			rotating.each([](const angle::rad& angle, collider_t& collider)
				{
				utils::math::geometry::transformations::rotate_self(collider.value(), angle);
				});
			auto translating{scene.ecs_registry.view<components::transform::absolute::x, components::transform::absolute::y, collider_t>()};
			translating.each([](const float& x, const float& y, collider_t& collider)
				{
				utils::math::geometry::transformations::translate_self(collider.value(), vec2f{x, y});
				});
			}
		else if constexpr (components::colliders::is_continuous_collider<collider_t>)
			{
			auto scaling{scene.ecs_registry.view<components::transform::absolute::scale, components::transform::absolute::next::scale, collider_t>()};
			scaling.each([](const float& scale, const float& scale_next, collider_t& collider)
				{
				utils::math::geometry::transformations::scale_self(collider.value(), scale, scale_next);
				});
			auto rotating{scene.ecs_registry.view<components::transform::absolute::angle, components::transform::absolute::next::angle, collider_t>()};
			rotating.each([](const angle::rad& angle, const angle::rad& angle_next, collider_t& collider)
				{
				utils::math::geometry::transformations::rotate_self(collider.value(), angle, angle_next);
				});
			auto translating{scene.ecs_registry.view<components::transform::absolute::x, components::transform::absolute::y, components::transform::absolute::next::x, components::transform::absolute::next::y, collider_t>()};
			translating.each([](const float& x, const float& y, const float& x_next, const float& y_next, collider_t& collider)
				{
				utils::math::geometry::transformations::translate_self(collider.value(), vec2f{x, y}, vec2f{x_next, y_next});
				});
			}

		if constexpr (!std::is_same_v<collider_t, components::colliders::aabb>)
			{
			auto moved{scene.ecs_registry.view<collider_t, components::colliders::aabb, components::transform::moved>()};
			moved.each([](const collider_t& collider, components::colliders::aabb& aabb) 
				{
				aabb.value() = static_cast<shapes::aabb>(collider.value());
				});
			}
		}

	namespace details
		{
		template <typename T, typename constraint_t = T>
		inline void apply_constaints_inner(iige::Scene& scene)
			{
			auto clamp{scene.ecs_registry.view<T, constraint_t::min, constraint_t::max>()};
			clamp.each([](T::value_type& value, const constraint_t::min::value_type& min, const constraint_t::max::value_type& max)
				{
				value = utils::clamp(value, min, max);
				});

			if constexpr (std::is_same_v<T::value_type, float>) // No min or max alone with angles doesn't make sense.
				{
				auto min{scene.ecs_registry.view<T, constraint_t::min>(entt::exclude<constraint_t::max>)};
				min.each([](T::value_type& value, const constraint_t::min::value_type& min) { value = std::max(value, min); });

				auto max{scene.ecs_registry.view<T, constraint_t::max>(entt::exclude<constraint_t::min>)};
				max.each([](T::value_type& value, const constraint_t::max::value_type& max) { value = std::min(value, max); });
				}
			}
		template <typename T, typename constraint_t = T>
		inline void apply_constraints(iige::Scene& scene)
			{
			apply_constaints_inner<T::x    , constraint_t::x    >(scene);
			apply_constaints_inner<T::y    , constraint_t::y    >(scene);
			apply_constaints_inner<T::angle, constraint_t::angle>(scene);
			apply_constaints_inner<T::scale, constraint_t::scale>(scene);
			}

		template <typename T>
		inline void apply_constraints_magnitude(iige::Scene& scene)
			{
			auto clamp{scene.ecs_registry.view<T::x, T::y, T::translation_magnitude::min, T::translation_magnitude::max>()};
			clamp.each([](T::x::value_type& x, T::y::value_type& y, const T::translation_magnitude::min::value_type& min, const T::translation_magnitude::max::value_type& max)
				{
				utils::math::vec2f vec2{x, y};
				float magnitude = vec2.magnitude();
				magnitude = utils::clamp(magnitude, min, max);
				vec2 = magnitude;
				x = vec2.x;
				y = vec2.y;
				});
			
			auto min{scene.ecs_registry.view<T::x, T::y, T::translation_magnitude::min>(entt::exclude<T::translation_magnitude::max>)};
			min.each([](T::x::value_type& x, T::y::value_type& y, const T::translation_magnitude::min::value_type& min)
				{
				utils::math::vec2f vec2{x, y};
				float magnitude = vec2.magnitude();
				magnitude = std::max(magnitude, min);
				vec2 = magnitude;
				x = vec2.x;
				y = vec2.y;
				});
			
			auto max{scene.ecs_registry.view<T::x, T::y, T::translation_magnitude::max>(entt::exclude<T::translation_magnitude::min>)};
			max.each([](T::x::value_type& x, T::y::value_type& y, const T::translation_magnitude::max::value_type& max)
				{
				utils::math::vec2f vec2{x, y};
				float magnitude = vec2.magnitude();
				magnitude = std::min(magnitude, max);
				vec2 = magnitude;
				x = vec2.x;
				y = vec2.y;
				});
			}

		template <typename self_t, typename other_t>
		inline void add_to_self_inner(iige::Scene& scene, float delta_time)
			{
			auto view{scene.ecs_registry.view<self_t, other_t>()};
			view.each([delta_time](self_t::value_type& self, const other_t::value_type& other) { self += other * delta_time; });
			}
		template <typename a_t, typename b_t, typename out_t>
		inline void add_into_inner(iige::Scene& scene, float delta_time)
			{
			auto view{scene.ecs_registry.view<a_t, b_t, out_t>()};
			view.each([delta_time](const a_t::value_type& a, const b_t::value_type& b, out_t::value_type& out) { out = a + b * delta_time; });
			}
		template <typename a_t, typename b_t, typename out_t, typename exclude>
		inline void add_into_exclude_inner(iige::Scene& scene, float delta_time)
			{
			auto view{scene.ecs_registry.view<a_t, b_t, out_t>(entt::exclude<exclude>)};
			view.each([delta_time](const a_t::value_type& a, const b_t::value_type& b, out_t::value_type& out) { out = a + b * delta_time; });
			}
		template <typename from_t, typename to_t>
		inline void assign_to_inner(iige::Scene& scene)
			{
			auto view{scene.ecs_registry.view<from_t, to_t>()};
			view.each([](const from_t::value_type& from, to_t::value_type& to) { to = from; });
			}

		template <typename self_t, typename other_t>
		inline void add_to_self(iige::Scene& scene, float delta_time)
			{
			add_to_self_inner<self_t::x    , other_t::x    >(scene, delta_time);
			add_to_self_inner<self_t::y    , other_t::y    >(scene, delta_time);
			add_to_self_inner<self_t::angle, other_t::angle>(scene, delta_time);
			add_to_self_inner<self_t::scale, other_t::scale>(scene, delta_time);
			}
		template <typename a_t, typename b_t, typename out_t>
		inline void add_into(iige::Scene& scene, float delta_time)
			{
			add_into_inner<a_t::x    , b_t::x    , out_t::x    >(scene, delta_time);
			add_into_inner<a_t::y    , b_t::y    , out_t::y    >(scene, delta_time);
			add_into_inner<a_t::angle, b_t::angle, out_t::angle>(scene, delta_time);
			add_into_inner<a_t::scale, b_t::scale, out_t::scale>(scene, delta_time);
			}
		template <typename a_t, typename b_t, typename out_t, typename exclude>
		inline void add_into_exclude(iige::Scene& scene, float delta_time)
			{
			add_into_exclude_inner<a_t::x    , b_t::x    , out_t::x    , exclude::x    >(scene, delta_time);
			add_into_exclude_inner<a_t::y    , b_t::y    , out_t::y    , exclude::y    >(scene, delta_time);
			add_into_exclude_inner<a_t::angle, b_t::angle, out_t::angle, exclude::angle>(scene, delta_time);
			add_into_exclude_inner<a_t::scale, b_t::scale, out_t::scale, exclude::scale>(scene, delta_time);
			}
		template <typename from_t, typename to_t>
		inline void assign_to(iige::Scene& scene)
			{
			assign_to_inner<from_t::x    , to_t::x    >(scene);
			assign_to_inner<from_t::y    , to_t::y    >(scene);
			assign_to_inner<from_t::angle, to_t::angle>(scene);
			assign_to_inner<from_t::scale, to_t::scale>(scene);
			}

		template <typename from_t, typename to_t, typename out_t>
		inline void interpolate_inner(iige::Scene& scene, float interpolation)
			{
			auto view{scene.ecs_registry.view<from_t, to_t, out_t>()};
			view.each([interpolation](const from_t::value_type& from, const to_t::value_type& to, out_t::value_type& out)
				{
				out = utils::lerp(from, to, interpolation);
				});
			}

		template <typename T>
		inline void mark_moved_inner(iige::Scene& scene)
			{
			auto moved{scene.ecs_registry.view<T>()};
			moved.each([&scene](entt::entity entity, T) { scene.ecs_registry.get_or_emplace<components::transform::moved>(entity); });
			}
		inline void mark_moved(iige::Scene& scene)
			{
			mark_moved_inner<components::transform::speed::x    >(scene);
			mark_moved_inner<components::transform::speed::y    >(scene);
			mark_moved_inner<components::transform::speed::angle>(scene);
			mark_moved_inner<components::transform::speed::scale>(scene);
			}
		}


	inline void move(iige::Scene& scene, float delta_time)
		{
		scene.ecs_registry.clear<components::transform::moved>();
		details::mark_moved(scene);
		details::mark_moved_inner<components::transform::absolute::x>(scene);
		
		// Speed and acceleration
		details::apply_constraints          <components::transform::acceleration                                                                                        >(scene);
		details::apply_constraints_magnitude<components::transform::acceleration                                                                                        >(scene);
		details::add_to_self                <components::transform::speed, components::transform::acceleration                                                          >(scene, delta_time);
		details::apply_constraints          <components::transform::speed                                                                                               >(scene);
		details::apply_constraints_magnitude<components::transform::speed                                                                                               >(scene);
		
		// Relative movement
		details::assign_to        <components::transform::relative::next, components::transform::relative                                                               >(scene);
		
		details::add_into         <components::transform::relative, components::transform::speed, components::transform::relative::next                                 >(scene, delta_time);
		details::apply_constraints<components::transform::relative::next, components::transform::relative                                                               >(scene);
		
		// Absolute only movement
		details::assign_to        <components::transform::absolute::next, components::transform::absolute                                                               >(scene);
		// Exclude relative > evaluates movement for entities without a parent
		details::add_into_exclude <components::transform::absolute, components::transform::speed, components::transform::absolute::next, components::transform::relative>(scene, delta_time);

		// Absolute based on relative
		// TODO > evaluate movement for entities with a parent
		
		details::apply_constraints<components::transform::absolute::next, components::transform::absolute                                                               >(scene);
		move_colliders<components::colliders::point                >(scene);
		move_colliders<components::colliders::segment              >(scene);
		move_colliders<components::colliders::aabb                 >(scene);
		move_colliders<components::colliders::circle               >(scene);
		move_colliders<components::colliders::polygon              >(scene);
		move_colliders<components::colliders::convex_polygon       >(scene);
		move_colliders<components::colliders::hollow_aabb          >(scene);
		move_colliders<components::colliders::hollow_circle        >(scene);
		move_colliders<components::colliders::hollow_polygon       >(scene);
		move_colliders<components::colliders::hollow_convex_polygon>(scene);
		move_colliders<components::colliders::continuous_point     >(scene);
		}

	inline void interpolate(iige::Scene& scene, float interpolation)
		{
		details::interpolate_inner<components::transform::absolute::x    , components::transform::absolute::next::x    , components::transform::interpolated::x    >(scene, interpolation);
		details::interpolate_inner<components::transform::absolute::y    , components::transform::absolute::next::y    , components::transform::interpolated::y    >(scene, interpolation);
		details::interpolate_inner<components::transform::absolute::angle, components::transform::absolute::next::angle, components::transform::interpolated::angle>(scene, interpolation);
		details::interpolate_inner<components::transform::absolute::scale, components::transform::absolute::next::scale, components::transform::interpolated::scale>(scene, interpolation);
		}
	}