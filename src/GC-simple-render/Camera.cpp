#include "Camera.h"

using namespace gce;

namespace sr
{
    Camera::Camera(CameraType const type) : m_camera(static_cast<CAMERA_TYPE>(type)), m_type(type){}

    void Camera::SetPosition(Vector3f32 const& pos)
    {
        m_transform.SetPosition(pos);
        m_dirty = true;
    }

    void Camera::SetRotation(Vector3f32 const& rotation)
    {
        m_transform.SetRotation(rotation);
        m_dirty = true;
    }

    void Camera::Translate(Vector3f32 const& translation)
    {
        m_transform.Translate(translation);
        m_dirty = true;
    }

    void Camera::Rotate(gce::Vector3f32 const& rotation)
    {
        m_transform.Rotate(rotation);
        m_dirty = true;
    }

    RenderCamera& Camera::GetGCRCamera()
    {
        if (m_dirty)
        {
            UpdateWorldMatrix();
            m_dirty = false;
        }
        return m_camera;
    }

    void Camera::SetFOV(float32 const fov)
    {
        if (m_type != PERSPECTIVE) return;
        m_camera.perspective.fov = fov;
        m_dirty = true;
    }


    void Camera::SetNearPlane(float32 const nearPlane)
    {
        m_camera.perspective.nearPlane = nearPlane;
        m_dirty = true;
    }

    void Camera::SetFarPlane(float32 const farPlane)
    {
        m_camera.perspective.farPlane = farPlane; 
        m_dirty = true;
    }

    void Camera::SetViewWidth(float32 const width)
    {
        if (m_type != ORTHOGRAPHIC) return;
        m_camera.orthographic.viewWidth = width;
        m_dirty = true;
    }
    
    void Camera::SetViewHeight(float32 const height)
    {
        if (m_type != ORTHOGRAPHIC) return;
        m_camera.orthographic.viewHeight = height;
        m_dirty = true;
    }

    void Camera::SetAspectRatio(float32 const aspectRatio)
    {
        if (m_type != PERSPECTIVE) return;
        m_camera.perspective.aspectRatio = aspectRatio;
        m_dirty = true;
    }
    
    void Camera::UpdateWorldMatrix()
    {
        m_transform.CalculateMatrix();
        m_camera.SetMatrix(m_transform.worldMatrix);
    }

}
