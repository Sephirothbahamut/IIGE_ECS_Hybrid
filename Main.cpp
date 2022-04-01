
#include <deque>
#include <set>
#include <stack>

#include <utils/memory.h>
#include <utils/tracking.h>

#include <utils/math/geometry/polygon.h>
#include <utils/math/geometry/circle.h>
#include <utils/math/geometry/transformations.h>
#include <utils/math/geometry/interactions.h>

#include <SFML/Graphics.hpp>
#include <entt.h>

#include <utils/containers/polymorphic_container.h>

struct indexed
	{
	size_t index{};
	};

template <typename T>
class batch_container
	{
	template <typename T>
	struct indexed_pair : utils::wrapper<T>, public utils::trackable
		{
		template <typename ...Args>
		indexed_pair(Args&&... args) : utils::wrapper<T>{std::forward<Args>(args)...} {}

		size_t index;
		};

	public:
		void remove(const utils::tracking_ptr<indexed_pair<T>>& target) noexcept
			{
			objects_to_be_removed.insert(target->index); 
			}

		template<typename ... Args>
		utils::tracking_ptr<indexed_pair<T>> emplace(Args ... args) noexcept
			{
			return {objects_to_be_added.emplace(std::forward<Args>(args)...)};
			}

		void update() noexcept
			{
			// Objects to be destroyed replace the place of objects to be added in the objects container.
			std::set<size_t>::iterator objects_to_be_removed_it{objects_to_be_removed.begin()};
			size_t delta_size{objects_to_be_removed.size() - objects_to_be_added.size()};
			while (!objects_to_be_added.empty() && objects_to_be_removed_it != objects_to_be_removed.end())
				{
				objects[*objects_to_be_removed_it] = std::move(objects_to_be_added.top());
				objects[*objects_to_be_removed_it].index = *objects_to_be_removed_it;

				objects_to_be_removed_it++;
				objects_to_be_added.pop();
				}
			if (!objects_to_be_added.empty())
				{
				do
					{
					size_t index{objects.size()};
					auto& new_object{objects.emplace_back(std::move(objects_to_be_added.top()))};
					new_object.index = index;

					objects_to_be_added.pop();
					} 
				while (!objects_to_be_added.empty());
				}
			else
				{
				size_t swap_candidate_index{objects.size() - 1}; //TODO check if size is 0
				std::set<size_t>::iterator last_objects_to_be_removed_it{objects_to_be_removed.end()};
				last_objects_to_be_removed_it--;
				while (true)
					{
					while (
						swap_candidate_index > 0
						&& swap_candidate_index == *last_objects_to_be_removed_it
						&& std::prev(objects_to_be_removed_it) != last_objects_to_be_removed_it)
						{
						swap_candidate_index--; last_objects_to_be_removed_it--;
						}
					if (std::prev(objects_to_be_removed_it) == last_objects_to_be_removed_it) { break; }

					objects[*objects_to_be_removed_it] = std::move(objects[swap_candidate_index]);
					objects[*objects_to_be_removed_it].index = *objects_to_be_removed_it;

					swap_candidate_index--;
					objects_to_be_removed_it++;
					}

				objects.resize(objects.size() - (delta_size));
				}
			objects_to_be_removed.clear();
			}
	private:
		std::deque<indexed_pair<T>> objects;
		std::set<size_t> objects_to_be_removed;
		std::stack<indexed_pair<T>> objects_to_be_added;
	};

class Object
	{
	public:
		const entt::entity entity{entt::null};
	private:
	};



template <typename T>
struct indexed_pair : utils::wrapper<T>
	{
	template <typename ...Args>
	indexed_pair(Args&&... args) : utils::wrapper<T>{std::forward<Args>(args)...} {}

	size_t index;
	};






#include "Window.h"
#include "Scene.h"
#include "Loop.h"

#include "ecs/components/bad_draw.h"
#include "ecs/components/speed.h"
#include "ecs/components/transform.h"
#include "ecs/systems/collision.h"

int mainz()
	{
	using namespace utils::angle::literals;

	iige::Window window{iige::Window::create_info{.size{800, 600}, .position{10, 10}, .title{"My window"}}};

	iige::Scene scene;

	utils::math::Transform2 t{.position{200.f, 150.f}, .orientation{}};

	for (size_t i = 0; i < 3000; i++)
		{
		auto entity{scene.ecs_registry.create()};
		scene.ecs_registry.emplace<iige::ecs::components::transform>(entity, t);
		scene.ecs_registry.emplace<iige::ecs::components::speed>(entity, utils::math::Transform2{.position{10.f, 10.f}, .orientation{5_deg}});
		scene.ecs_registry.emplace<iige::ecs::components::bad_draw>(entity, 32.f);
		scene.ecs_registry.emplace<iige::ecs::components::interpolated>(entity);
		scene.ecs_registry.emplace<iige::ecs::components::transform_next>(entity, t);
		}

	iige::ecs::systems::collision_impl<2> collision;

	iige::Loop loop{scene, window, collision, .5f};

	loop.run();

	return 0;
	}
