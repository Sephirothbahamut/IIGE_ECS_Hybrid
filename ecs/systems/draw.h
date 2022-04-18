#pragma once

#include "../entt.h"

#include "../../Window.h"
#include "../../Scene.h"

#include "../components/bad_draw.h"
#include "../components/spatial.h"

namespace iige::ecs::systems
	{
	void draw(iige::Scene& scene, sf::RenderWindow& window)
		{
		auto movement_view{scene.ecs_registry.view<components::transform, components::bad_draw>()};

		movement_view.each([&](const iige::transform& transform, auto& drawable)
			{
			drawable.cs.setPosition(utils::math::vec_cast<sf::Vector2, float>(transform.position));
			drawable.cs.setRotation(transform.orientation.value);
			window.draw(drawable.cs);
			});
		}
	}