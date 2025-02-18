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

			DirectX::XMFLOAT3 QuaternionToEuler(const DirectX::XMFLOAT4& quat)
			{
				DirectX::XMFLOAT3 euler;

				// Convert quaternion to rotation matrix
				DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quat));

				// Store matrix in XMFLOAT4X4 to access elements properly
				DirectX::XMFLOAT4X4 rotMatrix;
				DirectX::XMStoreFloat4x4(&rotMatrix, rotationMatrix);

				// Extract Euler angles (Yaw-Pitch-Roll)
				euler.y = atan2(rotMatrix._13, rotMatrix._33); // Yaw
				euler.x = asin(-rotMatrix._23);               // Pitch
				euler.z = atan2(rotMatrix._21, rotMatrix._22); // Roll

				return euler;
			}


			void Transform::SetWorldMatrix(const DirectX::XMMATRIX& worldMatrix)
			{
				DirectX::XMVECTOR scale, rotation, translation;
				if (DirectX::XMMatrixDecompose(&scale, &rotation, &translation, worldMatrix))
				{
					m_Position = DirectX::XMFLOAT3(DirectX::XMVectorGetX(translation),
						DirectX::XMVectorGetY(translation),
						DirectX::XMVectorGetZ(translation));

					DirectX::XMFLOAT4 quat;
					DirectX::XMStoreFloat4(&quat, rotation);
					DirectX::XMFLOAT3 eulerRotation = QuaternionToEuler(quat);
					m_Rotation = eulerRotation;

					m_Scale = DirectX::XMFLOAT3(DirectX::XMVectorGetX(scale),
						DirectX::XMVectorGetY(scale),
						DirectX::XMVectorGetZ(scale));
				}
			}

		}
	}
}