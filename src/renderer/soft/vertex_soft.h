#pragma once

#include "core/uuid.h"
#include "renderer/vertex.h"

namespace MR {

class VertexArrayObjectSoft : public VertexArrayObject {
public:
    explicit VertexArrayObjectSoft(const VertexArray& vertex_array) {
        vertex_stride = vertex_array.vertex_desces[0].stride;
        vertex_count = vertex_array.vertex_buffer_size / vertex_stride;
        vertexes.resize(vertex_count * vertex_stride);
        std::memcpy(vertexes.data(), vertex_array.vertex_buffer, vertex_array.vertex_buffer_size);

        indices_count = vertex_array.index_buffer_size / sizeof(int32_t);
        indices.resize(indices_count);
        std::memcpy(indices.data(), vertex_array.index_buffer, vertex_array.index_buffer_size);
    }

    void update_vertex_data(void* data, size_t size) override {
        std::memcpy(vertexes.data(), data, std::min(size, vertexes.size()));
    }

    int id() const override { return uuid_.get(); }

public:
    size_t vertex_stride = 0;
    size_t vertex_count = 0;
    size_t indices_count = 0;
    std::vector<uint8_t> vertexes;
    std::vector<int32_t> indices;

private:
    UUID<VertexArrayObjectSoft> uuid_;
};

}  // namespace MR