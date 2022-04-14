#pragma once

#include <utility>
#include <mutex>
#include <execution>

#include <utils/math/geometry/interactions.h>
#include <utils/definitions.h>
#include <utils/variant.h>
#include <utils/math/angle.h>

#include "../entt.h"

#include "../components/collision.h"
#include "../components/spatial.h"

#include "../../Window.h"
#include "../../Scene.h"


namespace iige::ecs::systems
	{
	namespace
		{
		namespace utmg = utils::math::geometry;
		namespace utm = utils::math;
		}

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
				auto collided{scene.ecs_registry.view<components::collided_with>()};
				collided.each([&](const entt::entity entity, const components::collided_with&)
					{
					scene.ecs_registry.remove<components::collided_with>(entity);
					});

				[&] <std::size_t... I>(std::index_sequence<I...>) {
					(evaluate<I>(scene), ...);
					}(std::make_index_sequence<layers_count>{});
				}
		private:
			template<size_t layer>
			void evaluate(iige::Scene& scene) const noexcept
				{
				auto targets/*TODO better names*/{scene.ecs_registry.view<components::has_collision<layer>, components::colliders::aabb, components::colliders::ptr>()};
				auto active /*TODO better names*/{scene.ecs_registry.view<components::collides_with<layer>, components::colliders::aabb, components::colliders::ptr>()};

				std::mutex adding_mutex;

				/*std::for_each(std::execution::par, active.begin(), active.end(), [&](const auto entity_a)
					{
					const components::colliders::aabb& a_aabb        {active.get<components::colliders::aabb>(entity_a)};
					components::colliders::ptr         a_collider_ptr{active.get<components::colliders::ptr >(entity_a)};
					/**/
				active.each([&](const entt::entity entity_a, const components::colliders::aabb& a_aabb, components::colliders::ptr a_collider_ptr)
					{
					targets.each([&](const entt::entity entity_b, const components::colliders::aabb& b_aabb, components::colliders::ptr b_collider_ptr)
						{
						if (entity_a == entity_b) { return; }

						if (utmg::collides(a_aabb.data, b_aabb.data))
							{
							bool collides{false};

							std::visit([&](const auto& a_collider_ptr)
								{
								std::visit([&](const auto& b_collider_ptr)
									{
									bool collides{utmg::collides(a_collider_ptr->data, b_collider_ptr->data)};
									if (collides) 
										{
										std::unique_lock lock{adding_mutex};
										utils::discard(scene.ecs_registry.get_or_emplace<components::collided_with>(entity_a, entity_b)); 
										}
									}, b_collider_ptr);
								}, a_collider_ptr);

							}
						});
					});


				/*active.each([&](const entt::entity entity_a, const components::colliders::aabb& a_aabb, components::colliders::ptr a_collider_ptr)
					{
					targets.each([&](const entt::entity entity_b, const components::colliders::aabb& b_aabb, components::colliders::ptr b_collider_ptr)
						{
						if (entity_a == entity_b) { return; }

						if (utmg::collides(a_aabb.data, b_aabb.data))
							{
							bool collides{false};


							std::visit([&](const auto& a_collider_ptr)
								{
								std::visit([&](const auto& b_collider_ptr)
									{
									bool collides{utmg::collides(a_collider_ptr->data, b_collider_ptr->data)};
									if (collides) { utils::discard(scene.ecs_registry.get_or_emplace<components::collided_with>(entity_a, entity_b)); }
									}, b_collider_ptr);
								}, a_collider_ptr);

							}
						});
					});*/
				}
		};

	inline void update_colliders_vertex_array(const components::utmg::segment& shape, sf::VertexArray& va, sf::Color c)
		{
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.a), c}); va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.b), c});
		}
	inline void update_colliders_vertex_array(const components::utmg::circle & shape, sf::VertexArray& va, sf::Color c)
		{
		shape.center;
		shape.radius;

		utm::vec2f right{utm::vec2f::right() * shape.radius};
		utm::vec2f vec{utm::vec2f::right() * shape.radius};

		utils::angle::deg delta_α{1000 / shape.radius};

		using namespace utils::angle::literals;
		for (utils::angle::deg α = delta_α; α.value < 360; α += delta_α)
			{
			va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(vec + shape.center), c});
			vec = right + α;
			va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(vec + shape.center), c});
			}
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(vec + shape.center), c});
		va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(right + shape.center), c});
		}
	inline void update_colliders_vertex_array(const components::utmg::polygon& shape, sf::VertexArray& va, sf::Color c)
		{
		for (const auto& edge : shape.get_edges())
			{
			va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(edge.a), c});
			va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(edge.b), c});
			}
		}
	inline void update_colliders_vertex_array(const components::utmg::aabb   & shape, sf::VertexArray& va, sf::Color c)
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

	template <components::colliders::is_collider T>
	inline void update_colliders_vertex_array(iige::Scene& scene, sf::VertexArray& va)
		{
		auto colliders_not_colliding{scene.ecs_registry.view<T>(entt::exclude<components::collided_with>)};
		auto colliders_____colliding{scene.ecs_registry.view<T, components::collided_with>()};

		sf::Color c;

		c = sf::Color::White;
		colliders_not_colliding.each([&](entt::entity entity, const T& shape)
			{
			update_colliders_vertex_array(shape.data, va, c);
			});
		c = sf::Color::Red;
		colliders_____colliding.each([&](entt::entity entity, const T& shape, components::collided_with)
			{
			update_colliders_vertex_array(shape.data, va, c);
			});
		}

	template <>
	inline void update_colliders_vertex_array<components::colliders::aabb>(iige::Scene& scene, sf::VertexArray& va)
		{
		using T = components::colliders::aabb;
		auto colliders_not_colliding{scene.ecs_registry.view<T>(entt::exclude<components::collided_with, components::colliders::segment, components::colliders::circle, components::colliders::polygon>)};
		auto colliders_____colliding{scene.ecs_registry.view<T, components::collided_with>(entt::exclude<components::colliders::segment, components::colliders::circle, components::colliders::polygon>)};

		sf::Color c;

		c = sf::Color::White;
		colliders_not_colliding.each([&](entt::entity entity, const T& shape)
			{
			update_colliders_vertex_array(shape.data, va, c);
			});
		c = sf::Color::Red;
		colliders_____colliding.each([&](entt::entity entity, const T& shape, components::collided_with)
			{
			update_colliders_vertex_array(shape.data, va, c);
			});
		}

	inline sf::VertexArray& update_colliders_vertex_array(iige::Scene& scene, sf::VertexArray& va)
		{
		va.clear();

		update_colliders_vertex_array<components::colliders::segment>(scene, va);
		update_colliders_vertex_array<components::colliders::aabb   >(scene, va);
		update_colliders_vertex_array<components::colliders::circle >(scene, va);
		update_colliders_vertex_array<components::colliders::polygon>(scene, va);

		return va;
		}
	}