#pragma once

#include <GCE/Render/Geometry.h>

#include <Aliases.h>

#include "Renderer.h"
#include "Transform.h"

namespace gce
{
    class Geometry;
}

namespace sr
{
    struct Texture;

    class Geometry
    {
    public:
        Geometry(gce::Geometry* pGceGeo);
        Geometry(Geometry const& other) = delete;
        Geometry(Geometry&& other) noexcept = delete;

        Geometry& operator=(Geometry const& other) = delete;
        Geometry& operator=(Geometry&& other) noexcept = delete;
        virtual ~Geometry();

        void SetPosition(gce::Vector3f32 const& pos);
        void SetRotation(gce::Vector3f32 const& rot);
        void SetScale(gce::Vector3f32 const& scale);

        void Translate(gce::Vector3f32 const& translation);
        void Rotate(gce::Vector3f32 const& rotation);
        void Scale(gce::Vector3f32 const& scale);

        gce::Vector3f32 GetPosition();
        gce::Vector3f32 GetRotation();
        gce::Vector3f32 GetScale();

        void SetTexture(Texture const& texture);
        void SetColor(gce::Vector3f32 const& color);

    protected:
        void UpdateWorldMatrix();
        gce::RenderItem& GetRenderItem();
        
    protected:
        gce::Geometry* m_pGeometry = nullptr;
        //Render Item with default material
        gce::RenderItem m_renderItem;
        
        Transform m_transform;
        bool m_isDirty = false;

        friend class Window;
    };
}

