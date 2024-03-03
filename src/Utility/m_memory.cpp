#include "m_memory.h"

void m_memcpy(void* dest, const void* src, uint32_t count)
{
    for(uint32_t i = 0; i < count; i++)
        ((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
}