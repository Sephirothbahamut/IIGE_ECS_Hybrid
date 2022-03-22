CPU
objects
Object* []
A       []
B : collide<pt76> []
C : collide<poly> []
D : collide<circ> []
E : collide<segm> []


pluto : collide<pt76, 6> {}
pippo : collide<pt76, 5>
	{
		
	virtual void resolve(pluto instance)
		{
		...
		}
	}

template <collider collider_t, size_t layer_id>
class collide
	{
	collide()
		{
		unique_id{collision_manager.add<layer_id, T>...)};
		}
	~collide()
		{
		collision_manager.remove<layer_id, T>(unique_id);
		}
		
	//move operations, update collider.owner
	
		
	template <collide T>
	virtual void resolve(collide<T::collider_t, T::layer_id>)
	}
	
class collision_manager
	{
	std::vector<collision_layer_manager> layers;
	
	utils::matrix<bool> collisions_matrix;
	
	void step()
		{
		foreach pair in collisions_matrix
			if(collisions_matrix[a, b]) { a.check(b); }
		}
	}
	
//assuming GPU
//class dismembered_polygon
//	{
//	edge edge;
//	unique_id id;
//	}
	

class collision_layer_manager
	{
	std::vector<pt76>
	std::vector<circ>
	std::vector<segm>
	//assuming CPU
	std::vector<poly>
	//assuming GPU
	//std::vector<dismembered_polygon>
	
	check(layer other)
		{
		foreach container
			foreach other.container
				magic
				//basic magic
				foreach a = container.element
					foreach b = other.container.element
						bool result{check(a, b)}
						if(result)
							{
							a.owner->resolve(b->owner);
							}
				//not so basic magic
				broad pass
		}
	}
	
//collider
template <typename T>
class collider
	{
	collide<T>* owner
	}


using point = vec2f;
class circle { vec2f, float }
class polygon { std::vector<vec2f> }

