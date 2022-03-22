#pragma once

#include <entt.h>

#include "../../Window.h"
#include "../../Scene.h"

#include "../../sfml_interfacing/vec.h"

#include "../components/bad_draw.h"
#include "../components/transform.h"
#include "../components/speed.h"

namespace iige::ecs::systems
	{
	void draw(const Scene& scene, sf::RenderWindow& window)
		{
		/*auto static_view{scene.ecs_registry.view<components::bad_draw, components::transform>(entt::exclude<components::interpolated>)};

		static_view.each([&](components::bad_draw& drawable, const components::transform& transform)
			{
			sf::CircleShape c{drawable}; c.setPosition(sfml::vec_cast<sf::Vector2, float>(transform.t.position));
			window.draw(c);
			});*/

		auto movement_view{scene.ecs_registry.view<components::interpolated, components::bad_draw>()};

		movement_view.each([&](const auto& transform, auto& drawable)
			{
			//drawable.circle.setPosition(sfml::vec_cast<sf::Vector2, float>(transform.t.position));
			window.draw(drawable.circle);
			});
		}
	}