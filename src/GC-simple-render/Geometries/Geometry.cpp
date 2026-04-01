#include "Geometry.h"

#include <GCE/Render/FrameResources.h>

#include "Texture.h"

namespace sr
{
    
    Geometry::~Geometry()
    {
        m_pGeometry->Release();
        gce::FrameResourcesManager::Instance().RemoveRenderItem(m_renderItem);
    }

    Geometry::Geometry(gce::Geometry* mpGceGeo)
    {
        gce::ObjectData& objData = m_renderItem.objData;
        objData.material.useTextureMetalness = 0;
        objData.material.useTextureAlbedo = 0;
        objData.material.useTextureAmbient = 0;
        objData.material.useTextureNormal = 0;
        objData.material.useTextureRoughness = 0;
        objData.material.useDisplacementTexture = 0;

        m_renderItem.pPso = Renderer::m_pPipelineStateObject;
        UpdateWorldMatrix();
        gce::FrameResourcesManager::AddRenderItem(m_renderItem);

        m_pGeometry = mpGceGeo;
        m_renderItem.pGeometry = m_pGeometry;
        m_renderItem.objData.indexCount = m_pGeometry->GetIndicesCount();
    }
    
    void Geometry::SetPosition(gce::Vector3f32 const& pos)
    {
        m_transform.SetPosition(pos);
        m_isDirty = true;
    }

    void Geometry::SetRotation(gce::Vector3f32 const& rot)
    {
        m_transform.SetRotation(rot);
        m_isDirty = true;
    }

    void Geometry::SetScale(gce::Vector3f32 const& scale)
    {
        m_transform.SetScale(scale);
        m_isDirty = true;
    }

    void Geometry::Translate(gce::Vector3f32 const& translation)
    {
        m_transform.Translate(translation);
        m_isDirty = true;
    }

    void Geometry::Rotate(gce::Vector3f32 const& rotation)
    {
        m_transform.Rotate(rotation);
        m_isDirty = true;
    }

    void Geometry::Scale(gce::Vector3f32 const& scale)
    {
        m_transform.Scale(scale);
        m_isDirty = true;
    }

    gce::Vector3f32 Geometry::GetPosition()
    {
        return m_transform.position;
    }

    gce::Vector3f32 Geometry::GetRotation()
    {
        return m_transform.rotation;
    }

    gce::Vector3f32 Geometry::GetScale()
    {
        return m_transform.scale;
    }

    void Geometry::UpdateWorldMatrix()
    {
        m_transform.CalculateMatrix();
        m_renderItem.objData.world = m_transform.worldMatrix.GetTransposed();
        m_renderItem.numFramesDirty = gce::FrameResourcesManager::GetNumFrameResources();
    }

    gce::RenderItem& Geometry::GetRenderItem()
    {
        if (m_isDirty)
        {
            UpdateWorldMatrix();
            m_isDirty = false;
        }
        return m_renderItem;
    }

    void Geometry::SetTexture(Texture const& texture)
    {
        m_renderItem.objData.material.useTextureAlbedo = true;
        m_renderItem.objData.material.albedoTextureID = texture.m_textureID;
    }

    void Geometry::SetColor(gce::Vector3f32 const& color)
    {
        m_renderItem.objData.material.useTextureAlbedo = false;
        m_renderItem.objData.material.color = color;
    }
}
