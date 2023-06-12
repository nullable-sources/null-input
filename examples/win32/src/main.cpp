import null.sdk;
import null.input;

//@note: we need an example proxy file for the correct order of initialization of variables.
//       For msvc, 3 years after the adoption of the 20th standard, the compiler is not able to cope with the code generation of modules,
//       as a result of which we first initialize the variables of our example, and only then the variables in the library, which can cause a crash
import example;

#include <windows.h>
#include <iostream>

        null::input::event_dispatcher.detach_listener(null::input::e_event_type::key_down, this);
    time_measurement.begin();
        null::input::event_dispatcher.detach_listener(null::input::e_event_type::key_up, this);
    }
    mouse_left.callbacks.at<null::input::e_key_callbacks::on_released>().add([] { std::cout << "mouse_left released" << std::endl; });
    mouse_left.callbacks.at<null::input::e_key_callbacks::on_down>().add([] { std::cout << "mouse_left down" << std::endl; });
public:
    window.callbacks.at<utils::win::e_window_callbacks::on_main_loop>().add(main_loop);
    window.callbacks.at<utils::win::e_window_callbacks::on_wnd_proc>().add(null::input::wnd_proc);
        std::cout << key.data.name << " key down" << std::endl;
    }

        window.main_loop();
        std::cout << key.data.name << " key up" << std::endl;
    } catch(const std::exception& exception) {
        std::cout << exception.what() << std::endl;

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
    window = utils::win::c_window{ instance };

    mouse_left.callbacks.add<void()>(null::e_key_state::released, []() { std::cout << "mouse_left released" << std::endl; });

    window.callbacks.set<bool(HWND, UINT, WPARAM, LPARAM)>(utils::e_window_callbacks::wnd_proc, null::input::wnd_proc);

    try {
        window.create();
        window.main_loop(main_loop);
        window.destroy();
    } catch(std::exception exp) {
        std::cout << exp.what() << std::endl;
    }
}