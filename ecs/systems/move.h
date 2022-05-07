#pragma once

#include <string>
#include <execution>
#include <stdexcept>

#include <utils/math/geometry/transformations.h>
#include <utils/compilation/debug.h>

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
			auto scaling{scene.ecs_registry.view<components::transform::absolute::scaling, collider_t>()};
			scaling.each([](const float& scaling, collider_t& collider)
				{
				utils::math::geometry::transformations::scale_self(collider.value(), scaling);
				});
			auto rotating{scene.ecs_registry.view<components::transform::absolute::rotation, collider_t>()};
			rotating.each([](const angle::rad& rotation, collider_t& collider)
				{
				utils::math::geometry::transformations::rotate_self(collider.value(), rotation);
				});
			auto translating{scene.ecs_registry.view<components::transform::absolute::translation, collider_t>()};
			translating.each([](const vec2f& xy, collider_t& collider)
				{
				utils::math::geometry::transformations::translate_self(collider.value(), xy);
				});
			}
		else if constexpr (components::colliders::is_continuous_collider<collider_t>)
			{
			auto scaling{scene.ecs_registry.view<components::transform::absolute::scaling, components::transform::absolute::next::scaling, collider_t>()};
			scaling.each([](const float& scaling, const float& scaling_next, collider_t& collider)
				{
				utils::math::geometry::transformations::scale_self(collider.value(), scaling, scaling_next);
				});
			auto rotating{scene.ecs_registry.view<components::transform::absolute::rotation, components::transform::absolute::next::rotation, collider_t>()};
			rotating.each([](const angle::rad& rotation, const angle::rad& rotation_next, collider_t& collider)
				{
				utils::math::geometry::transformations::rotate_self(collider.value(), rotation, rotation_next);
				});
			auto translating{scene.ecs_registry.view<components::transform::absolute::translation, components::transform::absolute::next::translation, collider_t>()};
			translating.each([](const vec2f& xy, const vec2f& xy_next, collider_t& collider)
				{
				utils::math::geometry::transformations::translate_self(collider.value(), xy, xy_next);
				});
			}

		
		auto moved{scene.ecs_registry.view<collider_t, components::colliders::details::bounding_aabb, components::transform::moved>()};
		moved.each([](const collider_t& collider, components::colliders::details::bounding_aabb& aabb) 
			{
			aabb.value() = static_cast<shapes::aabb>(collider.value());
			});
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
			apply_constaints_inner<T::translation, constraint_t::translation>(scene);
			apply_constaints_inner<T::rotation   , constraint_t::rotation   >(scene);
			apply_constaints_inner<T::scaling    , constraint_t::scaling    >(scene);
			}

		template <typename T>
		inline void apply_constraints_magnitude(iige::Scene& scene)
			{
			auto clamp{scene.ecs_registry.view<T::translation, T::translation_magnitude::min, T::translation_magnitude::max>()};
			clamp.each([](T::translation::value_type& xy, const T::translation_magnitude::min::value_type& min, const T::translation_magnitude::max::value_type& max)
				{
				float magnitude = xy.magnitude();
				magnitude = utils::clamp(magnitude, min, max);
				xy = magnitude;
				});
			
			auto min{scene.ecs_registry.view<T::translation, T::translation_magnitude::min>(entt::exclude<T::translation_magnitude::max>)};
			min.each([](T::translation::value_type& xy, const T::translation_magnitude::min::value_type& min)
				{
				float magnitude = xy.magnitude();
				magnitude = std::max(magnitude, min);
				xy = magnitude;
				});
			
			auto max{scene.ecs_registry.view<T::translation, T::translation_magnitude::max>(entt::exclude<T::translation_magnitude::min>)};
			max.each([](T::translation::value_type& xy, const T::translation_magnitude::max::value_type& max)
				{
				float magnitude = xy.magnitude();
				magnitude = std::min(magnitude, max);
				xy = magnitude;
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
			add_to_self_inner<self_t::translation, other_t::translation>(scene, delta_time);
			add_to_self_inner<self_t::rotation   , other_t::rotation   >(scene, delta_time);
			add_to_self_inner<self_t::scaling    , other_t::scaling    >(scene, delta_time);
			}
		template <typename a_t, typename b_t, typename out_t>
		inline void add_into(iige::Scene& scene, float delta_time)
			{
			add_into_inner<a_t::translation, b_t::translation, out_t::translation>(scene, delta_time);
			add_into_inner<a_t::rotation   , b_t::rotation   , out_t::rotation   >(scene, delta_time);
			add_into_inner<a_t::scaling    , b_t::scaling    , out_t::scaling    >(scene, delta_time);
			}
		template <typename a_t, typename b_t, typename out_t, typename exclude>
		inline void add_into_exclude(iige::Scene& scene, float delta_time)
			{
			add_into_exclude_inner<a_t::translation, b_t::translation, out_t::translation, exclude::translation>(scene, delta_time);
			add_into_exclude_inner<a_t::rotation   , b_t::rotation   , out_t::rotation   , exclude::rotation   >(scene, delta_time);
			add_into_exclude_inner<a_t::scaling    , b_t::scaling    , out_t::scaling    , exclude::scaling    >(scene, delta_time);
			}
		template <typename from_t, typename to_t>
		inline void assign_to(iige::Scene& scene)
			{
			assign_to_inner<from_t::translation, to_t::translation>(scene);
			assign_to_inner<from_t::rotation   , to_t::rotation   >(scene);
			assign_to_inner<from_t::scaling    , to_t::scaling    >(scene);
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
			mark_moved_inner<components::transform::absolute::next::translation>(scene);
			mark_moved_inner<components::transform::absolute::next::rotation   >(scene);
			mark_moved_inner<components::transform::absolute::next::scaling    >(scene);
			}


		template <components::transform::is_transform_component T>
		inline utils::observer_ptr<typename T::value_type> get_value_next_if_present_otherwise_current(Scene& scene, entt::entity entity)
			{
			if (auto ptr{scene.ecs_registry.try_get<T>(entity)})
				{
				return &(ptr->value());
				}

			if constexpr (components::transform::is_next<T>)
				{
				if (auto ptr{scene.ecs_registry.try_get<typename T::current_t>(entity)})
					{
					return &(ptr->value());
					}
				}

			return nullptr;
			}

		template <typename relative_t, typename absolute_t, typename base_t>
		inline void add_parent_inner(iige::Scene& scene)
			{
			auto view{scene.ecs_registry.view<relative_t, absolute_t>()};
			view.each([&](entt::entity entity, const relative_t::value_type& relative, absolute_t::value_type& absolute)
				{
				if constexpr (utils::compilation::debug)
					{
					//TODO make sure this is guaranteed by the scene
					if (!scene.ecs_registry.all_of<components::child>(entity))
						{
						throw std::runtime_error{"Malformed hierarchy structure: entity \"" + std::to_string(static_cast<uint32_t>(entity)) + "\" has relative transform components, but lacks child component."};
						}
					}
				auto parent_id{scene.ecs_registry.get<components::child>(entity).parent};

				auto updated_relative{relative};

				if constexpr (components::transform::is_translation<relative_t>)
					{
					auto parent_scaling_ptr{get_value_next_if_present_otherwise_current<typename base_t::scaling>(scene, parent_id)};
					if (parent_scaling_ptr) { updated_relative *= *parent_scaling_ptr; }

					auto parent_rotation_ptr{get_value_next_if_present_otherwise_current<typename base_t::rotation>(scene, parent_id)};
					if (parent_rotation_ptr) { updated_relative += *parent_rotation_ptr; }
					}

				auto parent_value_ptr{get_value_next_if_present_otherwise_current<absolute_t>(scene, parent_id)};
				if (parent_value_ptr)
					{
					absolute = updated_relative + *parent_value_ptr;
					}
				else
					{
					absolute = updated_relative;
					}
				});
			}
		inline void add_parent(iige::Scene& scene)
			{
			// TODO sort only if moved ? and if parent moved ???
			scene.ecs_registry.sort<components::child>([&](const entt::entity lhs, const entt::entity rhs) {
				const auto& clhs = scene.ecs_registry.get<components::child>(lhs);
				const auto& crhs = scene.ecs_registry.get<components::child>(rhs);
				return crhs.parent == lhs || clhs.next_sibling == rhs
					|| (!(clhs.parent == rhs || crhs.next_sibling == lhs) && (clhs.parent < crhs.parent || (clhs.parent == crhs.parent && &clhs < &crhs)));
				});

			add_parent_inner<components::transform::relative      ::translation, components::transform::absolute      ::translation, components::transform::absolute      >(scene);
			add_parent_inner<components::transform::relative      ::rotation   , components::transform::absolute      ::rotation   , components::transform::absolute      >(scene);
			add_parent_inner<components::transform::relative      ::scaling    , components::transform::absolute      ::scaling    , components::transform::absolute      >(scene);
			add_parent_inner<components::transform::relative::next::translation, components::transform::absolute::next::translation, components::transform::absolute::next>(scene);
			add_parent_inner<components::transform::relative::next::rotation   , components::transform::absolute::next::rotation   , components::transform::absolute::next>(scene);
			add_parent_inner<components::transform::relative::next::scaling    , components::transform::absolute::next::scaling    , components::transform::absolute::next>(scene);
			}

		template <typename if_not_t, typename remove_t>
		inline void clear_teleports_inner_inner(iige::Scene& scene)
			{
			auto view{scene.ecs_registry.view<remove_t>(entt::exclude<if_not_t>)};
			view.each([&](entt::entity entity, const remove_t&) { scene.ecs_registry.remove<remove_t>(entity); });
			}
		template <typename T>
		inline void clear_teleports_inner(iige::Scene& scene)
			{
			clear_teleports_inner_inner<components::transform::speed::translation, T::next::translation>(scene);
			clear_teleports_inner_inner<components::transform::speed::rotation   , T::next::rotation   >(scene);
			clear_teleports_inner_inner<components::transform::speed::scaling    , T::next::scaling    >(scene);
			}
		template <typename T>
		inline void clear_teleports(iige::Scene& scene)
			{
			clear_teleports_inner<components::transform::absolute>(scene);
			clear_teleports_inner<components::transform::relative>(scene);
			}
		}


	inline void move(iige::Scene& scene, float delta_time)
		{
		scene.ecs_registry.clear<components::transform::moved>();
		details::mark_moved(scene);
		//details::mark_moved_inner<components::transform::absolute::translation>(scene);
		
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
		details::add_parent                                                                                                                                              (scene);

		details::clear_teleports<components::transform::absolute                                                                                                        >(scene);
		details::clear_teleports<components::transform::relative                                                                                                        >(scene);

		
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
		details::interpolate_inner<components::transform::absolute::translation, components::transform::absolute::next::translation, components::transform::interpolated::translation>(scene, interpolation);
		details::interpolate_inner<components::transform::absolute::rotation   , components::transform::absolute::next::rotation   , components::transform::interpolated::rotation   >(scene, interpolation);
		details::interpolate_inner<components::transform::absolute::scaling    , components::transform::absolute::next::scaling    , components::transform::interpolated::scaling    >(scene, interpolation);
		}
	}