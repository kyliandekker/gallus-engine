#pragma once

#include <DirectXMath.h>

inline DirectX::XMVECTOR& operator+=(DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs) {
    lhs = DirectX::XMVectorAdd(lhs, rhs);
    return lhs;
}

// Subtraction Assignment (-=)
inline DirectX::XMVECTOR& operator-=(DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs) {
    lhs = DirectX::XMVectorSubtract(lhs, rhs);
    return lhs;
}

// Multiplication Assignment (*=) - Component-wise
inline DirectX::XMVECTOR& operator*=(DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs) {
    lhs = DirectX::XMVectorMultiply(lhs, rhs);
    return lhs;
}

// Division Assignment (/=) - Component-wise
inline DirectX::XMVECTOR& operator/=(DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs) {
    lhs = DirectX::XMVectorDivide(lhs, rhs);
    return lhs;
}

// Scalar Multiplication Assignment (*=)
inline DirectX::XMVECTOR& operator*=(DirectX::XMVECTOR& lhs, float scalar) {
    lhs = DirectX::XMVectorScale(lhs, scalar);
    return lhs;
}

// Scalar Division Assignment (/=)
inline DirectX::XMVECTOR& operator/=(DirectX::XMVECTOR& lhs, float scalar) {
    lhs = DirectX::XMVectorScale(lhs, 1.0f / scalar);
    return lhs;
}

// Binary Addition (+)
inline DirectX::XMVECTOR operator+(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs) {
    return DirectX::XMVectorAdd(lhs, rhs);
}

// Binary Subtraction (-)
inline DirectX::XMVECTOR operator-(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs) {
    return DirectX::XMVectorSubtract(lhs, rhs);
}

// Binary Subtraction (-)
inline DirectX::XMVECTOR operator-(const DirectX::XMVECTOR& lhs) {
    return DirectX::XMVectorSubtract(DirectX::XMVectorSet(0, 0, 0, 0), lhs);
}

// Binary Multiplication (*)
inline DirectX::XMVECTOR operator*(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs) {
    return DirectX::XMVectorMultiply(lhs, rhs);
}

// Binary Division (/)
inline DirectX::XMVECTOR operator/(const DirectX::XMVECTOR& lhs, const DirectX::XMVECTOR& rhs) {
    return DirectX::XMVectorDivide(lhs, rhs);
}

// Scalar Multiplication (*)
inline DirectX::XMVECTOR operator*(const DirectX::XMVECTOR& vec, float scalar) {
    return DirectX::XMVectorScale(vec, scalar);
}

inline DirectX::XMVECTOR operator*(float scalar, const DirectX::XMVECTOR& vec) {
    return DirectX::XMVectorScale(vec, scalar);
}

// Scalar Division (/)
inline DirectX::XMVECTOR operator/(const DirectX::XMVECTOR& vec, float scalar) {
    return DirectX::XMVectorScale(vec, 1.0f / scalar);
}