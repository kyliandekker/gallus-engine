#include "graphics/dx12/Camera.h"

#include "graphics/dx12/DxMath.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            Camera::Camera()
                : m_Position{ 0.0f, 0.0f, -10.0f }, m_Target{ 0.0f, 0.0f, 0.0f }, m_Up{ 0.0f, 1.0f, 0.0f }
            {
                SetProjection(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
            }

            void Camera::SetPosition(const DirectX::XMFLOAT3& position)
            {
                m_Position = position;
                m_ViewMatrix = DirectX::XMMatrixLookAtLH(
                    DirectX::XMLoadFloat3(&m_Position),
                    DirectX::XMLoadFloat3(&m_Target),
                    DirectX::XMLoadFloat3(&m_Up)
                );
            }

            void Camera::SetTarget(const DirectX::XMFLOAT3& target)
            {
                m_Target = target;
                m_ViewMatrix = DirectX::XMMatrixLookAtLH(
                    DirectX::XMLoadFloat3(&m_Position),
                    DirectX::XMLoadFloat3(&m_Target),
                    DirectX::XMLoadFloat3(&m_Up)
                );
            }

            void Camera::SetProjection(float fov, float aspectRatio, float nearPlane, float farPlane)
            {
                m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
                    DirectX::XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane
                );
            }

            DirectX::XMMATRIX Camera::GetViewMatrix() const
            {
                return m_ViewMatrix;
            }

            DirectX::XMMATRIX Camera::GetProjectionMatrix() const
            {
                return m_ProjectionMatrix;
            }
        }
    }
}