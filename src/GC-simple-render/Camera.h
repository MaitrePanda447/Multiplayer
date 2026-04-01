#pragma once

#include "Transform.h"

#include <Aliases.h>

#include <GCE/Render/define.h>
#include <GCE/Render/RenderCamera.h>

namespace sr
{
    enum CameraType : int8
    {
        PERSPECTIVE, ORTHOGRAPHIC
    };
    
    class Camera
    {
    public:
        Camera(CameraType type); 
        ~Camera() = default;

        void SetPosition(gce::Vector3f32 const& pos);
        void SetRotation(gce::Vector3f32 const& rotation);

        void Translate(gce::Vector3f32 const& translation);
        void Rotate(gce::Vector3f32 const& rotation);

        gce::Vector3f32 GetPosition() { return m_transform.position; };
        gce::Vector3f32 GetRotation() { return m_transform.rotation; };

        
        //Perspective settings
        void SetFOV(float32 fov);
        void SetNearPlane(float32 nearPlane);
        void SetFarPlane(float32 farPlane);

        //Orthographic settings
        void SetViewWidth(float32 width);
        void SetViewHeight(float32 height);

    private:
        ////////////////////////////////////////////////////////////////////////////////////////
        /// @note This function is automatically called by the window when a resizing occur.
        /// You should not call it manually.
        /// @param aspectRatio The new aspect ratio of the scene. 
        ////////////////////////////////////////////////////////////////////////////////////////
        void SetAspectRatio(float32 aspectRatio);
        
        gce::RenderCamera& GetGCRCamera();
        void UpdateWorldMatrix();
    private:
        gce::RenderCamera m_camera;
        CameraType m_type;
        Transform m_transform;        

        bool m_dirty = false;
        
        friend class Renderer;
        friend class Window;
        friend class ShadowMap;
     };
}
