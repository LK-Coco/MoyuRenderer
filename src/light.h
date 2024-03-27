#pragma once

#include "glm/fwd.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace MR {

struct BaseLight {
    glm::vec3 color = glm::vec3(1.0f);
    glm::mat4 shadow_projection_mat = glm::mat4(0.0);

    bool changed = false;

    float strength = 1.0f;
    float z_near = 1.0f;
    float z_far = 2000.0f;

    unsigned int shadow_res = 1024;
    unsigned int depth_map_tex_id = 0;
};

struct PointLight : public BaseLight {
    PointLight(glm::vec3 c, glm::vec3 p) {
        color = c;
        position = p;

        default_init();
    }

    glm::vec3 position = glm::vec3(0.0f);
    glm::mat4 shadow_transforms[6];

    void default_init() {
        shadow_projection_mat =
            glm::perspective(glm::radians(90.0f), 1.0f, z_near, z_far);

        shadow_transforms[0] =
            shadow_projection_mat *
            glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0),
                        glm::vec3(0.0, -1.0, 0.0));
        shadow_transforms[1] =
            shadow_projection_mat *
            glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0),
                        glm::vec3(0.0, -1.0, 0.0));
        shadow_transforms[2] =
            shadow_projection_mat *
            glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0),
                        glm::vec3(0.0, 0.0, 1.0));
        shadow_transforms[3] =
            shadow_projection_mat *
            glm::lookAt(position, position + glm::vec3(1.0, -1.0, 0.0),
                        glm::vec3(0.0, 0.0, -1.0));
        shadow_transforms[4] =
            shadow_projection_mat *
            glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0),
                        glm::vec3(0.0, -1.0, 0.0));
        shadow_transforms[5] =
            shadow_projection_mat *
            glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0),
                        glm::vec3(0.0, -1.0, 0.0));
    }
};

struct DirLight : public BaseLight {
    glm::vec3 direction = glm::vec3(-1.0f);

    glm::mat4 light_view = glm::mat4(0.0);
    glm::mat4 light_space_mat = glm::mat4(0.0);

    float distance;
    float ortho_box_size;
};

struct GPULight {
    glm::vec3 position;
    glm::vec3 color;
    float radius;
};

}  // namespace MR