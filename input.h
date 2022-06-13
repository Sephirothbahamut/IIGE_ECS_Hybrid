#pragma once

#include <array>
#include <tuple>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <unordered_map>

#include <utils/enum.h>
#include <utils/memory.h>
#include <utils/variant.h>
#include <utils/math/vec2.h>
#include <utils/containers/flat_set.h>
#include <utils/containers/handled_container.h>

#include "Window.h"

#include "ecs/systems/system.h"

namespace iige
	{
	class scene;
	class window;
	namespace loop::details
		{
		class base_loop;
		}
	}

namespace iige::input
	{
	enum class state_t { pressed, down, released, up };

	struct button_action
		{
		using callback_t = std::function<void(iige::scene&, iige::window&, float)>;
		using container_t = utils::containers::handled_container<callback_t>;

		callback_t pressed;
		callback_t down;
		callback_t released;
		};
	struct axis_1d_action
		{
		using callback_t = std::function<void(iige::scene&, iige::window&, float, float)>;
		using container_t = utils::containers::handled_container<callback_t>;

		container_t changed;
		container_t nonzero;
		};
	struct axis_2d_action
		{
		using callback_t = std::function<void(iige::scene&, iige::window&, float, utils::math::vec2f)>;
		using container_t = utils::containers::handled_container<callback_t>;

		container_t changed;
		container_t nonzero;
		};

	enum class mouse_axis { x, y };
	using mouse_button    = sf::Mouse::Button;
	using keyboard_key    = sf::Keyboard::Key;
	struct joystick_button { bool operator==(joystick_button const&) const = default; uint8_t joystick_id; uint8_t button_id;          };
	struct joystick_axis   { bool operator==(joystick_axis   const&) const = default; uint8_t joystick_id; sf::Joystick::Axis axis_id; };
	}

namespace std
	{
	//TODO make sure this is correct?
	template <>
	struct hash<iige::input::joystick_button>
		{//https://stackoverflow.com/questions/57319676/proper-modern-way-of-converting-two-uint8-t-into-int16-t
		size_t operator()(const iige::input::joystick_button& btn) const noexcept { return static_cast<unsigned>(btn.button_id) << 8 | static_cast<unsigned>(btn.joystick_id); }
		};
	template <>
	struct hash<iige::input::joystick_axis>
		{//https://stackoverflow.com/questions/57319676/proper-modern-way-of-converting-two-uint8-t-into-int16-t
		size_t operator()(const iige::input::joystick_axis& axs) const noexcept { return static_cast<unsigned>(axs.axis_id) << 8 | static_cast<unsigned>(axs.joystick_id); }
		};
	}

namespace iige::input
	{


	enum class input_t
		{
		
		};


	template <typename action_t, typename ...input_ts>
	class actions_manager
		{
		public:
			using actions_container_t = utils::containers::handled_container<action_t>;
			template <typename ...Args>
			actions_container_t::handle_t emplace_action(Args&& ...args)
				{
				return actions_container.emplace(std::forward<Args>(args)...);
				}
			void remove_action_associations(actions_container_t::handle_t& handle)
				{
				(dissociate_actions_of_type<input_ts>(handle), ...);
				}
		
			template <typename input_t>
			void associate_action(actions_container_t::handle_t& handle, input_t key)
				{
				auto& action_handle_to_inputs{get_action_handle_to_inputs<input_t>()};
				auto& input_to_action_handles{get_input_to_action_handles<input_t>()};
		
				action_handle_to_inputs[handle].push_back(key);
				input_to_action_handles[key].push_back(handle);
				}
		
			template <typename input_t>
			void dissociate_action(actions_container_t::handle_t& handle, input_t key)
				{
				auto& action_handle_to_inputs{get_action_handle_to_inputs<input_t>()};
		
				auto action_handle_to_inputs_it{action_handle_to_inputs.find(handle)};
				if (action_handle_to_inputs_it != action_handle_to_inputs.end())
					{
					auto vec{action_handle_to_inputs_it->second};
					vec.erase(std::remove(vec.begin(), vec.end(), key), vec.end());
		
					if (vec.empty()) { action_handle_to_inputs.erase(handle); }
					}
		
				auto& input_to_action_handles{get_input_to_action_handles<input_t>()};
				auto input_to_action_handles_it{input_to_action_handles.find(key)};
				if (input_to_action_handles_it != input_to_action_handles.end())
					{
					auto vec{input_to_action_handles_it->second};
					vec.erase(std::remove(vec.begin(), vec.end(), handle), vec.end());
		
					if (vec.empty()) { input_to_action_handles.erase(key); }
					}
				}
		
		protected:
			actions_container_t actions_container;
		
			template <typename input_t>
			using action_handle_to_inputs_t = std::unordered_map<typename actions_container_t::handle_t, std::vector<input_t>>;
		
			template <typename input_t>
			using input_to_action_handles_t = std::unordered_map<input_t, std::vector<typename actions_container_t::handle_t>>;
		
			template <typename input_t>
			action_handle_to_inputs_t<input_t>& get_action_handle_to_inputs()
				{
				return std::get<action_handle_to_inputs_t<input_t>>(action_handle_to_inputs_tuple);
				}
			template <typename input_t>
			input_to_action_handles_t<input_t>& get_input_to_action_handles()
				{
				return std::get<input_to_action_handles_t<input_t>>(input_to_action_handles_tuple);
				}

		private:
			std::tuple<action_handle_to_inputs_t<input_ts>...> action_handle_to_inputs_tuple;

			std::tuple<input_to_action_handles_t<input_ts>...> input_to_action_handles_tuple;

			template <typename input_t>
			void dissociate_actions_of_type(actions_container_t::handle_t& handle)
				{
				auto& action_handle_to_inputs{get_action_handle_to_inputs<input_t>()};
				auto& input_to_action_handles{get_input_to_action_handles<input_t>()};
		
				auto action_handle_to_inputs_it{action_handle_to_inputs.find(handle)};
				if (action_handle_to_inputs_it != action_handle_to_inputs.end())
					{
					auto vec{action_handle_to_inputs_it->second};
		
					for (auto key : vec)
						{
						auto input_to_action_handles_it{input_to_action_handles.find(key)};
						if (input_to_action_handles_it != input_to_action_handles.end())
							{
							auto vec{input_to_action_handles_it->second};
							vec.erase(std::remove(vec.begin(), vec.end(), handle), vec.end());
		
							if (vec.empty()) { input_to_action_handles.erase(key); }
							}
						}
		
					action_handle_to_inputs.erase(handle);
					}
				}
		};

	struct button_actions_manager : public actions_manager<button_action, keyboard_key, mouse_button, joystick_button>
		{
		public:
			template <state_t state, typename input_t>
			void evaluate_event_inner(input_t input, iige::scene& scene, iige::window& window, float delta_time)
				{
				auto it{get_input_to_action_handles<input_t>().find(input)};
				if (it == get_input_to_action_handles<input_t>().end()) { return; }
				auto vec{it->second};

				switch (state)
					{
					case iige::input::state_t::pressed : inner(scene, window, delta_time, vec, [](auto& a) { return a.pressed ; }); break;
					case iige::input::state_t::down    : inner(scene, window, delta_time, vec, [](auto& a) { return a.down    ; }); break;
					case iige::input::state_t::released: inner(scene, window, delta_time, vec, [](auto& a) { return a.released; }); break;
					case iige::input::state_t::up      : break; //no callback
					default: break;
					}
				}
		private:

			template <typename action_handles_t, typename action_state_getter_f>
			void inner(iige::scene& scene, iige::window& window, float delta_time, action_handles_t action_handles, action_state_getter_f f)
				{
				for (auto handle : action_handles)
					{
					auto& action{actions_container[handle]};
					f(action)(scene, window, delta_time);
					}
				}
		};

	class manager
		{
		public:
			manager()
				{
				value_keyboard_keys.resize(keyboard_key::KeyCount);
				value_mouse_buttons.resize(mouse_button::ButtonCount);
				}

			button_actions_manager button_actions;

			void events_begin()
				{
				}

			void evaluate_event(sf::Event& event, iige::scene& scene, iige::window& window, float delta_time)
				{
				switch (event.type)
					{
					case sf::Event::KeyPressed:
						if (event.key.code == -1) { return; }
						button_actions.evaluate_event_inner<state_t::pressed> (event.key.code, scene, window, delta_time);
						value_keyboard_keys[event.key.code] = 1.f;
						down_keyboard_keys.emplace(event.key.code);
						break;
					case sf::Event::KeyReleased:
						if (event.key.code == -1) { return; }
						button_actions.evaluate_event_inner<state_t::released>(event.key.code, scene, window, delta_time);
						value_keyboard_keys[event.key.code] = 0.f;
						down_keyboard_keys.remove(event.key.code);
						break;
						
					case sf::Event::MouseButtonPressed:
						if (event.key.code == -1) { return; }
						button_actions.evaluate_event_inner<state_t::pressed>(event.mouseButton.button, scene, window, delta_time);
						value_mouse_buttons[event.mouseButton.button] = 1.f;
						down_mouse_buttons.emplace(event.mouseButton.button);
						break;
					case sf::Event::MouseButtonReleased:
						if (event.key.code == -1) { return; }
						button_actions.evaluate_event_inner<state_t::released>(event.mouseButton.button, scene, window, delta_time);
						value_mouse_buttons[event.mouseButton.button] = 0.f;
						down_mouse_buttons.remove(event.mouseButton.button);
						break;

					case sf::Event::JoystickButtonPressed:
						if (true)
							{
							joystick_button btn{static_cast<uint8_t>(event.joystickButton.joystickId), static_cast<uint8_t>(event.joystickButton.button)};
							button_actions.evaluate_event_inner<state_t::pressed>(btn, scene, window, delta_time);
							value_joystick_buttons[event.joystickButton.joystickId][event.joystickButton.button] = 1.f;
							down_joystick_buttons.emplace(btn);
							}
						break;
					case sf::Event::JoystickButtonReleased:
						if (true)
							{
							joystick_button btn{static_cast<uint8_t>(event.joystickButton.joystickId), static_cast<uint8_t>(event.joystickButton.button)};
							button_actions.evaluate_event_inner<state_t::released>(btn, scene, window, delta_time);
							value_joystick_buttons[event.joystickButton.joystickId][event.joystickButton.button] = 0.f;
							down_joystick_buttons.remove(btn);
							}
						break;
					}
				}

			void events_end(iige::scene& scene, iige::window& window, float delta_time)
				{
				for (auto key : down_keyboard_keys)
					{
					button_actions.evaluate_event_inner<state_t::down>(key, scene, window, delta_time);
					}
				for (auto button : down_mouse_buttons)
					{
					button_actions.evaluate_event_inner<state_t::down>(button, scene, window, delta_time);
					}
				for (auto button : down_joystick_buttons)
					{
					button_actions.evaluate_event_inner<state_t::down>(button, scene, window, delta_time);
					}
				}

			//bool is_key_down(keyboard_key key) { return down[key]; }

		private:
			std::vector<float> value_keyboard_keys;
			std::vector<float> value_mouse_buttons;
			std::array<std::array<float, 32>, 8> value_joystick_buttons{};
			std::array<std::array<float,  6>, 8> value_joystick_axes   {};

			utils::containers::tiny_set<keyboard_key   > down_keyboard_keys;
			utils::containers::tiny_set<mouse_button   > down_mouse_buttons;
			utils::containers::tiny_set<joystick_button> down_joystick_buttons;
			utils::containers::tiny_set<joystick_axis  > down_joystick_axes;
		};
	}