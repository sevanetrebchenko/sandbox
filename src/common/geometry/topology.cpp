
#include "common/geometry/mesh/topology.h"

namespace Sandbox {

    GLuint GetRenderingPrimitive(MeshTopology topology) {
        switch (topology) {
            case MeshTopology::LINES:
                return GL_LINES;
            case MeshTopology::TRIANGLES:
                return GL_TRIANGLES;
            case MeshTopology::POINTS:
                return GL_POINTS;
        }
    }

}
