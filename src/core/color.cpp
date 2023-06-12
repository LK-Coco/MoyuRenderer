#include "color.h"

namespace MR {

Color::Color() {
    r = 0;
    g = 0;
    b = 0;
    a = 1;
}

Color Color::black = Color(0, 0, 0, 1);
Color Color::blue = Color(0, 0, 1, 1);
Color Color::clear = Color(0, 0, 0, 0);
Color Color::cyan = Color(0, 1, 1, 1);
Color Color::gray = Color(0.5f, 0.5f, 0.5f, 1);
Color Color::green = Color(0, 1, 0, 1);
Color Color::grey = Color(0.5f, 0.5f, 0.5f, 1);
Color Color::magenta = Color(1, 0, 1, 1);
Color Color::red = Color(1, 0, 0, 1);
Color Color::white = Color(1, 1, 1, 1);
Color Color::yellow = Color(1, 0.92f, 0.016f, 1);

}  // namespace MR