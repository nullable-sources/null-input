#include "null-input.h"

namespace null::input {
	void mouse_data_t::move(vec2_t new_pos) {
		pos = new_pos;
		delta_pos = pos - old_pos;
		old_pos = new_pos;
	}

	void c_key::update_states(utils::win::c_window window) {
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

	void begin_frame(utils::win::c_window window) {
		std::for_each(keys.begin(), keys.end(), [&](auto& item) { item.second.update_states(window); });
	}

	int wnd_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
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
			case WM_MOUSEMOVE:
			{
				mouse.move(vec2_t{ GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param) });
			} return 1;

			case WM_LBUTTONDBLCLK:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			{
				key_processing(e_key_id::mouse_left, msg == WM_LBUTTONUP);
			} return 1;

			case WM_RBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			{
				key_processing(e_key_id::mouse_right, msg == WM_RBUTTONUP);
			} return 1;

			case WM_MBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			{
				key_processing(e_key_id::mouse_middle, msg == WM_MBUTTONUP);
			} return 1;

			case WM_XBUTTONDBLCLK:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
			{
				key_processing(e_key_id(-e_key_id::mouse_middle + GET_XBUTTON_WPARAM(w_param)), msg == WM_XBUTTONUP);
			} return 1;

			case WM_MOUSEWHEEL: { mouse.wheel.y += (float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA; } return true;
			case WM_MOUSEHWHEEL: { mouse.wheel.x += (float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA; } return true;

			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
			{
				key_processing(e_key_id(w_param), false);
			} return 1;

			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				key_processing(e_key_id(w_param), true);
			} return 1;
		}
		return -1;
	}
}