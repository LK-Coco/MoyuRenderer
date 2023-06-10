#pragma once

namespace MR {

class Color {
public:
    Color(float red, float green, float blue, float alpha = 1)
        : r(red),
          g(green),
          b(blue),
          a(alpha) {}

    int operator[](int i) const {
        switch (i) {
            case 0: return r;
            case 1: return g;
            case 2: return b;
            case 3: return a;
        };
        throw "out of range";
    }

    float r, g, b, a;

    static Color black;
    static Color blue;
    static Color clear;
    static Color cyan;
    static Color gray;
    static Color green;
    static Color grey;
    static Color magenta;
    static Color red;
    static Color white;
    static Color yellow;
};

}  // namespace MR