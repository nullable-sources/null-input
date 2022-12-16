#include <null-input.h>

namespace null::input {
	void c_key::update_states(const utils::win::c_window& window) {
		if(~(state | e_key_state::down) && down_duration < 0.f) {
			state |= e_key_state::pressed;
			callbacks.call(e_key_state::pressed);
		} else state &= ~e_key_state::pressed;

		if(state & e_key_state::up && down_duration >= 0.f) {
			state |= e_key_state::released;
			callbacks.call(e_key_state::released);
		} else state &= ~e_key_state::released;

		down_duration = ~(state | e_key_state::down) ? (down_duration < 0.f ? 0.f : down_duration + window.time_data.delta_time) : -1.f;
	}

	void begin_frame(const utils::win::c_window& window) {
		std::ranges::for_each(keys, [&](std::pair<e_key_id, c_key&> key) { key.second.update_states(window); });
	}

	int wnd_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
		const static std::function<void(e_key_id, bool)> key_processing{ [](e_key_id key_id, bool is_up) {
			static keys_view_t current_pressed_keys{ };
			
			c_key& key{ keys[key_id] };
			e_key_state state{ is_up ? e_key_state::up : e_key_state::down };
			
			if(is_up) key.state |= state;
			else {
				key.state &= state;

				if(auto finded{ std::ranges::find(current_pressed_keys.ids, key_id) }; finded == current_pressed_keys.ids.end())
					current_pressed_keys.ids.push_back(key_id);
			}
			
			key.callbacks.call(state);
			if(c_bind* finded{ c_bind::find(current_pressed_keys) })
				finded->callbacks.call(state);

			if(is_up) {
				if(auto finded{ std::ranges::find(current_pressed_keys.ids, key_id) }; finded != current_pressed_keys.ids.end())
					current_pressed_keys.ids.erase(finded);
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