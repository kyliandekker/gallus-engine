#include "graphics/dx12/Camera.h"

#include "graphics/dx12/DxMath.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            Camera::Camera()
            {
                SetProjection(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
            }

            void Camera::SetProjection(float a_FoV, float a_AspectRatio, float a_NearPlane, float a_FarPlane)
            {
                m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
                    DirectX::XMConvertToRadians(a_FoV), a_AspectRatio, a_NearPlane, a_FarPlane
                );
            }

            Transform& Camera::GetTransform()
            {
                return m_Transform;
            }

            DirectX::XMMATRIX Camera::GetViewMatrix() const
            {
                return DirectX::XMMatrixLookAtLH(
                    DirectX::XMLoadFloat3(&m_Transform.GetPosition()),
                    DirectX::XMLoadFloat3(&m_Transform.GetPosition()) + DirectX::XMVectorSet(0, 0, 1, 0), // Assume forward direction along Z
                    DirectX::XMVectorSet(0, 1, 0, 0) // Up direction
                );
            }

            const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const
            {
                return m_ProjectionMatrix;
            }
        }
    }
}