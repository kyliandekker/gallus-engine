#include "graphics/dx12/Transform.h"

namespace coopscoop
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

            void Transform::SetPosition(const DirectX::XMFLOAT3& position)
            {
                m_Position = position;
            }

            void Transform::SetRotation(const DirectX::XMFLOAT3& rotation)
            {
                m_Rotation = rotation;
            }

            void Transform::SetScale(const DirectX::XMFLOAT3& scale)
            {
                m_Scale = scale;
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

            DirectX::XMFLOAT3 Transform::GetPosition() const
            {
                return m_Position;
            }

            DirectX::XMFLOAT3 Transform::GetRotation() const
            {
                return m_Rotation;
            }

            DirectX::XMFLOAT3 Transform::GetScale() const
            {
                return m_Scale;
            }

            DirectX::XMMATRIX Transform::GetWorldMatrix() const
            {
                using namespace DirectX;

                // Convert rotation from degrees to radians
                XMVECTOR rotationQuaternion = XMQuaternionRotationRollPitchYaw(
                    XMConvertToRadians(m_Rotation.x),
                    XMConvertToRadians(m_Rotation.y),
                    XMConvertToRadians(m_Rotation.z)
                );

                // Compute transformations
                XMMATRIX translationMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&m_Position));
                XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(rotationQuaternion);
                XMMATRIX scaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&m_Scale));

                // Combine transformations: S * R * T
                return scaleMatrix * rotationMatrix * translationMatrix;
            }
		}
	}
}