module;
#include <Windows.h>
#include <windowsx.h>
export module null.input;

import std.core;
import null.sdk;

template<typename ret_t, typename data_t, typename id_t, typename str_t>
constexpr std::pair<id_t, ret_t> create_key_pair(id_t id, str_t key_name) {
	key_name.erase(0, key_name.find("key_") != -1 ? 4 : 0); //remove "key" for numbers
	std::replace(key_name.begin(), key_name.end(), '_', ' '); //remove "_"

	return std::make_pair(id, ret_t{ data_t{ id, key_name } });
}
#define create_key_data(key_id) create_key_pair<c_key, c_key::key_data_t>(e_key_id::key_id, std::string{ #key_id })

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

		inline std::map<e_key_id, c_key> keys = {
			create_key_data(mouse_left), create_key_data(mouse_right),
			create_key_data(mouse_middle),
			create_key_data(mouse_x1), create_key_data(mouse_x2),

			create_key_data(left_shift), create_key_data(right_shift),
			create_key_data(left_ctrl), create_key_data(right_ctrl),
			create_key_data(left_alt), create_key_data(right_alt),
			create_key_data(cancel),
			create_key_data(backspace),
			create_key_data(tab),
			create_key_data(clear),
			create_key_data(enter),
			create_key_data(pause),
			create_key_data(caps_lock),
			create_key_data(escape),
			create_key_data(space),
			create_key_data(page_up), create_key_data(page_down),
			create_key_data(end),
			create_key_data(home),
			create_key_data(left), create_key_data(down),
			create_key_data(up), create_key_data(right),
			create_key_data(print_screen),
			create_key_data(insert),
			create_key_data(del),

			create_key_data(key_0), create_key_data(key_1), create_key_data(key_2), create_key_data(key_3), create_key_data(key_4),
			create_key_data(key_5), create_key_data(key_6), create_key_data(key_7), create_key_data(key_8), create_key_data(key_9),

			create_key_data(a), create_key_data(b), create_key_data(c), create_key_data(d), create_key_data(e),
			create_key_data(f), create_key_data(g), create_key_data(h), create_key_data(i), create_key_data(j),
			create_key_data(k), create_key_data(l), create_key_data(m), create_key_data(n), create_key_data(o),
			create_key_data(p), create_key_data(q), create_key_data(r), create_key_data(s), create_key_data(t),
			create_key_data(u), create_key_data(v), create_key_data(w), create_key_data(x), create_key_data(y),
			create_key_data(z),

			create_key_data(win),
			create_key_data(app),

			create_key_data(num_lock),
			create_key_data(num_0), create_key_data(num_1), create_key_data(num_2), create_key_data(num_3), create_key_data(num_4),
			create_key_data(num_5), create_key_data(num_6), create_key_data(num_7), create_key_data(num_8), create_key_data(num_9),
			create_key_data(num_multiply), create_key_data(num_add), create_key_data(num_subtract), create_key_data(num_decimal), create_key_data(num_divide),

			create_key_data(f1), create_key_data(f2), create_key_data(f3), create_key_data(f4), create_key_data(f5),
			create_key_data(f6), create_key_data(f7), create_key_data(f8), create_key_data(f9), create_key_data(f10),
			create_key_data(f11), create_key_data(f12),

			create_key_data(scroll_lock),

			create_key_data(oem_plus), create_key_data(oem_comma), create_key_data(oem_minus), create_key_data(oem_period),
			create_key_data(oem_1), create_key_data(oem_2), create_key_data(oem_3), create_key_data(oem_4), create_key_data(oem_5), create_key_data(oem_6), create_key_data(oem_7)
		};

		bool wnd_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
			std::function<void(e_key_id, bool)> mouse_processing = [](e_key_id key_id, bool is_up) {
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
					mouse_processing(e_key_id::mouse_left, msg == WM_LBUTTONUP);
				} return true;

				case WM_RBUTTONDBLCLK:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP: {
					mouse_processing(e_key_id::mouse_right, msg == WM_RBUTTONUP);
				} return true;

				case WM_MBUTTONDBLCLK:
				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP: {
					mouse_processing(e_key_id::mouse_middle, msg == WM_MBUTTONUP);
				} return true;

				case WM_XBUTTONDBLCLK:
				case WM_XBUTTONDOWN:
				case WM_XBUTTONUP: {
					mouse_processing(e_key_id(-e_key_id::mouse_middle + GET_XBUTTON_WPARAM(w_param)), msg == WM_XBUTTONUP);
				} return true;

				case WM_MOUSEWHEEL: { mouse.wheel.y += (float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA; } return true;
				case WM_MOUSEHWHEEL: { mouse.wheel.x += (float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA; } return true;
			}
			return false;
		}

		void begin_frame(utils::win::c_window window) {
			std::for_each(keys.begin(), keys.end(), [&](auto& item) { item.second.update_states(window); });
		}
	}
}