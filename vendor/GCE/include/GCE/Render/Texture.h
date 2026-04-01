#ifndef RENDER_TEXTURE_H_INCLUDED
#define RENDER_TEXTURE_H_INCLUDED

#include "StaticBuffer.h"

namespace gce
{
	class D12ComputePipelineObject;
	class D12PipelineObject;
	struct ImageData;
	class MipmapGenerator;



	enum class TextureType : uint32
	{
		ALBEDO = 0,      
		NORMAL = 1,      
		ROUGHNESS = 2,    
		METALNESS = 3,    
		AMBIENT = 4,
		DISPLACEMENT = 5
	};

	class Texture
	{
	public:
		Texture() = default;
		Texture(std::string_view path);
		Texture(std::string_view path, TextureType type);
		Texture(Texture const& other) = delete;
		Texture(Texture&& other) = delete;

		Texture& operator=(Texture const& other) = delete;
		Texture& operator=(Texture&& other) = delete;

		virtual ~Texture() = default;

		bool NeedsMipmapGeneration() const { return m_hasMipmaps && !m_mipmapsGenerated; };
		void GenerateMipmapsDeferred();

		virtual void Create(std::string_view path);
		virtual void Create(std::string_view path, TextureType type);
		virtual void Create3D(std::string_view path);
		uint32 CalculateMipLevels(uint32 width, uint32 height);

		void GenerateMipmapsTex();
		D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() const;
		static void InitializeMipmapGenerator(D12ComputePipelineObject* computePSO, D12PipelineObject* rootSig);
		static void CleanupMipmapGenerator();

		static bool IsFormatSuitableForMipmaps(DXGI_FORMAT format);
		static TextureType DetectTextureTypeFromPath(std::string_view path);

		void SetID();

		uint32 GetWidth() const { return m_width; }
		uint32 GetHeight() const { return m_height; }
		uint32 GetTextureID() const { return m_textureID; }
		StaticBuffer* GetBufferTexture() { return &m_textureBuffer; }

		static MipmapGenerator* m_mipmapGenerator;

	protected:
		StaticBuffer m_textureBuffer;

		uint32 m_textureID = -1;
		uint32 m_width = 0;
		uint32 m_height = 0;
		TextureType m_textureType = TextureType::ALBEDO;

		bool m_hasMipmaps = false;
		bool m_mipmapsGenerated = false;

		friend class MipmapGenerator;
	};
}

#endif