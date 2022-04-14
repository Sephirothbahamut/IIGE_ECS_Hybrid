//#include <SFML/Graphics.hpp>
//
//#include <sstream>
//
//#include <utils/memory.h>
//
//#include <utils/math/geometry/polygon.h>
//#include <utils/math/geometry/circle.h>
//#include <utils/math/geometry/transformations.h>
//#include <utils/math/geometry/interactions.h>
//
//namespace utmg = utils::math::geometry;
//namespace utm = utils::math;
//
//inline sf::Font font;
//
//
//sf::VertexArray to_sf(const utmg::polygon& polygon, const sf::Color& color) noexcept
//	{
//	sf::VertexArray ret{sf::LineStrip, polygon.get_vertices().size() + 1};
//	for (size_t i = 0; i < polygon.get_vertices().size(); i++)
//		{
//		const auto& vertex{polygon.get_vertices()[i]};
//		ret[i].position = {vertex.x, vertex.y};
//		ret[i].color = color;
//
//		std::stringstream ss;
//		ss << vertex.x << ", " << vertex.y;
//
//		sf::Text text{ss.str(), font};
//		text.setPosition(vertex.x, vertex.y);
//		text.setFillColor(sf::Color::Yellow);
//		}
//	ret[polygon.get_vertices().size()] = ret[0];
//
//	return ret;
//	}
//
//void stuff(const utmg::polygon& polygon, sf::RenderTarget& rt)
//	{
//	for (size_t i = 0; i < polygon.get_vertices().size(); i++)
//		{
//		const auto& vertex{polygon.get_vertices()[i]};
//
//		std::stringstream ss;
//		ss << vertex.x << ", " << vertex.y;
//
//		sf::Text text{ss.str(), font, 12};
//		text.setPosition(vertex.x, vertex.y);
//		text.setFillColor(sf::Color::Yellow);
//
//		rt.draw(text);
//		}
//	}
//
//sf::CircleShape to_sf(const utmg::circle& circle, const sf::Color& color) noexcept
//	{
//	sf::CircleShape s{circle.radius};
//	s.setPosition({circle.center.x, circle.center.y});
//	s.setFillColor(sf::Color::Transparent);
//	s.setOutlineColor(color);
//	s.setOutlineThickness(1);
//	s.setOrigin(circle.radius, circle.radius);
//	return s;
//	}
//sf::VertexArray to_sf(const utmg::segment& segment, const sf::Color& color) noexcept
//	{
//	sf::VertexArray ret{sf::LineStrip, 2};
//
//	ret[0].position = {segment.a.x, segment.a.y};
//	ret[1].position = {segment.b.x, segment.b.y};
//	ret[0].color = color;
//	ret[1].color = color;
//
//	return ret;
//	}
//
//sf::VertexArray to_sf(const utm::vec2f& point, const sf::Color& color) noexcept
//	{
//	sf::VertexArray ret{sf::Points, 1};
//
//	ret[0].position = {point.x, point.y};
//	ret[0].color = color;
//
//	return ret;
//	}
//
//sf::RectangleShape to_sf(const utmg::aabb& aabb, const sf::Color& color) noexcept
//	{
//	sf::RectangleShape ret{{aabb.width, aabb.height}};
//
//	ret.setPosition(utm::vec_cast<sf::Vector2, float>(aabb.ul));
//	ret.setOutlineColor(color);
//	ret.setOutlineThickness(1);
//	ret.setFillColor(sf::Color::Transparent);
//
//	return ret;
//	}
//
//int mainm()
//	{
//	if (!font.loadFromFile("data/fonts/consola.ttf")) { return 0; }
//
//	using namespace utils::angle::literals;
//	using namespace utils::math::geometry::transformations;
//	using poly_convex = utmg::convex_polygon;
//	using poly_gon = utmg::polygon;
//
//	sf::RenderWindow rw{{800, 600}, "pippo"};
//	rw.setFramerateLimit(60);
//
//	utmg::segment src_segment{{-10, 0}, {10, 0}};
//	utm::vec2f   src_point{0, 0};
//	//poly_convex   src_poly{{{-10, -10}, {10, -10}, {10, 10},         {-10, 10}}};
//	poly_gon      src_cttv{{{-10, -10}, {-5, 0}, { 10, -10 }, {10, 10}, {0, 0}, {-10, 10}}};
//	utmg::circle  src_circle{{0, 0}, 10};
//	utmg::aabb    src_aabb{.ll = 0, .up = 0, .rr = 20, .dw = 10};
//	
//	poly_convex src_poly{{0, 0}, {100, 50}, {50, 100}};
//
//	utm::transform2	tr_segment{{200, 300},  58_deg, 1.f};
//	utm::transform2	tr_point{{100, 400},  24_deg, 1.f};
//	utm::transform2	tr_poly{{250, 300}, /*105_deg*/0_deg, 1.f};
//	utm::transform2	tr_cttv{{250, 300}, 105_deg, 1.f};
//	utm::transform2	tr_circle{{350, 400},  75_deg, 1.f};
//	utm::transform2	tr_aabb{{350, 400},  75_deg, 1.f};
//
//	utmg::segment segment{src_segment * tr_segment};
//	utm::vec2f   point{src_point * tr_point};
//	poly_convex   poly{src_poly * tr_poly};
//	poly_gon      cttv{src_cttv * tr_cttv};
//	utmg::circle  circle{src_circle * tr_circle};
//	utmg::aabb    aabb{src_aabb * tr_aabb};
//
//	utils::observer_ptr<utm::transform2> selected_transform{nullptr};
//
//
//	auto side{segment.point_side(point)};
//
//	while (rw.isOpen())
//		{
//
//		//input
//		sf::Event event;
//		while (rw.pollEvent(event))
//			{
//			switch (event.type)
//				{
//				case sf::Event::Closed: rw.close(); break;
//
//				case sf::Event::KeyPressed:
//					if (true)
//						{
//						switch (event.key.code)
//							{
//							case sf::Keyboard::Num1: selected_transform = &tr_segment; break;
//							case sf::Keyboard::Num2: selected_transform = &tr_point;   break;
//							case sf::Keyboard::Num3: selected_transform = &tr_poly;    break;
//							case sf::Keyboard::Num4: selected_transform = &tr_cttv;    break;
//							case sf::Keyboard::Num5: selected_transform = &tr_circle;  break;
//							case sf::Keyboard::Num6: selected_transform = &tr_aabb;    break;
//							}
//
//						if (selected_transform != nullptr)
//							{
//							switch (event.key.code)
//								{
//								case sf::Keyboard::Right: selected_transform->position.x++; break;
//								case sf::Keyboard::Left:  selected_transform->position.x--; break;
//								case sf::Keyboard::Up:    selected_transform->position.y--; break;
//								case sf::Keyboard::Down:  selected_transform->position.y++; break;
//
//								case sf::Keyboard::PageUp:   selected_transform->orientation -= 1_deg; break;
//								case sf::Keyboard::PageDown: selected_transform->orientation += 1_deg; break;
//
//								case sf::Keyboard::Add:      selected_transform->size *= 2; break;
//								case sf::Keyboard::Subtract: selected_transform->size /= 2; break;
//								}
//							}
//						segment = src_segment * tr_segment;
//						point   = src_point   * tr_point;
//						poly    = src_poly    * tr_poly;
//						cttv    = src_cttv    * tr_cttv;
//						circle  = src_circle  * tr_circle;
//						aabb    = src_aabb    * tr_aabb;
//						}
//				}
//			}
//		//step
//		bool st{utmg::collides(segment, point)};
//		bool sp{utmg::collides(segment, poly)};
//		bool sv{utmg::collides(segment, cttv)};
//		bool tp{utmg::collides(point, poly)};
//		bool tv{utmg::collides(point, cttv)};
//		bool pc{utmg::collides(poly, circle)};
//		bool vc{utmg::collides(cttv, circle)};
//		bool tc{utmg::collides(point, circle)};
//		bool cs{utmg::collides(circle, segment)};
//		bool pv{utmg::collides(poly, cttv)};
//		
//		bool at{utmg::collides(aabb, point)};
//		bool ap{utmg::collides(aabb, poly)};
//		bool as{utmg::collides(aabb, segment)};
//		bool av{utmg::collides(aabb, cttv)};
//		bool ac{utmg::collides(aabb, circle)};
//
//
//		//draw
//		rw.clear();
//
//		rw.draw(to_sf(segment, (st || sp || cs || sv || as) ? sf::Color::Red : sf::Color::White));
//		rw.draw(to_sf(point,   (tp || tc || st || tv || at) ? sf::Color::Red : sf::Color::White));
//		rw.draw(to_sf(poly,    (tp || sp || pc || pv || ap) ? sf::Color::Red : sf::Color::White));
//		rw.draw(to_sf(circle,  (pc || tc || cs || vc || ac) ? sf::Color::Red : sf::Color::White));
//		rw.draw(to_sf(cttv,    (pv || vc || tv || sv || av) ? sf::Color::Red : sf::Color::White));
//		rw.draw(to_sf(aabb,    (at || ap || as || av || ac) ? sf::Color::Red : sf::Color::White));
//
//		stuff(cttv, rw);
//
//		rw.display();
//		}
//	return 0;
//	}
//
//
//
//#include <deque>
//#include <set>
//#include <stack>
//
//#include <utils/memory.h>
//#include <utils/tracking.h>
//
//#include <utils/math/geometry/polygon.h>
//#include <utils/math/geometry/circle.h>
//#include <utils/math/geometry/transformations.h>
//#include <utils/math/geometry/interactions.h>
//
//#include <SFML/Graphics.hpp>
//#include "../entt.h"
//
//#include <utils/containers/polymorphic_container.h>
//
//	struct indexed
//		{
//		size_t index{};
//		};
//
//	template <typename T>
//	class batch_container
//		{
//		template <typename T>
//		struct indexed_pair : utils::wrapper<T>, public utils::trackable
//			{
//			template <typename ...Args>
//			indexed_pair(Args&&... args) : utils::wrapper<T>{std::forward<Args>(args)...} {}
//
//			size_t index;
//			};
//
//		public:
//			void remove(const utils::tracking_ptr<indexed_pair<T>>& target) noexcept
//				{
//				objects_to_be_removed.insert(target->index);
//				}
//
//			template<typename ... Args>
//			utils::tracking_ptr<indexed_pair<T>> emplace(Args ... args) noexcept
//				{
//				return {objects_to_be_added.emplace(std::forward<Args>(args)...)};
//				}
//
//			void update() noexcept
//				{
//				// Objects to be destroyed replace the place of objects to be added in the objects container.
//				std::set<size_t>::iterator objects_to_be_removed_it{objects_to_be_removed.begin()};
//				size_t delta_size{objects_to_be_removed.size() - objects_to_be_added.size()};
//				while (!objects_to_be_added.empty() && objects_to_be_removed_it != objects_to_be_removed.end())
//					{
//					objects[*objects_to_be_removed_it] = std::move(objects_to_be_added.top());
//					objects[*objects_to_be_removed_it].index = *objects_to_be_removed_it;
//
//					objects_to_be_removed_it++;
//					objects_to_be_added.pop();
//					}
//				if (!objects_to_be_added.empty())
//					{
//					do
//						{
//						size_t index{objects.size()};
//						auto& new_object{objects.emplace_back(std::move(objects_to_be_added.top()))};
//						new_object.index = index;
//
//						objects_to_be_added.pop();
//						} while (!objects_to_be_added.empty());
//					}
//				else
//					{
//					size_t swap_candidate_index{objects.size() - 1}; //TODO check if size is 0
//					std::set<size_t>::iterator last_objects_to_be_removed_it{objects_to_be_removed.end()};
//					last_objects_to_be_removed_it--;
//					while (true)
//						{
//						while (
//							swap_candidate_index > 0
//							&& swap_candidate_index == *last_objects_to_be_removed_it
//							&& std::prev(objects_to_be_removed_it) != last_objects_to_be_removed_it)
//							{
//							swap_candidate_index--; last_objects_to_be_removed_it--;
//							}
//						if (std::prev(objects_to_be_removed_it) == last_objects_to_be_removed_it) { break; }
//
//						objects[*objects_to_be_removed_it] = std::move(objects[swap_candidate_index]);
//						objects[*objects_to_be_removed_it].index = *objects_to_be_removed_it;
//
//						swap_candidate_index--;
//						objects_to_be_removed_it++;
//						}
//
//					objects.resize(objects.size() - (delta_size));
//					}
//				objects_to_be_removed.clear();
//				}
//		private:
//			std::deque<indexed_pair<T>> objects;
//			std::set<size_t> objects_to_be_removed;
//			std::stack<indexed_pair<T>> objects_to_be_added;
//		};
//
//	class Object
//		{
//		public:
//			const entt::entity entity{entt::null};
//		private:
//		};
//
//
//
//	template <typename T>
//	struct indexed_pair : utils::wrapper<T>
//		{
//		template <typename ...Args>
//		indexed_pair(Args&&... args) : utils::wrapper<T>{std::forward<Args>(args)...} {}
//
//		size_t index;
//		};

