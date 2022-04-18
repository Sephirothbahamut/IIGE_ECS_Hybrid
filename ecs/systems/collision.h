#pragma once

#include <utility>
#include <mutex>
#include <execution>

#include <utils/math/geometry/interactions.h>
#include <utils/math/geometry/continuous_interactions.h>
#include <utils/definitions.h>
#include <utils/variant.h>
#include <utils/math/angle.h>

#include "../entt.h"

#include "../components/collision.h"
#include "../components/spatial.h"
#include "../../types/core.h"

#include "../../Window.h"
#include "../../Scene.h"



namespace iige::ecs::systems
	{
	class collision
		{
		public:
			virtual void operator()(iige::Scene& scene) const noexcept = 0;
		};

	template <size_t layers_count>
	class collision_impl : public collision
		{
		public:
			virtual void operator()(iige::Scene& scene) const noexcept final override
				{
				if (true)
					{
					auto collided{scene.ecs_registry.view<components::collided_with>()};
					collided.each([&](const entt::entity entity, const components::collided_with&)
						{
						scene.ecs_registry.remove<components::collided_with>(entity);
						});
					}
				if (true)
					{
					auto collided{scene.ecs_registry.view<components::collision_data>()};
					collided.each([&](const entt::entity entity, const components::collision_data&)
						{
						scene.ecs_registry.remove<components::collision_data>(entity);
						});
					}

				[&] <std::size_t... I>(std::index_sequence<I...>) {
					(evaluate<I>(scene), ...);
					}(std::make_index_sequence<layers_count>{});
				}
		private:
#pragma optimize("", off)
			template<size_t layer>
			void evaluate(iige::Scene& scene) const noexcept
				{
				auto targets/*TODO better names*/{scene.ecs_registry.view<components::has_collision<layer>, components::colliders::aabb, components::colliders::details::ptr>()};
				auto active /*TODO better names*/{scene.ecs_registry.view<components::collides_with<layer>, components::colliders::aabb, components::colliders::details::ptr>()};

				/*
				std::mutex adding_mutex;
				std::for_each(std::execution::par, active.begin(), active.end(), [&](const auto entity_a)
					{
					const components::colliders::aabb& a_aabb        {active.get<components::colliders::aabb>(entity_a)};
					components::colliders::ptr         a_collider_ptr{active.get<components::colliders::ptr >(entity_a)};
					/**/
				active.each([&](const entt::entity entity_a, const components::colliders::aabb& a_aabb, components::colliders::details::ptr a_collider_ptr)
					{
					targets.each([&](const entt::entity entity_b, const components::colliders::aabb& b_aabb, components::colliders::details::ptr b_collider_ptr)
						{
						if (entity_a == entity_b) { return; }

						if (shapes::collides(a_aabb.value(), b_aabb.value()))
							{
							bool collides{false};

							std::visit([&](const auto& a_collider_ptr)
								{
								std::visit([&](const auto& b_collider_ptr)
									{
									using a_collider_type = std::remove_reference<decltype(*a_collider_ptr)>::type;
									using b_collider_type = std::remove_reference<decltype(*b_collider_ptr)>::type;

									if constexpr (components::colliders::is_discrete_collider<a_collider_type>)
										{
										bool collides{shapes::collides(a_collider_ptr->value(), b_collider_ptr->value())};
										if (collides)
											{
											//std::unique_lock lock{adding_mutex};
											utils::discard(scene.ecs_registry.get_or_emplace<components::collided_with>(entity_a, entity_b));
											}
										}
									else if constexpr (components::colliders::is_continuous_collider<a_collider_type>)
										{
										bool inside{shapes::contains(b_collider_ptr->value(), a_collider_ptr->value().a)};

										auto result{utils::math::geometry::continuous_collides(a_collider_ptr->value(), b_collider_ptr->value(), inside)};
										if (result)
											{
											//std::unique_lock lock{adding_mutex};
											utils::discard(scene.ecs_registry.get_or_emplace<components::collided_with>(entity_a, entity_b));
											utils::discard(scene.ecs_registry.get_or_emplace<components::collision_data>(entity_a, components::collision_data{entity_b, result.value()}));
											}
										else if (inside)
											{
											utils::discard(scene.ecs_registry.get_or_emplace<components::collided_with>(entity_a, entity_b));
											}
										}
									}, b_collider_ptr);
								}, a_collider_ptr);
							}
						});
					});
				}
#pragma optimize("", on)
		};

	inline void update_colliders_vertex_array(const shapes::segment         & shape, sf::VertexArray& va, sf::Color c)
		{
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.a), c}); va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.b), c});
		}
	inline void update_colliders_vertex_array(const shapes::circle          & shape, sf::VertexArray& va, sf::Color c)
		{
		shape.center;
		shape.radius;

		iige::vec2f right{iige::vec2f::right() * shape.radius};
		iige::vec2f vec  {iige::vec2f::right() * shape.radius};

		utils::math::angle::deg delta_α{1000 / shape.radius};

		using namespace utils::math::angle::literals;
		for (utils::math::angle::deg α = delta_α; α.value < 360; α += delta_α)
			{
			va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(vec + shape.center), c});
			vec = right + α;
			va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(vec + shape.center), c});
			}
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(vec + shape.center), c});
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(right + shape.center), c});
		}
	inline void update_colliders_vertex_array(const shapes::polygon         & shape, sf::VertexArray& va, sf::Color c)
		{
		for (const auto& edge : shape.get_edges())
			{
			va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(edge.a), c});
			va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(edge.b), c});
			}
		}
	inline void update_colliders_vertex_array(const shapes::aabb            & shape, sf::VertexArray& va, sf::Color c)
		{
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.ul), c});
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.ur), c});
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.ur), c});
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.dr), c});
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.dr), c});
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.dl), c});
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.dl), c});
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.ul), c});
		}
	inline void update_colliders_vertex_array(const shapes::point           & shape, sf::VertexArray& va, sf::Color c)
			{
			shapes::aabb aabb{.ll{shape.x - 1}, .up{shape.y - 1}, .rr{shape.x + 1}, .dw{shape.y + 1}};
			update_colliders_vertex_array(aabb, va, c);
			}
	inline void update_colliders_vertex_array(const utils::math::geometry::continuous_point& shape, sf::VertexArray& va, sf::Color c)
		{
		update_colliders_vertex_array(shape.a, va, c);
		update_colliders_vertex_array(static_cast<shapes::segment>(shape), va, c);

		vec2f arrow_hand_ll{shape.b + (shape.perpendicular_left () * 4.f) - (shape.forward() * 4.f)};
		vec2f arrow_hand_rr{shape.b + (shape.perpendicular_right() * 4.f) - (shape.forward() * 4.f)};
		update_colliders_vertex_array(shapes::segment{shape.b, arrow_hand_ll}, va, c);
		update_colliders_vertex_array(shapes::segment{shape.b, arrow_hand_rr}, va, c);
		}

	template <components::colliders::is_collider T>
	inline void update_colliders_vertex_array(iige::Scene& scene, sf::VertexArray& va)
		{
		auto colliders_not_colliding{scene.ecs_registry.view<T>(entt::exclude<components::collided_with>)};
		auto colliders_____colliding{scene.ecs_registry.view<T, components::collided_with>()};

		sf::Color c;

		c = sf::Color::White;
		colliders_not_colliding.each([&](entt::entity entity, const T& shape)
			{
			update_colliders_vertex_array(shape.value(), va, c);
			});
		c = sf::Color::Red;
		colliders_____colliding.each([&](entt::entity entity, const T& shape, components::collided_with)
			{
			update_colliders_vertex_array(shape.value(), va, c);
			});
		}

	template <>
	inline void update_colliders_vertex_array<components::colliders::aabb>(iige::Scene& scene, sf::VertexArray& va)
		{
		using T = components::colliders::aabb;
		auto colliders_not_colliding{scene.ecs_registry.view<T>(entt::exclude<components::collided_with, components::colliders::segment, components::colliders::circle, components::colliders::polygon, components::colliders::convex_polygon, components::colliders::continuous_point>)};
		auto colliders_____colliding{scene.ecs_registry.view<T, components::collided_with>(entt::exclude<components::colliders::segment, components::colliders::circle, components::colliders::polygon, components::colliders::convex_polygon, components::colliders::continuous_point>)};

		sf::Color c;

		c = sf::Color::White;
		colliders_not_colliding.each([&](entt::entity entity, const T& shape)
			{
			update_colliders_vertex_array(shape.value(), va, c);
			});
		c = sf::Color::Red;
		colliders_____colliding.each([&](entt::entity entity, const T& shape, components::collided_with)
			{
			update_colliders_vertex_array(shape.value(), va, c);
			});
		}

	inline sf::VertexArray& update_colliders_vertex_array(iige::Scene& scene, sf::VertexArray& va)
		{
		va.clear();

		update_colliders_vertex_array<components::colliders::point           >(scene, va);
		update_colliders_vertex_array<components::colliders::segment         >(scene, va);
		update_colliders_vertex_array<components::colliders::aabb            >(scene, va);
		update_colliders_vertex_array<components::colliders::circle          >(scene, va);
		update_colliders_vertex_array<components::colliders::polygon         >(scene, va);
		update_colliders_vertex_array<components::colliders::convex_polygon  >(scene, va);
		update_colliders_vertex_array<components::colliders::continuous_point>(scene, va);

		return va;
		}
	}