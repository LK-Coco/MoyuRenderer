#include "physics.h"
#include "scene.h"

namespace MR {

bool Physics::ray_cast(const glm::vec3& origin, const glm::vec3& direction,
                       HitResult& result) {
    float t = std::numeric_limits<float>().max();
    Ray ray;
    ray.origin = origin;
    ray.direction = direction;
    bool is_hit = false;
    for (auto& entity : Scene::entities) {
        if (entity.obj->hit(ray) && t > ray.distance) {
            result.hit_entity = &entity;
            t = ray.distance;
            is_hit = true;
        }
    }

    return is_hit;
}

}  // namespace MR