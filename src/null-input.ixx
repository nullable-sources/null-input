module;
#include <Windows.h>
#include <windowsx.h>
#include <unordered_map>
#include <map>
#include <any>
#include <ranges>
#include <algorithm>
#include <chrono>

#include <utils/fast_operators.h>
export module null.input;

import null.sdk;

export namespace null::input {
	enum class e_key_id {
		none,

		mouse_left = VK_LBUTTON, mouse_right = VK_RBUTTON,
		mouse_middle = VK_MBUTTON,
		mouse_x1 = VK_XBUTTON1, mouse_x2 = VK_XBUTTON2,

		shift = VK_SHIFT,
		ctrl = VK_CONTROL,
		alt = VK_MENU,

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

	//@note:	void()
	//			callbacks for up, down and released, pressed called once, the only difference - up and down called in wnd_proc, released and pressed called in main loop.
	enum class e_key_state : std::uint32_t {
		up = 1 << 0,
		down = ~up,
		released = 1 << 1,
		pressed = 1 << 2
	}; enum_create_bit_operators(e_key_state, true);

	enum class e_key_callbacks {
		on_up,
		on_down,
		on_released,
		on_pressed
	};
	using key_state_callbacks_t = utils::callbacks_tuple_t<
		utils::callbacks_t<e_key_callbacks::on_up, void()>,
		utils::callbacks_t<e_key_callbacks::on_down, void()>,
		utils::callbacks_t<e_key_callbacks::on_released, void()>,
		utils::callbacks_t<e_key_callbacks::on_pressed, void()>
	>;

	struct mouse_data_t {
	public:
		vec2_t<int> pos{ }, delta_pos{ };
		vec2_t<float> wheel{ };

	public:
		void move(const vec2_t<int>& new_pos) {
			delta_pos = new_pos - pos;
			pos = new_pos;
		}
	} mouse{ };

	class c_key {
	public:
		struct key_data_t {
		public:
			const e_key_id id{ };
			const std::string_view name{ };
		} const data{ };

	private:
		float down_duration{ -1.f };

	public:
		key_state_callbacks_t callbacks{ };

		e_key_state state{ e_key_state::up };

	public:
		c_key() { }
		c_key(const key_data_t& _data) : data{ _data } { }

	public:
		bool check_state(e_key_state _state) const { return _state == e_key_state::down ? is_down() : (state & _state); }
		bool is_up() const { return state & e_key_state::up; }
		bool is_down() const { return ~(state | e_key_state::down); }
		bool is_released() const { return state & e_key_state::released; }
		bool is_pressed() const { return state & e_key_state::pressed; }

		void update_states(const utils::c_segment_time_measurement& time_measurement) {
			if(~(state | e_key_state::down) && down_duration < 0.f) {
				state |= e_key_state::pressed;
				callbacks.at<e_key_callbacks::on_pressed>().call();
			} else state &= ~e_key_state::pressed;

			if(state & e_key_state::up && down_duration >= 0.f) {
				state |= e_key_state::released;
				callbacks.at<e_key_callbacks::on_released>().call();
			} else state &= ~e_key_state::released;

			down_duration = ~(state | e_key_state::down) ? (down_duration < 0.f ? 0.f : down_duration + std::chrono::duration<float>{ time_measurement.representation() }.count()) : -1.f;
		}
	};

#define create_key(key_id) { e_key_id::key_id, { { e_key_id::key_id, #key_id } } } //@note: reflection takes too long during compilation (~15s)
	std::unordered_map<e_key_id, c_key> keys{
		create_key(mouse_left), create_key(mouse_right),
		create_key(mouse_middle),
		create_key(mouse_x1), create_key(mouse_x2),

		create_key(shift),
		create_key(ctrl),
		create_key(alt),

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

	struct keys_view_t {
	public:
		std::vector<e_key_id> ids{ };

	public:
		keys_view_t() { }
		keys_view_t(const std::initializer_list<e_key_id>& _ids) : ids{ _ids } { }

	public:
		bool check_state(e_key_state state) const { return std::ranges::all_of(ids, [&](const e_key_id& id) { return keys[id].check_state(state); }); }
		bool is_up() const { return check_state(e_key_state::up); }
		bool is_down() const { return check_state(e_key_state::down); }
		bool is_released() const { return check_state(e_key_state::released); }
		bool is_pressed() const { return check_state(e_key_state::pressed); }

	public:
		bool operator==(const keys_view_t&) const = default;
	};

	enum class e_event_type { key_down, key_up };
	class c_event_dispatcher : public utils::c_event_dispatcher<e_event_type> {
	public:
		void key_down(const c_key& key) { dispatch_event(e_event_type::key_down, { { "key", key } }); }
		void key_up(const c_key& key) { dispatch_event(e_event_type::key_up, { { "key", key } }); }
	} event_dispatcher{ };

	class i_event_listener : public utils::i_event_listener<e_event_type> {
	public:
		i_event_listener() { }
		virtual ~i_event_listener() { }

	private:
		void process_event(e_event_type id, const std::unordered_map<std::string, std::any>& parameters) override {
			switch(id) {
				case e_event_type::key_down: { key_down(std::any_cast<const c_key&>(parameters.at("key"))); } break;
				case e_event_type::key_up: { key_up(std::any_cast<const c_key&>(parameters.at("key"))); } break;
			}
		}

	public:
		virtual void key_down(const c_key& key) { }
		virtual void key_up(const c_key& key) { }
	};

	void begin_frame(const utils::c_segment_time_measurement& time_measurement) {
		std::ranges::for_each(keys, [&](std::pair<e_key_id, c_key&> key) { key.second.update_states(time_measurement); });
	}

	int wnd_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
		const static std::function<void(e_key_id, bool)> key_processing{ [](e_key_id key_id, bool is_up) {
			static keys_view_t current_pressed_keys{ };

			c_key& key{ keys[key_id] };
			e_key_state state{ is_up ? e_key_state::up : e_key_state::down };

			if(is_up) {
				key.state |= e_key_state::up;
				key.callbacks.at<e_key_callbacks::on_up>().call();
				event_dispatcher.key_up(key);
			} else {
				key.state &= e_key_state::down;
				key.callbacks.at<e_key_callbacks::on_down>().call();
				event_dispatcher.key_down(key);
			}
		} };

		switch(msg) {
			case WM_MOUSEMOVE: {
				mouse.move({ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			} break;

			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP: {
				key_processing(e_key_id::mouse_left, msg == WM_LBUTTONUP);
			} break;

			case WM_RBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP: {
				key_processing(e_key_id::mouse_right, msg == WM_RBUTTONUP);
			} break;

			case WM_MBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP: {
				key_processing(e_key_id::mouse_middle, msg == WM_MBUTTONUP);
			} break;

			case WM_XBUTTONDBLCLK:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP: {
				key_processing((e_key_id)(-e_key_id::mouse_middle + GET_XBUTTON_WPARAM(w_param)), msg == WM_XBUTTONUP);
			} break;

			case WM_MOUSEWHEEL: { mouse.wheel.y = (float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA; } return true;
			case WM_MOUSEHWHEEL: { mouse.wheel.x = (float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA; } return true;

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN: {
				key_processing((e_key_id)w_param, false);
			} break;

			case WM_KEYUP:
			case WM_SYSKEYUP: {
				key_processing((e_key_id)w_param, true);
			} break;

			default: return -1;
		}
		return 1;
	}
}