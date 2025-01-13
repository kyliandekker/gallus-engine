#pragma once

#include <DirectXMath.h>

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

                void SetPosition(const DirectX::XMFLOAT3& position);
                void SetTarget(const DirectX::XMFLOAT3& target);
                void SetProjection(float fov, float aspectRatio, float nearPlane, float farPlane);

                DirectX::XMMATRIX GetViewMatrix() const;
                DirectX::XMMATRIX GetProjectionMatrix() const;

            private:
                DirectX::XMFLOAT3 m_Position;
                DirectX::XMFLOAT3 m_Target;
                DirectX::XMFLOAT3 m_Up;
                DirectX::XMMATRIX m_ViewMatrix;
                DirectX::XMMATRIX m_ProjectionMatrix;
            };
        }
    }
}