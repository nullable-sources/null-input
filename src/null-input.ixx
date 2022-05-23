module;
#include <Windows.h>
#include <windowsx.h>
export module null.input;

import std.core;
import null.sdk;

namespace null { enum class e_key_id; }
template<> struct utils::enum_reflection::range<null::e_key_id> {
	static constexpr int min = 0, max = (int)VK_OEM_7;
};

export namespace null {
	enum class e_key_id {
		none,

		mouse_left = VK_LBUTTON, mouse_right = VK_RBUTTON,
		mouse_middle = VK_MBUTTON,
		mouse_x1 = VK_XBUTTON1, mouse_x2 = VK_XBUTTON2,

		left_shift = VK_LSHIFT, right_shift = VK_RSHIFT,
		left_ctrl = VK_LCONTROL, right_ctrl = VK_RCONTROL,
		left_alt = VK_LMENU, right_alt = VK_RMENU,

		cancel = VK_CANCEL,
		backspace = VK_BACK,
		tab = VK_TAB,
		clear = VK_CLEAR,
		enter = VK_RETURN,

		pause = VK_PAUSE,
		caps_lock = VK_CAPITAL,
		escape = VK_ESCAPE,
		space = VK_SPACE,
		page_up = VK_PRIOR,
		page_down = VK_NEXT,
		end = VK_END,
		home = VK_HOME,
		left = VK_LEFT,
		up = VK_UP,
		right = VK_RIGHT,
		down = VK_DOWN,
		print_screen = VK_SNAPSHOT,
		insert = VK_INSERT,
		del = VK_DELETE,

		key_0 = '0', key_1 = '1', key_2 = '2', key_3 = '3', key_4 = '4',
		key_5 = '5', key_6 = '6', key_7 = '7', key_8 = '8', key_9 = '9',

		a = 'A', b = 'B', c = 'C', d = 'D', e = 'E',
		f = 'F', g = 'G', h = 'H', i = 'I', j = 'J',
		k = 'K', l = 'L', m = 'M', n = 'N', o = 'O',
		p = 'P', q = 'Q', r = 'R', s = 'S', t = 'T',
		u = 'U', v = 'V', w = 'W', x = 'X', y = 'Y',
		z = 'Z',

		win = VK_LWIN,
		app = VK_APPS,

		num_lock = VK_NUMLOCK,
		num_0 = VK_NUMPAD0, num_1 = VK_NUMPAD1, num_2 = VK_NUMPAD2, num_3 = VK_NUMPAD3, num_4 = VK_NUMPAD4,
		num_5 = VK_NUMPAD5, num_6 = VK_NUMPAD6, num_7 = VK_NUMPAD7, num_8 = VK_NUMPAD8, num_9 = VK_NUMPAD9,
		num_multiply = VK_MULTIPLY, num_add = VK_ADD, num_subtract = VK_SUBTRACT, num_decimal = VK_DECIMAL, num_divide = VK_DIVIDE,

		f1 = VK_F1, f2 = VK_F2, f3 = VK_F3, f4 = VK_F4, f5 = VK_F5,
		f6 = VK_F6, f7 = VK_F7, f8 = VK_F8, f9 = VK_F9, f10 = VK_F10,
		f11 = VK_F11, f12 = VK_F12,

		scroll_lock = VK_SCROLL,

		oem_plus = VK_OEM_PLUS, oem_comma = VK_OEM_COMMA, oem_minus = VK_OEM_MINUS, oem_period = VK_OEM_PERIOD,
		oem_1 = VK_OEM_1, oem_2 = VK_OEM_2, oem_3 = VK_OEM_3, oem_4 = VK_OEM_4, oem_5 = VK_OEM_5, oem_6 = VK_OEM_6, oem_7 = VK_OEM_7
	}; enum_create_cast_operator(e_key_id, -);

	//std::function<void()>
	//callbacks for up, down and released, pressed called once, the only difference - up and down called in wnd_proc, released and pressed called in main loop.
	enum class e_key_state : std::uint32_t {
		up = 1 << 0,
		down = ~up,
		released = 1 << 1,
		pressed = 1 << 2
	}; enum_create_bit_operators(e_key_state, true);

	namespace input {
		struct mouse_data_t {
		private:
			vec2_t old_pos{ };

		public:
			vec2_t wheel{ }, pos{ }, delta_pos{ };

			void move(vec2_t new_pos) {
				pos = new_pos;
				delta_pos = pos - old_pos;
				old_pos = new_pos;
			}
		} inline mouse{ };

		class c_key {
		public:
			struct key_data_t {
				e_key_id id{ };
				std::string name{ };
			} data{ };

		private:
			float down_duration{ -1.f };

		public:
			array_callbacks_t<e_key_state> callbacks{ };

			e_key_state state{ e_key_state::up };
			bool is_up() { return state & e_key_state::up; }
			bool is_down() { return ~(state | e_key_state::down); }
			bool is_released() { return state & e_key_state::released; }
			bool is_pressed() { return state & e_key_state::pressed; }

			c_key() = default;
			c_key(key_data_t _data) : data(_data) { }

			void update_states(utils::win::c_window window) {
				if(~(state | e_key_state::down) && down_duration < 0.f) {
					state |= e_key_state::pressed;
					if(callbacks.have_callbacks(e_key_state::pressed)) callbacks.call<void()>(e_key_state::pressed);
				} else state &= ~e_key_state::pressed;

				if(state & e_key_state::up && down_duration >= 0.f) {
					state |= e_key_state::released;
					if(callbacks.have_callbacks(e_key_state::released)) callbacks.call<void()>(e_key_state::released);
				} else state &= ~e_key_state::released;

				down_duration = ~(state | e_key_state::down) ? (down_duration < 0.0f ? 0.0f : down_duration + window.time_data.delta_time) : -1.0f;
			}
		};

		std::map<e_key_id, c_key> generate_keys() {
			std::map<e_key_id, c_key> result;
			for(auto key : utils::enum_reflection::members<e_key_id>()) {
				std::string name = std::string{ key.second };
				if(name.empty()) continue;

				name.erase(0, name.find("key_") != -1 ? 4 : 0); //remove "key" for numbers
				std::replace(name.begin(), name.end(), '_', ' '); //remove "_"

				result[key.first] = { { key.first, name } };
			}
			return result;
		}

		inline std::map<e_key_id, c_key> keys = generate_keys();

		bool wnd_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
			std::function<void(e_key_id, bool)> key_processing = [](e_key_id key_id, bool is_up) {
				c_key& key = keys[key_id];
				if(is_up) {
					key.state |= e_key_state::up;
					if(key.callbacks.have_callbacks(e_key_state::up)) key.callbacks.call<void()>(e_key_state::up);
				} else {
					key.state &= e_key_state::down;
					if(key.callbacks.have_callbacks(e_key_state::down)) key.callbacks.call<void()>(e_key_state::down);
				}
			};

			switch(msg) {
				case WM_MOUSEMOVE: {
					mouse.move(vec2_t{ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
				} return true;

				case WM_LBUTTONDBLCLK:
				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP: {
					key_processing(e_key_id::mouse_left, msg == WM_LBUTTONUP);
				} return true;

				case WM_RBUTTONDBLCLK:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP: {
					key_processing(e_key_id::mouse_right, msg == WM_RBUTTONUP);
				} return true;

				case WM_MBUTTONDBLCLK:
				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP: {
					key_processing(e_key_id::mouse_middle, msg == WM_MBUTTONUP);
				} return true;

				case WM_XBUTTONDBLCLK:
				case WM_XBUTTONDOWN:
				case WM_XBUTTONUP: {
					key_processing(e_key_id(-e_key_id::mouse_middle + GET_XBUTTON_WPARAM(w_param)), msg == WM_XBUTTONUP);
				} return true;

				case WM_MOUSEWHEEL: { mouse.wheel.y += (float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA; } return true;
				case WM_MOUSEHWHEEL: { mouse.wheel.x += (float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA; } return true;

				case WM_KEYDOWN:
				case WM_SYSKEYDOWN: {
					key_processing(e_key_id(w_param), false);
				} return true;

				case WM_KEYUP:
				case WM_SYSKEYUP: {
					key_processing(e_key_id(w_param), true);
				} return true;
			}
			return false;
		}

		void begin_frame(utils::win::c_window window) {
			std::for_each(keys.begin(), keys.end(), [&](auto& item) { item.second.update_states(window); });
		}
	}
}