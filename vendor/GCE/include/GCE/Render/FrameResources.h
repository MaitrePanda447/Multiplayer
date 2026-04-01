#pragma once
#include "Structs.h"
#include "DynamicBuffer.h"
#include "../Core/Containers/FreeList.hpp"
#include "RenderTarget.h"
#include <queue>

#include "ShadowCubeMap.h"
#include "ParticlesSystem.h"

namespace gce
{
    struct FrameResource
    {
    public:
        FrameResource();
        ~FrameResource();

        DynamicBuffer* m_pPassCB = nullptr;
        DynamicBuffer* m_pLightsCB = nullptr;
        DynamicBuffer* m_pDebugLinesCB = nullptr;
        DynamicBuffer* m_pShadowPassCB = nullptr;
        DynamicBuffer* m_pShadowDataCB = nullptr;
        DynamicBuffer* m_pShadowCubePassCB = nullptr;
        DynamicBuffer* m_pShadowCubeDataCB = nullptr;
        Vector<DynamicBuffer*> m_pParticlesCB;
        Vector<DynamicBuffer*> m_pObjDatasCB;

        ID3D12CommandAllocator* m_pCommandAllocator = nullptr;

    private:
        uint64 m_fenceValue = 0;

        friend class FrameResourcesManager;
    };

    class FrameResourcesManager
    {
    public:
        FrameResourcesManager();
        FrameResourcesManager(const FrameResourcesManager&) = delete;

        FrameResourcesManager& operator=(FrameResourcesManager const& other) = delete;
        FrameResourcesManager& operator=(FrameResourcesManager&& other) noexcept = delete;

        ~FrameResourcesManager();

        void BeginFrame();
        void EndFrame();

        static void AddRenderItem(RenderItem& item);
        static void AddParticlesEmitter(ParticlesEmitter& particulesEmitter);
        void RemoveRenderItem(RenderItem& item);
        void RemoveParticlesEmitter(ParticlesEmitter& particulesEmitter);
        void Flush();

        static FrameResource* GetCurrentFrameResource();
        static constexpr int32 GetNumFrameResources() { return s_numFrameResources; }
        static ID3D12GraphicsCommandList* GetCmdList() { return Instance().m_pCmdList; }
        static ID3D12CommandQueue* GetCmdQueue() { return Instance().m_pCmdQueue; }
        static ShadowMap* GetShadowMap() { return Instance().m_pShadowMap; }
        static ShadowCubeMap* GetShadowCubeMap() { return Instance().m_pShadowCubeMap; }
        void SetShadowMap(ShadowMap* sdm) { m_pShadowMap = sdm; }
        void SetShadowCubeMap(ShadowCubeMap* scdm) { m_pShadowCubeMap = scdm; }

        void UpdateObjCBuffer();
        void UpdateParticlesCBuffer();


        static FrameResourcesManager& Instance()
        {
            static FrameResourcesManager fr;
            return fr;
        }

    protected:
        static const int32 s_numFrameResources = 3;
        int32 m_currentFrameIndex = 0;
        Vector<FrameResource> m_frameResources;

        Vector<RenderItem*> m_pAllRitems = {};
        Vector<ParticlesEmitter*> m_pAllParticleEmitter = {};

        ID3D12CommandQueue* m_pCmdQueue = nullptr;
        ID3D12GraphicsCommandList* m_pCmdList = nullptr;

        ID3D12Fence* m_pFence = nullptr;
        uint64 m_currentFenceValue = 0;

        ShadowMap* m_pShadowMap = nullptr;
        ShadowCubeMap* m_pShadowCubeMap = nullptr;

        friend class FrameResource;

        friend class RenderTarget;
        friend class ShadowMap;
        friend class ShadowCubeMap;
        friend class LightManager;
        friend class ParticlesEmitter;
    };
}
