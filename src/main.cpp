#include <iostream>
#include "renderer_view.h"

using namespace std;

int main(int argc, char** argv) {
    MR::RendererView viewer(1280, 720);
    viewer.run();
}
