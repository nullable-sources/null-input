module;
#include <windows.h>
#include <iostream>
#include <unordered_map>
#include <map>
#include <chrono>
export module example;

import null.sdk;
import null.input;

export utils::win::c_window window{ };
export utils::c_segment_time_measurement time_measurement{ };
export null::input::c_key& mouse_left{ null::input::keys[null::input::e_key_id::mouse_left] };

export class listener_t : public null::input::i_event_listener {
public:
    listener_t() {
        null::input::event_dispatcher.attach_listener(null::input::e_event_type::key_down, this);
        null::input::event_dispatcher.attach_listener(null::input::e_event_type::key_up, this);
    }

    ~listener_t() {
        null::input::event_dispatcher.detach_listener(null::input::e_event_type::key_down, this);
        null::input::event_dispatcher.detach_listener(null::input::e_event_type::key_up, this);
    }

public:
    void key_down(const null::input::c_key& key) override {
        std::cout << key.data.name << " key down" << std::endl;
    }

    void key_up(const null::input::c_key& key) override {
        std::cout << key.data.name << " key up" << std::endl;
    }
} listener{ };

export void main_loop() {
    time_measurement.update();
    null::input::begin_frame(time_measurement);

    if(mouse_left.is_down())
        std::cout << "mouse_left down" << std::endl;

    //@note: null::input::keys[null::input::e_key_id::mouse_left].is_down() && null::input::keys[null::input::e_key_id::mouse_right].is_down()
    if(null::input::keys_view_t{ null::input::e_key_id::mouse_left, null::input::e_key_id::mouse_right }.is_down())
        std::cout << "mouse_left and mouse_right down" << std::endl;
}