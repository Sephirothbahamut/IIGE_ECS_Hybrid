#pragma once
#include <utils/math/vec2.h>

#include <SFML/Graphics.hpp>

namespace iige
	{
	class display
		{
		public:
			static utils::math::vec2u get_screen_size()
				{
				auto tmp{sf::VideoMode::getDesktopMode()};
				return {tmp.width, tmp.height};
				}
		};

	class window
		{
		public:
			struct create_info
				{
				utils::math::vec2u size{800, 600};
				utils::math::vec2i position{static_cast<int>(display::get_screen_size().x) / 2, static_cast<int>(display::get_screen_size().y) / 2};
				std::string title{"iige::window"};
				};

			window(const create_info& create_info) : sf_window{sf::VideoMode{create_info.size.x, create_info.size.y}, create_info.title, sf::Style::Resize, sf::ContextSettings{0, 0, 8}}
				{
				sf_window.setPosition({static_cast<int>(create_info.position.x), static_cast<int>(create_info.position.y)});
				sf_window.setKeyRepeatEnabled(false);
				}

			window(const window& copy)            = delete;
			window& operator=(const window& copy) = delete;
			window(window&& move) noexcept        = default;
			window& operator=(window&& move)      = default;

			bool poll_event(sf::Event& event)
				{
				bool ret = sf_window.pollEvent(event);
				if (ret)
					{
					switch (event.type)
						{
						case sf::Event::EventType::Closed: close();
						}
					}
				return ret;
				}
			void close() { sf_window.close(); }
			bool is_open() { return sf_window.isOpen(); }

			sf::RenderWindow sf_window;
		private:
		};
	}