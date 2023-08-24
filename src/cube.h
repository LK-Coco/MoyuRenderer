#pragma once
#include "object.h"
#include "glad/glad.h"

namespace MR {

class Cube : public Object {
public:
    Cube();
    ~Cube();

    virtual void render(std::shared_ptr<Shader> &shader) override;

private:
    void init();

    GLuint vao_;
};
}  // namespace MR