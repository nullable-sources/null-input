#include <windowsx.h>
#include "null-input.h"

namespace ntl::input {
    void c_key::update_states(const c_segment_time_measurement& time_measurement) {
        if(~(state | e_key_state::down) && down_duration < 0.f) {
            state |= e_key_state::pressed;
            callbacks.at<e_key_callbacks::on_pressed>().call();
        } else state &= ~e_key_state::pressed;

        if(state & e_key_state::up && down_duration >= 0.f) {
            state |= e_key_state::released;
            callbacks.at<e_key_callbacks::on_released>().call();
        } else state &= ~e_key_state::released;

        down_duration = ~(state | e_key_state::down) ? (down_duration < 0.f ? 0.f : down_duration + std::chrono::duration<float>(time_measurement.representation()).count()) : -1.f;
    }

    void begin_frame(const c_segment_time_measurement& time_measurement) {
        std::ranges::for_each(keys | std::views::values, std::bind(&c_key::update_states, std::placeholders::_1, time_measurement));
    }

    int wnd_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
        const static std::function<void(e_key_id, bool, bool)> key_processing = [](e_key_id key_id, bool is_up, bool repeated) {
            c_key& key = keys[key_id];

            if(is_up) {
                key.state |= e_key_state::up;
                key.callbacks.at<e_key_callbacks::on_up>().call();
                key_event_dispatcher.key_up(key);
            } else {
                key.state &= e_key_state::down;
                key.callbacks.at<e_key_callbacks::on_down>().call();
                key_event_dispatcher.key_down(key, repeated);
            }
        };

        switch(msg) {
            case WM_MOUSEMOVE: {
                mouse.move(vec2_t<int>(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param)));
            } break;

            case WM_LBUTTONDBLCLK:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP: {
                key_processing(e_key_id::mouse_left, msg == WM_LBUTTONUP, false);
            } break;

            case WM_RBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP: {
                key_processing(e_key_id::mouse_right, msg == WM_RBUTTONUP, false);
            } break;

            case WM_MBUTTONDBLCLK:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP: {
                key_processing(e_key_id::mouse_middle, msg == WM_MBUTTONUP, false);
            } break;

            case WM_XBUTTONDBLCLK:
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP: {
                key_processing((e_key_id)(-e_key_id::mouse_middle + GET_XBUTTON_WPARAM(w_param)), msg == WM_XBUTTONUP, false);
            } break;

            case WM_MOUSEWHEEL: { mouse.wheel.y = (float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA; } return true;
            case WM_MOUSEHWHEEL: { mouse.wheel.x = (float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA; } return true;

            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP: {
                key_processing((e_key_id)w_param, (HIWORD(l_param) & KF_UP) == KF_UP, (HIWORD(l_param) & KF_REPEAT) == KF_REPEAT);
            } break;

            case WM_KILLFOCUS: {
                for(auto& [id, key] : keys)
                    if(key.is_down()) key_processing(id, true, false);
            } break;

            default: return -1;
        }
        return 1;
    }
}