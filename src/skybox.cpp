#include <iostream>

#include "skybox.h"
#include "glad/glad.h"
#include "resources/resources.h"

namespace MR {

void Skybox::set_hdr_cube_map(TextureCube* tex) { cube_map_ = tex; }

void Skybox::draw() {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    cube_map_->bind(0);

    cube_.render();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

}  // namespace MR