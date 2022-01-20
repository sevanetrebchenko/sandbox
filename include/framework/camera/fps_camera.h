
#ifndef SANDBOX_FPS_CAMERA_H
#define SANDBOX_FPS_CAMERA_H

#include <framework/camera/camera.h>

namespace Sandbox {

    class FPSCamera : public ICamera {
        public:
            FPSCamera(int width, int height);
            ~FPSCamera() override;

            void Update() override;

        private:
            void CameraMovement();
            void LookAround();

            float cameraSpeed_;
    };

}

#endif //SANDBOX_FPS_CAMERA_H
