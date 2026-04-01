#pragma once
#include <GCE/Render/Texture.h>

#include <Aliases.h>

namespace sr
{
   struct Texture : gce::Texture
   {
      Texture(std::string_view const path) : gce::Texture(path, gce::TextureType::ALBEDO) {}

      friend class Geometry;
   }; 
}
