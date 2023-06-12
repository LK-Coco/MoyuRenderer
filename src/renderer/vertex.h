#pragma once

#include "core/base.h"

namespace MR {

class VertexArrayObject {
public:
    virtual int id() const = 0;
    virtual void update_vertex_data(void *data, size_t size) = 0;
};

struct VertexArributeDesc {
    size_t size;
    size_t stride;
    size_t offset;
};

struct VertexArray {
    size_t vertex_size = 0;
    std::vector<VertexArributeDesc> vertex_desces;

    uint8_t *vertex_buffer = nullptr;
    size_t vertex_buffer_size = 0;
    int32_t *index_buffer = nullptr;
    size_t index_buffer_size = 0;
};

}  // namespace MR