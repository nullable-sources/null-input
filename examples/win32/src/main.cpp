import null.sdk;
import null.input;

//@note: we need an example proxy file for the correct order of initialization of variables.
//       For msvc, 3 years after the adoption of the 20th standard, the compiler is not able to cope with the code generation of modules,
//       as a result of which we first initialize the variables of our example, and only then the variables in the library, which can cause a crash
import example;

#include <windows.h>
#include <iostream>

int main(HINSTANCE instance) {
    window = utils::win::c_window{ instance };

    mouse_left.callbacks.at<null::input::e_key_callbacks::on_released>().add([] { std::cout << "mouse_left released" << std::endl; });
    mouse_left.callbacks.at<null::input::e_key_callbacks::on_down>().add([] { std::cout << "mouse_left down" << std::endl; });

    window.callbacks.at<utils::win::e_window_callbacks::on_main_loop>().add(main_loop);
    window.callbacks.at<utils::win::e_window_callbacks::on_wnd_proc>().add(null::input::wnd_proc);

    try {
        window.create();
        window.main_loop(main_loop);
        window.destroy();
    } catch(std::exception exp) {
        std::cout << exp.what() << std::endl;
    }
}