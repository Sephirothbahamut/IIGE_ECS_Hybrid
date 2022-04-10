#pragma once

#include <utility>

#include <utils/math/geometry/interactions.h>
#include <utils/definitions.h>
#include <utils/variant.h>
#include <utils/math/angle.h>

#include <entt.h>

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
				auto targets/*TODO better names*/{scene.ecs_registry.view<components::has_collision<layer>, components::collider>()};
				auto active /*TODO better names*/{scene.ecs_registry.view<components::collides_with<layer>, components::collider>()};
				
				active.each([&](const entt::entity entity_a, const components::collider& a_collider_variant)
					{
					targets.each([&](const entt::entity entity_b, const components::collider& b_collider_variant)
						{
						if (entity_a == entity_b) { return; }

						std::visit([&](const auto& a_collider)
							{
							std::visit([&](const auto& b_collider)
								{
								if (!utmg::collides(static_cast<utmg::aabb>(a_collider), static_cast<utmg::aabb>(b_collider))) { return; }


								bool collides{utmg::collides(a_collider, b_collider)};
								//if (collides) { std::cout << "A collision!!!\n"; }
								if (collides) { utils::discard(scene.ecs_registry.get_or_emplace<components::collided_with>(entity_a, entity_b)); }
				
								}, b_collider_variant);
							}, a_collider_variant);
						});
					});
				}
		};


	inline sf::VertexArray& update_colliders_vertex_array(iige::Scene& scene, sf::VertexArray& va)
		{
		va.clear();

		auto colliders_not_colliding{scene.ecs_registry.view<components::collider>(entt::exclude<components::collided_with>)};
		auto colliders_____colliding{scene.ecs_registry.view<components::collider, components::collided_with>()};

		sf::Color c;

		utils::overloaded overloaded
			{
			[&](std::monostate&) {},
			[&](const utm::vec2f& shape) { va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape), c}); va.append({sf::Vector2f{shape.x + 1, shape.y}, c});},
			[&](const utmg::segment& shape) { va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.a), c}); va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(shape.b), c}); },
			[&](const utmg::circle& shape)
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
				va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(vec   + shape.center), c});
				va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(right + shape.center), c});
				},
			[&](const utmg::polygon& shape)
				{
				for (const auto& edge : shape.get_edges())
					{
					va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(edge.a), c});
					va.append(sf::Vertex{utils::math::vec_cast<sf::Vector2, float>(edge.b), c});
					}
				},
			[&](const utmg::aabb& shape)
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
			};
		
		c = sf::Color::White;
		colliders_not_colliding.each([&](const components::collider& drawable)
			{
			std::visit(overloaded, drawable);
			});
		c = sf::Color::Red;
		colliders_____colliding.each([&](const components::collider& drawable, components::collided_with)
			{
			std::visit(overloaded, drawable);
			});

		return va;
		}
	}