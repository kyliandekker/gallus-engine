#include "graphics/dx12/Transform.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
            Transform::Transform()
                : m_Position{ 0.0f, 0.0f, 0.0f },
                m_Rotation{ 0.0f, 0.0f, 0.0f },
                m_Scale{ 1.0f, 1.0f, 1.0f }
            {}

            void Transform::SetPosition(const DirectX::XMFLOAT3& a_Position)
            {
                m_Position = a_Position;
            }

            void Transform::SetRotation(const DirectX::XMFLOAT3& a_Rotation)
            {
                m_Rotation = a_Rotation;
            }

            void Transform::SetScale(const DirectX::XMFLOAT3& a_Scale)
            {
                m_Scale = a_Scale;
            }

            DirectX::XMFLOAT3& Transform::GetPosition()
            {
                return m_Position;
            }

            DirectX::XMFLOAT3& Transform::GetRotation()
            {
                return m_Rotation;
            }

            DirectX::XMFLOAT3& Transform::GetScale()
            {
                return m_Scale;
            }

            const DirectX::XMFLOAT3& Transform::GetPosition() const
            {
                return m_Position;
            }

            const DirectX::XMFLOAT3& Transform::GetRotation() const
            {
                return m_Rotation;
            }

            const DirectX::XMFLOAT3& Transform::GetScale() const
            {
                return m_Scale;
            }

            const DirectX::XMMATRIX Transform::GetWorldMatrix() const
            {
                // Convert rotation from degrees to radians
                DirectX::XMVECTOR rotationQuaternion = DirectX::XMQuaternionRotationRollPitchYaw(
                    DirectX::XMConvertToRadians(m_Rotation.x),
                    DirectX::XMConvertToRadians(m_Rotation.y),
                    DirectX::XMConvertToRadians(m_Rotation.z)
                );

                // Compute transformations
                DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position));
                DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotationQuaternion);
                DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale));

                // Combine transformations: S * R * T
                return scaleMatrix * rotationMatrix * translationMatrix;
            }
		}
	}
}