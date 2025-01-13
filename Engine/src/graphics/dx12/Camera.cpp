#include "graphics/dx12/Camera.h"

#include "graphics/dx12/DxMath.h"

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            Camera::Camera()
                : m_ViewDirty(true)
                , m_InverseViewDirty(true)
                , m_ProjectionDirty(true)
                , m_InverseProjectionDirty(true)
                , m_vFoV(45.0f)
                , m_AspectRatio(1.0f)
                , m_zNear(0.1f)
                , m_zFar(100.0f)
            {
                pData = (AlignedData*)_aligned_malloc(sizeof(AlignedData), 16);
                pData->m_Translation = DirectX::XMVectorZero();
                pData->m_Rotation = DirectX::XMQuaternionIdentity();
            }

            Camera::~Camera()
            {
                _aligned_free(pData);
            }

            void XM_CALLCONV Camera::SetLookAt(DirectX::FXMVECTOR a_Eye, DirectX::FXMVECTOR a_Target, DirectX::FXMVECTOR a_Up)
            {
                pData->m_ViewMatrix = DirectX::XMMatrixLookAtLH(a_Eye, a_Target, a_Up);

                pData->m_Translation = a_Eye;
                pData->m_Rotation = XMQuaternionRotationMatrix(XMMatrixTranspose(pData->m_ViewMatrix));

                m_InverseViewDirty = true;
                m_ViewDirty = false;
            }

            DirectX::XMMATRIX Camera::GetViewMatrix() const
            {
                if (m_ViewDirty)
                {
                    UpdateViewMatrix();
                }
                return pData->m_ViewMatrix;
            }

            DirectX::XMMATRIX Camera::GetInverseViewMatrix() const
            {
                if (m_InverseViewDirty)
                {
                    pData->m_InverseViewMatrix = DirectX::XMMatrixInverse(nullptr, pData->m_ViewMatrix);
                    m_InverseViewDirty = false;
                }

                return pData->m_InverseViewMatrix;
            }

            void Camera::SetProjection(float a_FovY, float a_Aspect, float a_zNear, float a_zFar)
            {
                m_vFoV = a_FovY;
                m_AspectRatio = a_Aspect;
                m_zNear = a_zNear;
                m_zFar = a_zFar;

                m_ProjectionDirty = true;
                m_InverseProjectionDirty = true;
            }

            DirectX::XMMATRIX Camera::GetProjectionMatrix() const
            {
                if (m_ProjectionDirty)
                {
                    UpdateProjectionMatrix();
                }

                return pData->m_ProjectionMatrix;
            }

            DirectX::XMMATRIX Camera::GetInverseProjectionMatrix() const
            {
                if (m_InverseProjectionDirty)
                {
                    UpdateInverseProjectionMatrix();
                }

                return pData->m_InverseProjectionMatrix;
            }

            void Camera::SetFoV(float a_FovY)
            {
                if (m_vFoV != a_FovY)
                {
                    m_vFoV = a_FovY;
                    m_ProjectionDirty = true;
                    m_InverseProjectionDirty = true;
                }
            }

            float Camera::GetFoV() const
            {
                return m_vFoV;
            }


            void XM_CALLCONV Camera::SetTranslation(DirectX::FXMVECTOR a_Translation)
            {
                pData->m_Translation = a_Translation;
                m_ViewDirty = true;
            }

            DirectX::XMVECTOR Camera::GetTranslation() const
            {
                return pData->m_Translation;
            }

            void Camera::SetRotation(DirectX::FXMVECTOR a_Rotation)
            {
                pData->m_Rotation = a_Rotation;
            }

            DirectX::XMVECTOR Camera::GetRotation() const
            {
                return pData->m_Rotation;
            }

            void XM_CALLCONV Camera::Translate(DirectX::FXMVECTOR a_Translation, Space a_Space)
            {
                switch (a_Space)
                {
                    case Space::Local:
                    {
                        pData->m_Translation += DirectX::XMVector3Rotate(a_Translation, pData->m_Rotation);
                    }
                    break;
                    case Space::World:
                    {
                        pData->m_Translation += a_Translation;
                    }
                    break;
                }

                pData->m_Translation = DirectX::XMVectorSetW(pData->m_Translation, 1.0f);

                m_ViewDirty = true;
                m_InverseViewDirty = true;
            }

            void Camera::Rotate(DirectX::FXMVECTOR a_Quaternion)
            {
                pData->m_Rotation = DirectX::XMQuaternionMultiply(pData->m_Rotation, a_Quaternion);

                m_ViewDirty = true;
                m_InverseViewDirty = true;
            }

            void Camera::UpdateViewMatrix() const
            {
                DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationQuaternion(pData->m_Rotation));
                DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(-(pData->m_Translation));

                pData->m_ViewMatrix = translationMatrix * rotationMatrix;

                m_InverseViewDirty = true;
                m_ViewDirty = false;
            }

            void Camera::UpdateInverseViewMatrix() const
            {
                if (m_ViewDirty)
                {
                    UpdateViewMatrix();
                }

                pData->m_InverseViewMatrix = XMMatrixInverse(nullptr, pData->m_ViewMatrix);
                m_InverseViewDirty = false;
            }

            void Camera::UpdateProjectionMatrix() const
            {
                pData->m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_vFoV), m_AspectRatio, m_zNear, m_zFar);

                m_ProjectionDirty = false;
                m_InverseProjectionDirty = true;
            }

            void Camera::UpdateInverseProjectionMatrix() const
            {
                if (m_ProjectionDirty)
                {
                    UpdateProjectionMatrix();
                }

                pData->m_InverseProjectionMatrix = DirectX::XMMatrixInverse(nullptr, pData->m_ProjectionMatrix);
                m_InverseProjectionDirty = false;
            }
        }
    }
}