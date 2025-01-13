#pragma once

#include <DirectXMath.h>

namespace coopscoop
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
                void SetPosition(const DirectX::XMFLOAT3& position);
                void SetRotation(const DirectX::XMFLOAT3& rotation); // Rotation in degrees
                void SetScale(const DirectX::XMFLOAT3& scale);

                // Getters
                DirectX::XMFLOAT3& GetPosition();
                DirectX::XMFLOAT3& GetRotation();
                DirectX::XMFLOAT3& GetScale();

                DirectX::XMFLOAT3 GetPosition() const;
                DirectX::XMFLOAT3 GetRotation() const;
                DirectX::XMFLOAT3 GetScale() const;

                // Compute world matrix
                DirectX::XMMATRIX GetWorldMatrix() const;

            private:
                DirectX::XMFLOAT3 m_Position;
                DirectX::XMFLOAT3 m_Rotation; // Euler angles in degrees
                DirectX::XMFLOAT3 m_Scale;
            };
		}
	}
}