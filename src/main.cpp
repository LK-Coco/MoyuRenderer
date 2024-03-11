#include <iostream>
#include "jyu_app/src/application.h"
#include "editor_layer.h"

using namespace std;

int main(int argc, char** argv) {
    Jyu::ApplicationSpec spec{"Render Demo", 1280, 720};
    Jyu::Application app(spec);
    app.push_layer<MR::EditorLayer>();
    app.run();
}
