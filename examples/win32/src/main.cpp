#include <iostream>
#include <null-input.h>

utils::win::c_window window{ };
null::input::c_key& mouse_left{ null::input::keys[null::input::e_key_id::mouse_left] };

void main_loop() {
    null::input::begin_frame(window);

    if(mouse_left.is_down())
        std::cout << "mouse_left down" << std::endl;

    //@note: null::input::keys[null::input::e_key_id::mouse_left].is_down() && null::input::keys[null::input::e_key_id::mouse_right].is_down()
    if(null::input::keys_view_t{ null::input::e_key_id::mouse_left, null::input::e_key_id::mouse_right }.is_down())
        std::cout << "mouse_left and mouse_right down" << std::endl;
}

int main(HINSTANCE instance) {
    window = utils::win::c_window{ instance };

    null::input::c_bind{ { null::input::e_key_id::a, null::input::e_key_id::alt } }
        .add_callback(null::input::e_key_state::down, [] { std::cout << "bind a + alt" << std::endl; })
        .add();

    null::input::c_bind{ { null::input::e_key_id::a, null::input::e_key_id::alt, null::input::e_key_id::c } }
        .add_callback(null::input::e_key_state::up, [] { std::cout << "bind a + alt + c" << std::endl; })
        .add();

    mouse_left.callbacks.add<void()>(null::input::e_key_state::released, []() { std::cout << "mouse_left released" << std::endl; });
    mouse_left.callbacks.add<void()>(null::input::e_key_state::down, []() { std::cout << "mouse_left down" << std::endl; });

    window.callbacks.add<void()>(utils::win::e_window_callbacks::on_main_loop, main_loop);
    window.callbacks.add<int(HWND, UINT, WPARAM, LPARAM)>(utils::win::e_window_callbacks::on_wnd_proc, null::input::wnd_proc);

    try {
        window.create();
        window.main_loop();
        window.destroy();
    } catch(const std::exception& exception) {
        std::cout << exception.what() << std::endl;
    }

    return 0;
}