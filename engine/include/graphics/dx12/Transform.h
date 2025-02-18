#pragma once

#include <DirectXMath.h>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class Transform
			{
			public:
				Transform();

				// Setters
				void SetPosition(const DirectX::XMFLOAT3& a_Position);
				void SetRotation(const DirectX::XMFLOAT3& a_Rotation); // Rotation in degrees
				void SetScale(const DirectX::XMFLOAT3& a_Scale);

				// Getters
				DirectX::XMFLOAT3& GetPosition();
				DirectX::XMFLOAT3& GetRotation();
				DirectX::XMFLOAT3& GetScale();

				const DirectX::XMFLOAT3& GetPosition() const;
				const DirectX::XMFLOAT3& GetRotation() const;
				const DirectX::XMFLOAT3& GetScale() const;

				// Compute world matrix
				const DirectX::XMMATRIX GetWorldMatrix() const;
				void SetWorldMatrix(const DirectX::XMMATRIX& worldMatrix);

			private:
				DirectX::XMFLOAT3 m_Position;
				DirectX::XMFLOAT3 m_Rotation; // Euler angles in degrees
				DirectX::XMFLOAT3 m_Scale;
			};
		}
	}
}