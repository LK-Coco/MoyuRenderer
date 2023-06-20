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
    Buffer(uint32_t w, uint32_t h, T *data)
        : width_(w),
          height_(h),
          data_(data),
          data_size_(w * h) {}
    virtual ~Buffer() { dipose(); }
    virtual void dipose() {
        width_ = 0;
        height_ = 0;
        inner_width_ = 0;
        inner_height_ = 0;
        data_size_ = 0;
        if (data_ != nullptr) delete[] data_;
        T *data_ = nullptr;
    }

    inline bool empty() const { return data_ == nullptr; }
    inline BufferLayout get_layout() const { return Layout_Linear; }
    inline uint32_t get_width() const { return width_; }
    inline uint32_t get_height() const { return height_; }
    inline uint32_t get_data_size() const { return data_size_; }
    inline uint32_t get_bytes_size() const { return data_size_ * sizeof(T); }

protected:
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    uint32_t inner_width_ = 0;
    uint32_t inner_height_ = 0;
    T *data_ = nullptr;
    uint32_t data_size_ = 0;
};

}  // namespace MR