#ifndef ENGINE_DESCRIPTOR_ALLOCATOR_H_INCLUDED
#define ENGINE_DESCRIPTOR_ALLOCATOR_H_INCLUDED

#include "define.h"

namespace gce 
{
    constexpr UINT INVALID_DESCRIPTOR = 0xFFFFFFFF;

	class DescriptorAllocator
	{
    public:
        DescriptorAllocator(uint64 maxSlots);
        ~DescriptorAllocator();

        uint32 Allocate();
        void Free(UINT index);

    private:
        void SetBit(UINT index);
        void ClearBit(UINT index);
        bool TestBit(UINT index);

        uint64 m_maxSlots;
        uint64 m_bitmapSize;
        Vector<uint64> m_bitmap;
        Vector<uint32> m_freeList;
	};
}

#endif