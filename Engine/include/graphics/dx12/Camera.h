#pragma once

#include <DirectXMath.h>

#include "graphics/dx12/Transform.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            class Camera
            {
            public:
                Camera();

                void SetProjection(float fov, float aspectRatio, float nearPlane, float farPlane);

                Transform& GetTransform();
                DirectX::XMMATRIX GetViewMatrix() const;
                const DirectX::XMMATRIX& GetProjectionMatrix() const;

            private:
                Transform m_Transform;
                DirectX::XMMATRIX m_ProjectionMatrix;
            };
        }
    }
}