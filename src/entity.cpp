#include "entity.h"

namespace MR {

void Entity::render(bool use) {
    if (use) {
        auto raw = obj.get();
        mat->fill_unifrom(*raw);
    }
    obj->render();
}

}  // namespace MR