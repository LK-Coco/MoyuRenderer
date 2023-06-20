#pragma once

#include <unordered_map>
#include "core/base.h"
#include "core/uuid.h"
#include "texture.h"

namespace MR {

class ShaderProgram;

class Uniform {
public:
    explicit Uniform(std::string name) : name(std::move(name)) {}

    inline int get_hash() const { return uuid_.get(); }

    virtual int get_location(ShaderProgram &program) = 0;
    virtual void bind_program(ShaderProgram &program, int location) = 0;

public:
    std::string name;

private:
    UUID<Uniform> uuid_;
};

class UniformBlock : public Uniform {
public:
    UniformBlock(const std::string &name, int size) : Uniform(name), block_size(size) {}

    virtual void set_subData(void *data, int len, int offset) = 0;
    virtual void set_data(void *data, int len) = 0;

protected:
    int block_size;
};

class UniformSampler : public Uniform {
public:
    UniformSampler(const std::string &name, Texture::Type type, Texture::Format format)
        : Uniform(name),
          type_(type),
          format_(format) {}
    virtual void set_exture(const Ref<Texture> &tex) = 0;

protected:
    Texture::Type type_;
    Texture::Format format_;
};

class ShaderResources {
public:
    std::unordered_map<int, Ref<UniformBlock>> blocks;
    std::unordered_map<int, Ref<UniformSampler>> samplers;
};
}  // namespace MR