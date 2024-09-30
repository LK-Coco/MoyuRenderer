#include "skybox_pass.h"

#include "scene.h"

namespace MR {

SkyboxPass::SkyboxPass()
    : skybox_shader_("assets/shaders/skybox/skybox.vs",
                     "assets/shaders/skybox/skybox.fs", {}) {
    skybox_shader_.use();
    skybox_shader_.set_int("environmentMap", 0);
}

void SkyboxPass::render(const glm::mat4& proj, const glm::mat4& view) {
    skybox_shader_.use();
    skybox_shader_.set_mat4("projection", proj);
    skybox_shader_.set_mat4("view", view);

    cube_map_->bind(0);

    // 关闭面剔除
    glDisable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    cube_.render();

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}

}  // namespace MR