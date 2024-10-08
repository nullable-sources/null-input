#pragma once
#include <Windows.h>
#include <map>
#include <null-sdk.h>

namespace ntl::input {
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
    }; NULLSDK_ENUM_CREATE_CAST_OPERATOR(e_key_id, -);

    //@note: void()
    //       callbacks for up, down and released, pressed called once, the only difference - up and down called in wnd_proc, released and pressed called in main loop.
    enum class e_key_state : std::uint32_t {
        up = 1 << 0,
        down = ~up,
        released = 1 << 1,
        pressed = 1 << 2
    }; NULLSDK_ENUM_CREATE_BIT_OPERATORS(e_key_state, true);

    enum class e_key_callbacks {
        on_up,
        on_down,
        on_released,
        on_pressed
    };
    using key_state_callbacks_t = callbacks_tuple_t<
        callbacks_t<e_key_callbacks::on_up, void()>,
        callbacks_t<e_key_callbacks::on_down, void()>,
        callbacks_t<e_key_callbacks::on_released, void()>,
        callbacks_t<e_key_callbacks::on_pressed, void()>
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
    } inline mouse{ };

    class c_key {
    public:
        struct key_data_t {
        public:
            const e_key_id id{ };
            const std::string name{ };

        public:
            std::string formated_name() const {
                std::string result = name;
                if(result.starts_with("key_"))
                    result = result.substr(4);

                std::ranges::replace(result, '_', ' ');
                return result;
            }
        } const data{ };

    private:
        float down_duration{ -1.f };

    public:
        key_state_callbacks_t callbacks{ };

        e_key_state state{ e_key_state::up };

    public:
        c_key() { }
        c_key(const key_data_t& _data) : data(_data) { }

    public:
        bool check_state(const e_key_state& _state) const { return _state == e_key_state::down ? is_down() : (state & _state); }
        bool is_up() const { return state & e_key_state::up; }
        bool is_down() const { return ~(state | e_key_state::down); }
        bool is_released() const { return state & e_key_state::released; }
        bool is_pressed() const { return state & e_key_state::pressed; }

        void update_states(const c_segment_time_measurement& time_measurement);
    };

#define NULLINPUT_CREATE_KEY(key_id) { e_key_id::key_id, { { e_key_id::key_id, #key_id } } } //@note: reflection takes too long during compilation (~15s)
    inline std::map<e_key_id, c_key> keys = {
        NULLINPUT_CREATE_KEY(mouse_left), NULLINPUT_CREATE_KEY(mouse_right),
        NULLINPUT_CREATE_KEY(mouse_middle),
        NULLINPUT_CREATE_KEY(mouse_x1), NULLINPUT_CREATE_KEY(mouse_x2),

        NULLINPUT_CREATE_KEY(shift),
        NULLINPUT_CREATE_KEY(ctrl),
        NULLINPUT_CREATE_KEY(alt),

        NULLINPUT_CREATE_KEY(cancel),
        NULLINPUT_CREATE_KEY(backspace),
        NULLINPUT_CREATE_KEY(tab),
        NULLINPUT_CREATE_KEY(clear),
        NULLINPUT_CREATE_KEY(enter),

        NULLINPUT_CREATE_KEY(pause),
        NULLINPUT_CREATE_KEY(caps_lock),
        NULLINPUT_CREATE_KEY(escape),
        NULLINPUT_CREATE_KEY(space),
        NULLINPUT_CREATE_KEY(page_up),
        NULLINPUT_CREATE_KEY(page_down),
        NULLINPUT_CREATE_KEY(end),
        NULLINPUT_CREATE_KEY(home),
        NULLINPUT_CREATE_KEY(left),
        NULLINPUT_CREATE_KEY(up),
        NULLINPUT_CREATE_KEY(right),
        NULLINPUT_CREATE_KEY(down),
        NULLINPUT_CREATE_KEY(print_screen),
        NULLINPUT_CREATE_KEY(insert),
        NULLINPUT_CREATE_KEY(del),

        NULLINPUT_CREATE_KEY(key_0), NULLINPUT_CREATE_KEY(key_1), NULLINPUT_CREATE_KEY(key_2), NULLINPUT_CREATE_KEY(key_3), NULLINPUT_CREATE_KEY(key_4),
        NULLINPUT_CREATE_KEY(key_5), NULLINPUT_CREATE_KEY(key_6), NULLINPUT_CREATE_KEY(key_7), NULLINPUT_CREATE_KEY(key_8), NULLINPUT_CREATE_KEY(key_9),

        NULLINPUT_CREATE_KEY(a), NULLINPUT_CREATE_KEY(b), NULLINPUT_CREATE_KEY(c), NULLINPUT_CREATE_KEY(d), NULLINPUT_CREATE_KEY(e),
        NULLINPUT_CREATE_KEY(f), NULLINPUT_CREATE_KEY(g), NULLINPUT_CREATE_KEY(h), NULLINPUT_CREATE_KEY(i), NULLINPUT_CREATE_KEY(j),
        NULLINPUT_CREATE_KEY(k), NULLINPUT_CREATE_KEY(l), NULLINPUT_CREATE_KEY(m), NULLINPUT_CREATE_KEY(n), NULLINPUT_CREATE_KEY(o),
        NULLINPUT_CREATE_KEY(p), NULLINPUT_CREATE_KEY(q), NULLINPUT_CREATE_KEY(r), NULLINPUT_CREATE_KEY(s), NULLINPUT_CREATE_KEY(t),
        NULLINPUT_CREATE_KEY(u), NULLINPUT_CREATE_KEY(v), NULLINPUT_CREATE_KEY(w), NULLINPUT_CREATE_KEY(x), NULLINPUT_CREATE_KEY(y),
        NULLINPUT_CREATE_KEY(z),

        NULLINPUT_CREATE_KEY(win),
        NULLINPUT_CREATE_KEY(app),

        NULLINPUT_CREATE_KEY(num_lock),
        NULLINPUT_CREATE_KEY(num_0), NULLINPUT_CREATE_KEY(num_1), NULLINPUT_CREATE_KEY(num_2), NULLINPUT_CREATE_KEY(num_3), NULLINPUT_CREATE_KEY(num_4),
        NULLINPUT_CREATE_KEY(num_5), NULLINPUT_CREATE_KEY(num_6), NULLINPUT_CREATE_KEY(num_7), NULLINPUT_CREATE_KEY(num_8), NULLINPUT_CREATE_KEY(num_9),
        NULLINPUT_CREATE_KEY(num_multiply), NULLINPUT_CREATE_KEY(num_add), NULLINPUT_CREATE_KEY(num_subtract), NULLINPUT_CREATE_KEY(num_decimal), NULLINPUT_CREATE_KEY(num_divide),

        NULLINPUT_CREATE_KEY(f1), NULLINPUT_CREATE_KEY(f2), NULLINPUT_CREATE_KEY(f3), NULLINPUT_CREATE_KEY(f4), NULLINPUT_CREATE_KEY(f5),
        NULLINPUT_CREATE_KEY(f6), NULLINPUT_CREATE_KEY(f7), NULLINPUT_CREATE_KEY(f8), NULLINPUT_CREATE_KEY(f9), NULLINPUT_CREATE_KEY(f10),
        NULLINPUT_CREATE_KEY(f11), NULLINPUT_CREATE_KEY(f12),

        NULLINPUT_CREATE_KEY(scroll_lock),

        NULLINPUT_CREATE_KEY(oem_plus), NULLINPUT_CREATE_KEY(oem_comma), NULLINPUT_CREATE_KEY(oem_minus), NULLINPUT_CREATE_KEY(oem_period),
        NULLINPUT_CREATE_KEY(oem_1), NULLINPUT_CREATE_KEY(oem_2), NULLINPUT_CREATE_KEY(oem_3), NULLINPUT_CREATE_KEY(oem_4), NULLINPUT_CREATE_KEY(oem_5), NULLINPUT_CREATE_KEY(oem_6), NULLINPUT_CREATE_KEY(oem_7)
    };
#undef NULLINPUT_CREATE_KEY

    struct keys_view_t {
    public:
        std::vector<e_key_id> ids{ };

    public:
        keys_view_t() { }
        keys_view_t(const std::vector<e_key_id>& _ids) : ids(_ids) { }
        keys_view_t(const std::initializer_list<e_key_id>& _ids) : ids(_ids) { }

    public:
        bool check_state(e_key_state state) const { return std::ranges::all_of(ids, [&](e_key_id id) { return keys[id].check_state(state); }); }
        bool is_up() const { return check_state(e_key_state::up); }
        bool is_down() const { return check_state(e_key_state::down); }
        bool is_released() const { return check_state(e_key_state::released); }
        bool is_pressed() const { return check_state(e_key_state::pressed); }

        std::vector<e_key_id>::iterator begin() { return ids.begin(); }
        std::vector<e_key_id>::const_iterator begin() const { return ids.begin(); }
        std::vector<e_key_id>::iterator end() { return ids.end(); }
        std::vector<e_key_id>::const_iterator end() const { return ids.end(); }

        const e_key_id& front() const { return ids.front(); }
        const e_key_id& back() const { return ids.back(); }

        bool contains(e_key_id key_id) const { return std::ranges::find(ids, key_id) != end(); }
        void remove(e_key_id key_id) { std::erase(ids, key_id); }
        void add(e_key_id key_id) { ids.push_back(key_id); }
        void clear() { ids.clear(); }
        bool empty() const { return ids.empty() || (ids.size() == 1 && ids[0] == e_key_id::none); }
        size_t size() const { return ids.size(); }

        bool include(const keys_view_t& keys_view) const {
            if(keys_view.empty() || keys_view.size() > size()) return false;
            return std::ranges::equal(std::ranges::subrange(begin(), std::next(begin(), keys_view.size())), keys_view.ids);
        }

        keys_view_t subview(size_t offset, size_t count = size_t(-1)) const {
            size_t start_idx = std::min(offset, size());
            size_t end_idx = std::clamp(offset + std::min(count, size()), start_idx, size());
            return std::vector(std::next(begin(), start_idx), std::next(begin(), end_idx));
        }

    public:
        bool operator==(const keys_view_t&) const = default;
    };

    enum class e_event_type { key_down, key_up };
    class c_key_event_dispatcher : public c_event_dispatcher<e_event_type> {
    public:
        void key_down(const c_key& key, bool repeated) { dispatch_event(e_event_type::key_down, { { "key", key }, { "repeated", repeated } }); }
        void key_up(const c_key& key) { dispatch_event(e_event_type::key_up, { { "key", key } }); }
    } inline key_event_dispatcher{ };

    class i_key_event_listener : public i_event_listener<e_event_type> {
    public:
        i_key_event_listener() { }
        virtual ~i_key_event_listener() { }

    private:
        void process_event(e_event_type id, const event_parameters_t& parameters) override {
            switch(id) {
                case e_event_type::key_down: { key_down(std::any_cast<const c_key&>(parameters.at("key")), std::any_cast<bool>(parameters.at("repeated"))); } break;
                case e_event_type::key_up: { key_up(std::any_cast<const c_key&>(parameters.at("key"))); } break;
            }
        }

    public:
        virtual void key_down(const c_key& key, bool repeated) { }
        virtual void key_up(const c_key& key) { }
    };

    void begin_frame(const c_segment_time_measurement& time_measurement);
    int wnd_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param);
}