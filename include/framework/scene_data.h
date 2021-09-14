
#ifndef SANDBOX_SCENE_DATA_H
#define SANDBOX_SCENE_DATA_H

#include <sandbox.h>
#include <framework/scene.h>

namespace Sandbox {

    struct SceneData {
        IScene* scene_;

        std::string scenePrefix_; // All lowercase, underscored name. Used for files.
        std::string sceneName_;
        bool hasAlias_;

        std::string dataDirectory_;
        std::string imGuiIniName_;
        std::string imGuiLogName_;
    };

}

#endif //SANDBOX_SCENE_DATA_H
