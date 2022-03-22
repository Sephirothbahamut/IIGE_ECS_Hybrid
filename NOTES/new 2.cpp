

using position = vec2f;
using translation = vec2f;

using orientation = angle;

class rotation : angle 
	{
	lerp tra float 
	
	// DO ABSOFUCKINGLUTELY NOT USE ANGLE::LERP ON A ROTATION
	static deg lerp(deg a, deg b, float t) { return std::lerp(a.value, b.value, t); }
	}

transform { pos, or, float }
delta_transform { transl, rot, float }

transform + delta_transform = transform; // :) /*logically transform, but we need delta_transform :( */
private: logically_incorrect_plus(transform, delta_transform) -> delta_transform


in_world
	transform 	pos
				angle (0-360)
				scale
	
moving

	"speed"
	transform	transl
				rot (-inf, +inf)
				scale
				
	"target_in_world"	
	transform 	transl
				rot (-inf, +inf)
				scale
	
______________________________________________________________
begin_step

in_world        = target_in_world; //inside cast which clamps angle
target_in_world = our_plus(in_world, speed);



draw
interp(in_world, target_in_world) // interp(angle, rotation) return angle