#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <null-sdk.h>

namespace null {
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

			void move(const vec2_t& new_pos);
		} inline mouse{ };

		class c_key {
		public:
			struct key_data_t {
				const e_key_id id{ };
				const std::string_view name{ };
			} const data{ };

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
			c_key(const key_data_t& _data) : data(_data) { }

			void update_states(const utils::win::c_window& window);
		};

#define create_key(key_id) { e_key_id::key_id, { { e_key_id::key_id, #key_id } } } //reflection takes too long during compilation (~15s)
		inline std::map<e_key_id, c_key> keys = {
			create_key(mouse_left), create_key(mouse_right),
			create_key(mouse_middle),
			create_key(mouse_x1), create_key(mouse_x2),

			create_key(left_shift), create_key(right_shift),
			create_key(left_ctrl), create_key(right_ctrl),
			create_key(left_alt), create_key(right_alt),

			create_key(cancel),
			create_key(backspace),
			create_key(tab),
			create_key(clear),
			create_key(enter),

			create_key(pause),
			create_key(caps_lock),
			create_key(escape),
			create_key(space),
			create_key(page_up),
			create_key(page_down),
			create_key(end),
			create_key(home),
			create_key(left),
			create_key(up),
			create_key(right),
			create_key(down),
			create_key(print_screen),
			create_key(insert),
			create_key(del),

			create_key(key_0), create_key(key_1), create_key(key_2), create_key(key_3), create_key(key_4),
			create_key(key_5), create_key(key_6), create_key(key_7), create_key(key_8), create_key(key_9),
			
			create_key(a), create_key(b), create_key(c), create_key(d), create_key(e),
			create_key(f), create_key(g), create_key(h), create_key(i), create_key(j),
			create_key(k), create_key(l), create_key(m), create_key(n), create_key(o),
			create_key(p), create_key(q), create_key(r), create_key(s), create_key(t),
			create_key(u), create_key(v), create_key(w), create_key(x), create_key(y),
			create_key(z),

			create_key(win),
			create_key(app),

			create_key(num_lock),
			create_key(num_0), create_key(num_1), create_key(num_2), create_key(num_3), create_key(num_4),
			create_key(num_5), create_key(num_6), create_key(num_7), create_key(num_8), create_key(num_9),
			create_key(num_multiply), create_key(num_add), create_key(num_subtract), create_key(num_decimal), create_key(num_divide),

			create_key(f1), create_key(f2), create_key(f3), create_key(f4), create_key(f5),
			create_key(f6), create_key(f7), create_key(f8), create_key(f9), create_key(f10),
			create_key(f11), create_key(f12),

			create_key(scroll_lock),

			create_key(oem_plus), create_key(oem_comma), create_key(oem_minus), create_key(oem_period),
			create_key(oem_1), create_key(oem_2), create_key(oem_3), create_key(oem_4), create_key(oem_5), create_key(oem_6), create_key(oem_7)
		};
#undef create_key

		void begin_frame(const utils::win::c_window& window);

		int wnd_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
	}
}