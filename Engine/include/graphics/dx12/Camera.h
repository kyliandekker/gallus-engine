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

                void SetProjection(float a_FoV, float a_AspectRatio, float a_NearPlane, float a_FarPlane);

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