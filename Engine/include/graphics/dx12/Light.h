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
  *  @file Light.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief Light structures that use HLSL constant buffer padding rules.
  */

#include <DirectXMath.h>

namespace coopscoop
{
    namespace graphics
    {
        namespace dx12
        {
            struct PointLight
            {
                PointLight()
                    : Position(0.0f, 0.0f, 0.0f, 1.0f)
                    , Color(1.0f, 1.0f, 1.0f, 1.0f)
                    , ConstantAttenuation(1.0f)
                    , LinearAttenuation(0.0f)
                    , QuadraticAttenuation(0.0f)
                {}

                DirectX::XMFLOAT4    Position; // Light position in world space.
                //----------------------------------- (16 byte boundary)
                DirectX::XMFLOAT4    Color;
                //----------------------------------- (16 byte boundary)
                float       Ambient;
                float       ConstantAttenuation;
                float       LinearAttenuation;
                float       QuadraticAttenuation;
                //----------------------------------- (16 byte boundary)
                // Total:                              16 * 3 = 48 bytes
            };

            struct SpotLight
            {
                SpotLight()
                    : Position(0.0f, 0.0f, 0.0f, 1.0f)
                    , Direction(0.0f, 0.0f, 1.0f, 0.0f)
                    , Color(1.0f, 1.0f, 1.0f, 1.0f)
                    , SpotAngle(DirectX::XM_PIDIV2)
                    , ConstantAttenuation(1.0f)
                    , LinearAttenuation(0.0f)
                    , QuadraticAttenuation(0.0f)
                {}

                DirectX::XMFLOAT4    Position; // Light position in world space.
                //----------------------------------- (16 byte boundary)
                DirectX::XMFLOAT4    Direction; // Light direction in world space.
                //----------------------------------- (16 byte boundary)
                DirectX::XMFLOAT4    Color;
                //----------------------------------- (16 byte boundary)
                float       Ambient;
                float       SpotAngle;
                float       ConstantAttenuation;
                float       LinearAttenuation;
                //----------------------------------- (16 byte boundary)
                float       QuadraticAttenuation;
                //----------------------------------- (4 byte boundary)
                DirectX::XMFLOAT3      Padding = { 0,0, 0 }; // 12 bytes (Fix alignment)
                // Total:                              16 * 5 = 80 bytes
            };

            struct DirectionalLight
            {
                DirectionalLight() 
                    : Direction(0.0f, 0.0f, 1.0f, 0.0f),
                    Color(1.0f, 1.0f, 1.0f, 1.0f)
                {}

                DirectX::XMFLOAT4    Direction; // Light direction in world space.
                //----------------------------------- (16 byte boundary)
                DirectX::XMFLOAT4    Color;
                //----------------------------------- (16 byte boundary)
                float       Ambient;
                //----------------------------------- (4 byte boundary)
                DirectX::XMFLOAT3      Padding = { 0,0, 0 }; // 12 bytes (Fix alignment)
                // Total:                              16 * 3 = 48 bytes 
            };
        }
    }
}