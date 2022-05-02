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
		//TODO figure out compound transform view :|

		auto movement_view{scene.ecs_registry.view<components::transform::interpolated::x, components::transform::interpolated::y, components::bad_draw>()};

		movement_view.each([&](const float& x, const float& y, auto& drawable)
			{
			drawable.cs.setPosition({x, y});
			window.draw(drawable.cs);
			});
		}
	}