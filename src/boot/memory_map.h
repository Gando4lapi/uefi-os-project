#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <efi.h>
#include <efilib.h>

// Собственная структура для хранения данных области памяти
typedef struct {
    UINT32 type;
    UINT64 phys_start;
    UINT64 phys_end;
    UINT64 num_pages;
    UINT64 attribute;
} MY_MEMORY_REGION;

VOID print_memory_map(EFI_SYSTEM_TABLE *SystemTable);

#endif
