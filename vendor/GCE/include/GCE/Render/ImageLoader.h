#ifndef RENDER_IMAGE_LOADER_H_INCLUDED
#define RENDER_IMAGE_LOADER_H_INCLUDED

#include <filesystem>
#include <wincodec.h>
#include <vector>

#include "define.h"

#define __ImageLoader_CAR(expr) do {if (FAILED(expr)){return false; }} while (false)

namespace gce
{
	class StaticBuffer;

	struct ImageData
    {
        ImageData() = default;

        std::vector<int8> data;
        uint32 width = 0;
        uint32 height = 0;
        uint32 bitsPerPixel = 0;
        uint32 chanelCount = 0;
        uint32 textureStride = 0;
        uint32 textureSize = 0;

        GUID pixelFormat = {};
        DXGI_FORMAT giPixelFormat = {};

        uint32 mipLevels = 1;
        D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE;
    };

    class ImageLoader
    {
    public:

        ImageLoader(const ImageLoader&) = delete;
        ImageLoader(ImageLoader&& other) noexcept = delete;

        ImageLoader& operator=(ImageLoader const&) = delete;
        ImageLoader& operator=(ImageLoader&&) = delete;

        static ImageData LoadTexture(std::filesystem::path const& imagePath);
        static void LoadBitMap(std::filesystem::path const& imagePath, ID2D1Bitmap** ppBitMap);
        static uint32 GetBytesPerPixel(DXGI_FORMAT format);
        static size_t CalculateTotalMipmapSize(uint32 width, uint32 height, uint32 bytesPerPixel, uint32 mipLevels);

    private:
        static void CopyTextureRegion(ID3D12GraphicsCommandList* cmdList, StaticBuffer const* pBuffer, D3D12_PLACED_SUBRESOURCE_FOOTPRINT const& footprint, uint64 offset = 0);
     
        ImageLoader() = default;
        ~ImageLoader() = default;

    private:
        friend class RenderContext;
    };
}
#endif