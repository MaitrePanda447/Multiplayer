#include "Light.h"
#include <GCE/Render/LightManager.h>

void sr::Light::SetColor(gce::Vector4 color)
{
    m_Light.color = color;
}

sr::Light sr::Light::CreatePointLight(gce::Vector3f32 const& position, gce::Vector4 const& color, float32 range, float32 intensity, float32 strength)
{
    gce::LightData data = gce::LightManager::CreatePointLight(position, color, range, intensity, strength);
    return Light(data);
}

sr::Light sr::Light::CreateSpotLight(gce::Vector3f32 const& position, gce::Vector3f32 const& direction, gce::Vector4 const& color, float32 spotAngle, float32 penumbraAngle, float32 range, float32 intensity, float32 strength)
{
    gce::LightData data = gce::LightManager::CreateSpotLight(position, direction, color, spotAngle, penumbraAngle, range, intensity, strength);
    return Light(data);
}

sr::Light sr::Light::CreateDirectionalLight(gce::Vector3f32 const& direction, gce::Vector4 const& color, float32 intensity, float32 strength)
{
    gce::LightData data = gce::LightManager::CreateDirectionalLight(direction, color, intensity, strength);
    return Light(data);
}

void sr::Light::AddLight(Light& light)
{
    gce::LightManager::AddLight(light.m_Light);
}

void sr::Light::RemoveLight(Light& light)
{
    gce::LightManager::RemoveLight(light.m_Light.id);
}
