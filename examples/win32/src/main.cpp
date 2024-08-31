#include <iostream>
#include <null-input.h>

ntl::win::c_window window{ };
ntl::c_segment_time_measurement time_measurement{ };
ntl::input::c_key& mouse_left{ ntl::input::keys[ntl::input::e_key_id::mouse_left] };

class listener_t : public ntl::input::i_key_event_listener {
public:
    listener_t() {
        ntl::input::key_event_dispatcher.attach_listener(ntl::input::e_event_type::key_down, this);
        ntl::input::key_event_dispatcher.attach_listener(ntl::input::e_event_type::key_up, this);
    }

    ~listener_t() {
        ntl::input::key_event_dispatcher.detach_listener(ntl::input::e_event_type::key_down, this);
        ntl::input::key_event_dispatcher.detach_listener(ntl::input::e_event_type::key_up, this);
    }

public:
    void key_down(const ntl::input::c_key& key, bool repeated) override {
        std::cout << "event listener: " << key.data.name << " key down (" << repeated << ")" << std::endl;
    }

    void key_up(const ntl::input::c_key& key) override {
        std::cout << "event listener: " << key.data.name << " key up" << std::endl;
    }
} listener{ };

void main_loop() {
    time_measurement.update();
    ntl::input::begin_frame(time_measurement);

    if(mouse_left.is_down())
        std::cout << "mouse_left down" << std::endl;

    //@note: ntl::input::keys[ntl::input::e_key_id::mouse_left].is_down() && ntl::input::keys[ntl::input::e_key_id::mouse_right].is_down()
    if(ntl::input::keys_view_t{ ntl::input::e_key_id::mouse_left, ntl::input::e_key_id::mouse_right }.is_down())
        std::cout << "mouse_left and mouse_right down" << std::endl;
}

int main(HINSTANCE instance) {
    time_measurement.begin();
    window = ntl::win::c_window(instance);

    mouse_left.callbacks.at<ntl::input::e_key_callbacks::on_released>().add([] { std::cout << "mouse_left released" << std::endl; });
    mouse_left.callbacks.at<ntl::input::e_key_callbacks::on_down>().add([] { std::cout << "mouse_left down" << std::endl; });

    window.callbacks.at<ntl::win::e_window_callbacks::on_main_loop>().add(main_loop);
    window.callbacks.at<ntl::win::e_window_callbacks::on_wnd_proc>().add(ntl::input::wnd_proc);

    try {
        window.create();
        window.main_loop();
        window.destroy();
    } catch(const std::exception& exception) {
        std::cout << exception.what() << std::endl;
    }

    return 0;
}