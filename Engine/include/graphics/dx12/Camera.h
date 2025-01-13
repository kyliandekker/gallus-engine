#pragma once

/*
 *  Copyright(c) 2018 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

/**
  *  @file Camera.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief DirectX 12 Camera class.
*/

#include <DirectXMath.h>

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            // When performing transformations on the camera, 
            // it is sometimes useful to express which space this 
            // transformation should be applied.
            enum class Space
            {
                Local,
                World,
            };

            class Camera
            {
            public:

                Camera();
                virtual ~Camera();

                void XM_CALLCONV SetLookAt(DirectX::FXMVECTOR a_Eye, DirectX::FXMVECTOR a_Target, DirectX::FXMVECTOR a_Up);
                DirectX::XMMATRIX GetViewMatrix() const;
                DirectX::XMMATRIX GetInverseViewMatrix() const;

                /**
                 * Set the camera to a perspective projection matrix.
                 * @param fovy The vertical field of view in degrees.
                 * @param aspect The aspect ratio of the screen.
                 * @param zNear The distance to the near clipping plane.
                 * @param zFar The distance to the far clipping plane.
                 */
                void SetProjection(float a_FovY, float a_Aspect, float a_zNear, float a_zFar);
                DirectX::XMMATRIX GetProjectionMatrix() const;
                DirectX::XMMATRIX GetInverseProjectionMatrix() const;

                /**
                 * Set the field of view in degrees.
                 */
                void SetFoV(float a_FovY);

                /**
                 * Get the field of view in degrees.
                 */
                float GetFoV() const;

                /**
                 * Set the camera's position in world-space.
                 */
                void XM_CALLCONV SetTranslation(DirectX::FXMVECTOR a_Translation);
                DirectX::XMVECTOR GetTranslation() const;

                /**
                 * Set the camera's rotation in world-space.
                 * @param rotation The rotation quaternion.
                 */
                void XM_CALLCONV SetRotation(DirectX::FXMVECTOR a_Rotation);
                /**
                 * Query the camera's rotation.
                 * @returns The camera's rotation quaternion.
                 */
                DirectX::XMVECTOR GetRotation() const;

                void XM_CALLCONV Translate(DirectX::FXMVECTOR a_Translation, Space a_Space = Space::Local);
                void Rotate(DirectX::FXMVECTOR quaternion);

            protected:
                virtual void UpdateViewMatrix() const;
                virtual void UpdateInverseViewMatrix() const;
                virtual void UpdateProjectionMatrix() const;
                virtual void UpdateInverseProjectionMatrix() const;

                // This data must be aligned otherwise the SSE intrinsics fail
                // and throw exceptions.
                __declspec(align(16)) struct AlignedData
                {
                    // World-space position of the camera.
                    DirectX::XMVECTOR m_Translation;
                    // World-space rotation of the camera.
                    // THIS IS A QUATERNION!!!!
                    DirectX::XMVECTOR m_Rotation;

                    DirectX::XMMATRIX m_ViewMatrix, m_InverseViewMatrix;
                    DirectX::XMMATRIX m_ProjectionMatrix, m_InverseProjectionMatrix;
                };
                AlignedData* pData;

                // projection parameters
                float m_vFoV;   // Vertical field of view.
                float m_AspectRatio; // Aspect ratio
                float m_zNear;      // Near clip distance
                float m_zFar;       // Far clip distance.

                // True if the view matrix needs to be updated.
                mutable bool m_ViewDirty, m_InverseViewDirty;
                // True if the projection matrix needs to be updated.
                mutable bool m_ProjectionDirty, m_InverseProjectionDirty;

            private:

            };
        }
    }
}