#include "entity.h"

namespace MR {

void Entity::render() {
    auto raw = obj.get();
    mat->fill_unifrom(*raw);
    obj->render();
}

}  // namespace MR