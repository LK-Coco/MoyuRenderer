#pragma once

#include "core/base.h"
#include "texture.h"

namespace MR {

struct FrameBufferAttachment {
    Ref<Texture> tex = nullptr;
    uint32_t layer = 0;
    uint32_t level = 0;
};

class FrameBuffer {
public:
    explicit FrameBuffer(bool off_screen) : off_screen_(off_screen) {}

    virtual void set_color_attachment(Ref<Texture>& color_tex, uint32_t level) {
        color_attachment_.tex = color_tex;
        color_attachment_.layer = 0;
        color_attachment_.level = level;
        color_ready_ = true;
    }

    virtual void set_depth_attachment(Ref<Texture>& depth_tex) {
        depth_attachment_.tex = depth_tex;
        depth_attachment_.layer = 0;
        depth_attachment_.level = 0;
        depth_ready_ = true;
    }

    inline const FrameBufferAttachment& get_color_attachment() const { return color_attachment_; }
    inline const FrameBufferAttachment& get_depth_attachment() const { return depth_attachment_; }

    inline bool is_off_screen() const { return off_screen_; }

private:
    bool color_ready_ = false;
    bool depth_ready_ = false;
    bool off_screen_;

    FrameBufferAttachment color_attachment_;
    FrameBufferAttachment depth_attachment_;
};

}  // namespace MR