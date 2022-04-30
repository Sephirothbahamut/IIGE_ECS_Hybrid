#pragma once

#include <utils/math/vec2.h>
#include <utils/math/Transform2.h>
#include <utils/math/angle.h>

namespace iige
	{
	template <typename T>
	using vec2 = utils::math::vec2<T>;

	using vec2i   = utils::math::vec2i  ;
	using vec2i8  = utils::math::vec2i8 ;
	using vec2i16 = utils::math::vec2i16;
	using vec2i32 = utils::math::vec2i32;
	using vec2i64 = utils::math::vec2i64;

	using vec2u   = utils::math::vec2u  ;
	using vec2u8  = utils::math::vec2u8 ;
	using vec2u16 = utils::math::vec2u16;
	using vec2u32 = utils::math::vec2u32;
	using vec2u64 = utils::math::vec2u64;

	using vec2s   = utils::math::vec2s;
	using vec2f   = utils::math::vec2f;
	using vec2d   = utils::math::vec2d;

	using transform = utils::math::transform2;

	namespace angle = utils::beta::math::angle;
	}