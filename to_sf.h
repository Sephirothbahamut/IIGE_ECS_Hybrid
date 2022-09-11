#pragma once

#include <utils/math/vec2.h>

#include <SFML/Graphics.hpp>

namespace iige
	{
	template <typename out_t, typename source_t>
	inline sf::Vector2<out_t> to_sf(const utils::math::vec2<source_t>& vec) noexcept { return {static_cast<out_t>(vec.x), static_cast<out_t>(vec.y)}; }
	}