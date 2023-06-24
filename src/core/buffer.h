#pragma once

#include <stdint.h>
#include "glm_inc.h"

namespace MR {

enum BufferLayout {
    Layout_Linear,
    Layout_Tiled,
    Layout_Morton,
};

template <typename T>
class Buffer {
public:
    Buffer();
    virtual ~Buffer();
    Buffer(uint32_t length);
    Buffer(const Buffer<T>&);
    Buffer<T>& operator=(const Buffer<T>&);

    const uint32_t get_length() const noexcept;
    const T* get() const noexcept;
    T* get() noexcept;

protected:
    T* data_ = nullptr;
    uint32_t length_ = 0;
};

}  // namespace MR

#include "buffer.inl"