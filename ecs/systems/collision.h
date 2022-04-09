#pragma once

#include <utility>

#include <utils/math/geometry/interactions.h>
#include <utils/definitions.h>

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
		template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
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
				[&] <std::size_t... I>(std::index_sequence<I...>) {
					(evaluate<I>(scene), ...);
					}(std::make_index_sequence<layers_count>{});
				}
		private:
			template<size_t layer>
			void evaluate(iige::Scene& scene) const noexcept
				{
				//auto targets/*TODO better names*/{scene.ecs_registry.view<components::has_collision<layer>, components::collider>()};
				//auto active /*TODO better names*/{scene.ecs_registry.view<components::collides_with<layer>, components::collider>()};
				//
				////active.each([&](const components::collides_with<layer>& collides_with, const components::collider& a_collider_variant, const components::transform& a_transform)
				//active.each([&](const auto& collides_with, const components::collider& a_collider_variant)
				//	{
				//	targets.each([&](const auto& has_collision, const components::collider& b_collider_variant)
				//		{
				//		std::visit([&](const auto& a_collider)
				//			{
				//			std::visit([&](const auto& b_collider)
				//				{
				//				bool collides{utmg::collides(a_collider, b_collider)};
				//				if (collides) { std::cout << "A collision!!!\n"; }
				//				}, b_collider_variant);
				//			}, a_collider_variant);
				//		});
				//	});
				}
		};


	void draw_colliders(iige::Scene& scene, sf::RenderWindow& window)
		{
		sf::VertexArray va{sf::PrimitiveType::Lines};

		auto colliders_not_colliding{scene.ecs_registry.view<components::transform, components::collider>()};
		auto colliders_____colliding{scene.ecs_registry.view<components::transform, components::collider, components::collided_with>()};

		colliders_not_colliding.each([&](const utils::math::Transform2& transform, const components::collider& drawable)
			{


			});
		}
	}