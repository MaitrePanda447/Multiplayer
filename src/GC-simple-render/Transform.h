#pragma once
#include <GCE/Core/Maths/Vector3.h>
#include <GCE/Core/Maths/Quaternion.h>
#include <GCE/Core/Maths/Matrix.h>
#include <DirectXMath.h>

#include <Aliases.h>

namespace sr
{
    struct Transform
    {
        void SetPosition(const gce::Vector3f32& pos);
        void SetRotation(gce::Vector3f32 const& eulerRot);
        void SetScale(gce::Vector3f32 const& s);

        void Translate(gce::Vector3f32 const& translation);
        void Rotate(gce::Vector3f32 const& eulerRot);
        void Scale(gce::Vector3f32 const& s);

        gce::Matrix worldMatrix;
        gce::Quaternion quatRotation;
        gce::Vector3f32 position = {0.0f, 0.0f, 0.0f};
        gce::Vector3f32 scale = {1.0f, 1.0f, 1.0f};
        gce::Vector3f32 rotation = {0.0f, 0.0f, 0.0f};

    private:
        void SetMatrix(gce::Matrix const& matrix);
        void CalculateMatrix();

    private:
        friend class ShadowMap;
        friend class Geometry;
        friend class Camera;
    };

    inline void Transform::CalculateMatrix()
    {
        using namespace gce;
        
        Matrix transform = Matrix::Identity();
        transform *= Matrix::Scale(scale);
        transform *= Matrix::Rotation(quatRotation);
        transform *= Matrix::Translation(position);
        worldMatrix = transform;
    }

    inline void Transform::SetMatrix(gce::Matrix const& matrix)
    {
        worldMatrix = matrix;
    }

    inline void Transform::SetPosition(gce::Vector3f32 const& pos)
    {
        position = pos; 
    }

    inline void Transform::SetRotation(gce::Vector3f32 const& eulerRot)
    {
        float32 y = DirectX::XMConvertToRadians(eulerRot.x);
        float32 p = DirectX::XMConvertToRadians(eulerRot.y);
        float32 r = DirectX::XMConvertToRadians(eulerRot.z);
        
        rotation += gce::Vector3f32(y, p, r);
        quatRotation = gce::Quaternion::RotationEuler(gce::Vector3f32(y, p, r)); 
    }

    inline void Transform::SetScale(gce::Vector3f32 const& s)
    {
       scale = s;
    }

    inline void Transform::Translate(gce::Vector3f32 const& translation)
    {
        position += translation;
    }

    inline void Transform::Rotate(gce::Vector3f32 const& eulerRot)
    {
        quatRotation *= gce::Quaternion::RotationEuler(eulerRot);
    }

    inline void Transform::Scale(gce::Vector3f32 const& s)
    {
        scale += s;
    }
}
