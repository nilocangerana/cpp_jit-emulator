#pragma once


class ICPU{
    public:
        virtual ~ICPU() = default;

        virtual void* AllocateExecutableMemory(size_t size) = 0;

        virtual void Run() = 0;

        //Block size worst case: 32 instructions × ~20 bytes ≈ 640 bytes + safety margin
        const size_t MAX_BLOCK_BYTES = 1024;
        const int MAX_BLOCK_INSTRUCTIONS = 32;
};