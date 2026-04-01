#include "Renderer.h"

#include "Camera.h"
#include "Window.h"

#include <GCE/Render/LightManager.h>
#include <GCE/Render/FrameResources.h>

using namespace gce;

namespace sr
{
    void Renderer::Init(Window* pWindow)
    {
        m_pWindow = pWindow;
        
        m_domainShader.Load(std::string(SHADER_PATH) + "Lit.ds.cso");
        m_hullShader.Load(std::string(SHADER_PATH) + "Lit.hs.cso");
        m_pixelShader.Load(std::string(SHADER_PATH) + "Lit.ps.cso");
        m_vertexShader.Load(std::string(SHADER_PATH) + "Lit.vs.cso");
        m_rootSig.Load(std::string(SHADER_PATH) + "Lit.rs.cso");

        PSODescriptor desc = {};
        desc.type = DEFAULT;
        m_pPipelineStateObject = new D12PipelineObject(m_vertexShader, m_pixelShader, m_hullShader, m_domainShader, m_rootSig, desc);

        SetupDefaultLighting();

        m_pShadowMap = new ShadowMap(*this);
    }

    void Renderer::Destroy()
    {
        delete m_pShadowMap;
        delete m_pPipelineStateObject;
    }

    void Renderer::SetupDefaultLighting()
    {
        LightManager::SetLightsProperties(8.0f, 100.0f, 2.0f, 32.0f,0.1f);
        LightData directionalLight = LightManager::CreateDirectionalLight(LIGHT_DIR, Vector4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 1.0f);
        LightManager::AddLight(directionalLight); 
    }

    void Renderer::ResizeCameraProj()
    {
        switch (m_pCurrentCamera->m_type)
        {
        case PERSPECTIVE:
            m_pCurrentCamera->SetAspectRatio(m_pWindow->m_window.GetAspectRatio());
            m_pCurrentCamera->UpdateWorldMatrix();
            break;
        case ORTHOGRAPHIC:
            m_pCurrentCamera->SetViewWidth(static_cast<float32>(m_pWindow->m_width));
            m_pCurrentCamera->SetViewHeight(static_cast<float32>(m_pWindow->m_height));
            break;
        }
    }

    void Renderer::Begin3D(Camera& camera)
    {
        static bool firstSet = true;
        m_pCurrentCamera = &camera;
        if (firstSet)
        {
            ResizeCameraProj();
            firstSet = false;
        }
        FrameResourcesManager::Instance().BeginFrame();
        FrameResourcesManager::Instance().UpdateObjCBuffer();

        LightManager::UpdateLightCB();
    }
    
    void Renderer::BeginShadow()
    {
        m_pShadowMap->m_shadowMap.BeginShadow();
        
        m_pShadowMap->m_shadowMap.SetLightCamera(m_pShadowMap->m_shadowCam.m_camera, {0.0f, 10.0f, -10.0f}, {0.0f, -1.0f, 1.0f});
    }

    void Renderer::DrawShadowMap(RenderItem& item)
    {
        m_pShadowMap->m_shadowMap.DrawShadowMap(item, *m_pShadowMap->m_pPsoShadow);
    }

    void Renderer::EndShadow()
    {
        m_pShadowMap->m_shadowMap.EndShadow();
        ShadowData shadowData = m_pShadowMap->m_shadowMap.GetShadowData();
        FrameResourcesManager::GetCurrentFrameResource()->m_pShadowDataCB->CopyData(shadowData);
    }

    void Renderer::End3D()
    {
        FrameResourcesManager::Instance().EndFrame();
    }

}
