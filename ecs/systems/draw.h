#pragma once

#include <utils/math/vec2.h>

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

		if (true)
			{
			auto movement_view{scene.ecs_registry.view<components::transform::interpolated::translation, components::bad_draw>()};

			movement_view.each([&](const vec2f& translation, auto& drawable)
				{
				drawable.cs.setPosition({translation.x, translation.y});
				window.draw(drawable.cs);
				});
			}
		if (true)
			{
			auto movement_view{scene.ecs_registry.view<components::transform::interpolated::translation, components::transform::interpolated::rotation, components::bad_draw>()};

			movement_view.each([&](const vec2f& translation, const angle::rad& rotation, components::bad_draw& drawable)
				{
				drawable.cs.setPosition({translation.x, translation.y});
				drawable.cs.setRotation(static_cast<angle::deg>(rotation).value);
				window.draw(drawable.cs);
				});
			}
		if (true)
			{
			auto movement_view{scene.ecs_registry.view<components::transform::absolute::translation, components::transform::interpolated::rotation, components::bad_draw>(entt::exclude<components::transform::interpolated::translation>)};

			movement_view.each([&](const vec2f& translation, const angle::rad& rotation, components::bad_draw& drawable)
				{
				drawable.cs.setPosition({translation.x, translation.y});
				drawable.cs.setRotation(static_cast<angle::deg>(rotation).value);
				window.draw(drawable.cs);
				});
			}
		}
	}