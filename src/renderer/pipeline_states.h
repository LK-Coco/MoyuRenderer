#pragma once

#include "render_states.h"

namespace MR {

class PipelineStates {
public:
    explicit PipelineStates(const RenderStates& states) : states_(states) {}

    virtual ~PipelineStates() = default;

private:
    RenderStates states_;
};

}  // namespace MR