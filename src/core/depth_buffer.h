#include "buffer.h"

namespace MR {

class DepthBuffer : public Buffer<float> {
public:
    DepthBuffer(uint32_t width, uint32_t height);
    DepthBuffer(const DepthBuffer&) = delete;
    DepthBuffer& operator=(const DepthBuffer&) = delete;
    ~DepthBuffer();

    inline const uint32_t width() const noexcept { return width_; }
    inline const uint32_t height() const noexcept { return height_; }

    bool try_update(uint32_t x, uint32_t y, float new_z);
    void clear();

protected:
    uint32_t width_;
    uint32_t height_;
};

}  // namespace MR