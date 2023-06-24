#include "depth_buffer.h"

namespace MR {

DepthBuffer::DepthBuffer(uint32_t width, uint32_t height)
    : Buffer(width * height),
      width_(width),
      height_(height) {}

DepthBuffer::~DepthBuffer() {}

bool DepthBuffer::try_update(uint32_t x, uint32_t y, float new_z) {
    if (x >= width_ || y >= height_) return false;
    if (new_z < data_[y * width_ + x]) {
        data_[y * width_ + x] = new_z;
        return true;
    }

    return false;
}

void DepthBuffer::clear() { std::fill(data_, data_ + length_, std::numeric_limits<float>::max()); }

}  // namespace MR