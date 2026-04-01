#pragma once

#include "ShadowMap.h"

#include <Aliases.h>

#include <GCE/Render/Shader.h>
#include <GCE/Render/D12PipelineObject.h>

#define SHADER_PATH "res/Render/Shaders/"

namespace sr
{
    class Window;
    class Camera;

    enum Flag : int32;
    
    class Renderer
    {
    private:
        Renderer() = default;

        void Init(Window* pWindow);
        void Destroy();

        void Begin3D(Camera& camera);
        void End3D();

        void BeginShadow();
        void DrawShadowMap(gce::RenderItem& item);
        void EndShadow();
        
        void SetupDefaultLighting();
        void ResizeCameraProj();
        
    private:
        Window* m_pWindow = nullptr;
        Camera* m_pCurrentCamera = nullptr;
        ShadowMap* m_pShadowMap = nullptr;

        gce::Shader m_domainShader;
        gce::Shader m_hullShader; 
        gce::Shader m_pixelShader;  
        gce::Shader m_vertexShader; 
        gce::Shader m_rootSig; 

        inline static gce::D12PipelineObject* m_pPipelineStateObject = nullptr;

        friend class Window;
        friend class Geometry;
        friend class ShadowMap;
    };
}
