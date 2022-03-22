#pragma once

#include <SFML/Graphics.hpp>

namespace iige::ecs::components
	{
	struct bad_draw
		{
		bad_draw(float radius) : circle{radius} {}

		sf::CircleShape circle;
		};
	}