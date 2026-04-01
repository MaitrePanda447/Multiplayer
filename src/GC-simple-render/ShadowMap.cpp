#include "ShadowMap.h"

#include <GCE/Render/FrameResources.h>
#include <GCE/Render/DescriptorAllocator.h>

#include "Renderer.h"
#include "Window.h"

using namespace gce;

namespace sr
{
    ShadowMap::ShadowMap(Renderer const& renderer)
    {

        m_psShadow.Load("res/Render/Shaders/Shadow.ps.cso");
        m_vsShadow.Load("res/Render/Shaders/Shadow.vs.cso");
        m_rootSigShadow.Load("res/Render/Shaders/Shadow.rs.cso");

        PSODescriptor desc = {};
        desc.type = SHADOW_MAP;
        m_pPsoShadow = new D12PipelineObject(m_vsShadow, m_psShadow, m_rootSigShadow, desc);

        FrameResourcesManager::Instance().SetShadowMap(&m_shadowMap);
        m_shadowMap.SetBias(0.005f, 0.02f);

        uint32 srvIndex = RenderContext::GetDescriptorAllocator()->Allocate();
        RenderTarget* renderTarget = renderer.m_pWindow->m_window.GetRenderTarget();
        
        m_shadowMap.BuildDescriptors(
            RenderContext::GetCpuDescriptorHandle(srvIndex),
            RenderContext::GetGpuDescriptorHandle(srvIndex),
            CD3DX12_CPU_DESCRIPTOR_HANDLE(renderTarget->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart(), 1 ,
                RenderContext::DsvDescriptorSize));

        m_shadowCam.SetNearPlane(0.1f);
        m_shadowCam.SetFarPlane(150.0f);
        m_shadowCam.SetViewWidth(80.0f);
        m_shadowCam.SetViewHeight(80.0f);

		Vector3f32 target = LIGHT_POS + LIGHT_DIR;
		Matrix shadowWorld = Matrix::LookAtLH(LIGHT_POS, target, {0.0f, 1.0f, 0.0f}).GetInverse();
        m_shadowCam.m_camera.SetMatrix(shadowWorld);
    }

    ShadowMap::~ShadowMap()
    {
        delete m_pPsoShadow;
    }
}
