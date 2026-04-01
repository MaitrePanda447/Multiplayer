#ifndef DRIVER_PRIORITY_H
#define DRIVER_PRIORITY_H

extern "C" {
    typedef unsigned long DWORD;

	// NVIDIA: Choose high-performance GPU
    __declspec(selectany) __declspec(dllexport)
    DWORD NvOptimusEnablement = 0x00000001;

	// AMD: Choose high-performance GPU
    __declspec(selectany) __declspec(dllexport)
    int AmdPowerXpressRequestHighPerformance = 1;
}

#endif