
#pragma once

#include "pch.h"

namespace Sandbox {

    enum class MeshTopology {
        LINES,
        TRIANGLES,
        POINTS
    };

    [[nodiscard]] GLuint GetRenderingPrimitive(MeshTopology topology);

}