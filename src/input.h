#pragma once

#include <functional>

namespace MR {

class Input {
public:
    static Input& get_instance() {
        static Input instance;
        return instance;
    }

    std::function<void(double, double)> on_mouse_move = [&](double xpos, double ypos) {
        mouse_moved_ = true;
        mouse_x_pos_ = xpos;
        mouse_y_pos_ = ypos;
    };

private:
    Input() = default;
    ~Input() = default;

    Input(const Input&) = delete;             // 禁用拷贝构造函数
    Input& operator=(const Input&) = delete;  // 禁止拷贝赋值运算符

    bool mouse_moved_ = false;
    double mouse_x_pos_, mouse_y_pos_;
};

}  // namespace MR