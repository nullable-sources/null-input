#include <iostream>
#include <null-input.h>

utils::win::c_window window{ };
utils::c_segment_time_measurement time_measurement{ };
null::input::c_key& mouse_left{ null::input::keys[null::input::e_key_id::mouse_left] };

class listener_t : public null::input::i_event_listener {
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

void main_loop() {
    time_measurement.update();
    null::input::begin_frame(time_measurement);

    if(mouse_left.is_down())
        std::cout << "mouse_left down" << std::endl;

    //@note: null::input::keys[null::input::e_key_id::mouse_left].is_down() && null::input::keys[null::input::e_key_id::mouse_right].is_down()
    if(null::input::keys_view_t{ null::input::e_key_id::mouse_left, null::input::e_key_id::mouse_right }.is_down())
        std::cout << "mouse_left and mouse_right down" << std::endl;
}

int main(HINSTANCE instance) {
    time_measurement.begin();
    window = utils::win::c_window{ instance };

    mouse_left.callbacks.at<null::input::e_key_callbacks::on_released>().add([] { std::cout << "mouse_left released" << std::endl; });
    mouse_left.callbacks.at<null::input::e_key_callbacks::on_down>().add([] { std::cout << "mouse_left down" << std::endl; });

    window.callbacks.at<utils::win::e_window_callbacks::on_main_loop>().add(main_loop);
    window.callbacks.at<utils::win::e_window_callbacks::on_wnd_proc>().add(null::input::wnd_proc);

    try {
        window.create();
        window.main_loop();
        window.destroy();
    } catch(const std::exception& exception) {
        std::cout << exception.what() << std::endl;
    }

    return 0;
}