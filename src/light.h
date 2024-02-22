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
    glm::mat4 look_at_per_face[6];

    void default_init() {
        z_near = 0.1f;
        z_far = 100.0f;

        shadow_projection_mat =
            glm::perspective(glm::radians(90.0f), 1.0f, z_near, z_far);

        look_at_per_face[0] =
            glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0),
                        glm::vec3(0.0, -1.0, 0.0));
        look_at_per_face[1] =
            glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0),
                        glm::vec3(0.0, -1.0, 0.0));
        look_at_per_face[2] =
            glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0),
                        glm::vec3(0.0, 0.0, 1.0));
        look_at_per_face[3] =
            glm::lookAt(position, position + glm::vec3(1.0, -1.0, 0.0),
                        glm::vec3(0.0, 0.0, -1.0));
        look_at_per_face[4] =
            glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0),
                        glm::vec3(0.0, -1.0, 0.0));
        look_at_per_face[5] =
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
    glm::vec4 position;
    glm::vec4 color;
    unsigned int enabled;
    float intensity;
    float range;
    float padding;
};

}  // namespace MR