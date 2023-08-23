#pragma once

namespace MR {

class Object {
public:
    virtual ~Object(){};
    virtual void render() = 0;
};
}  // namespace MR