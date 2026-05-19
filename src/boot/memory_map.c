#include "memory_map.h"

// Таблица строковых представлений типов памяти (определены в UEFI Spec)
static CHAR16 *memory_type_to_str(EFI_MEMORY_TYPE type) {
    switch (type) {
        case EfiReservedMemoryType:   return L"Reserved";
        case EfiLoaderCode:           return L"LoaderCode";
        case EfiLoaderData:           return L"LoaderData";
        case EfiBootServicesCode:     return L"BootCode";
        case EfiBootServicesData:     return L"BootData";
        case EfiRuntimeServicesCode:  return L"RuntimeCode";
        case EfiRuntimeServicesData:  return L"RuntimeData";
        case EfiConventionalMemory:   return L"Conventional";
        case EfiUnusableMemory:       return L"Unusable";
        case EfiACPIReclaimMemory:    return L"ACPI Reclaim";
        case EfiACPIMemoryNVS:        return L"ACPI NVS";
        case EfiMemoryMappedIO:       return L"MMIO";
        case EfiMemoryMappedIOPortSpace: return L"MMIO Port";
        case EfiPalCode:              return L"PAL Code";
        default:                      return L"Unknown";
    }
}

VOID print_memory_map(EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS status;
    UINTN mapSize = 0, mapKey, descriptorSize;
    UINT32 descriptorVersion;
    EFI_MEMORY_DESCRIPTOR *memoryMap = NULL;

    // Первый вызов для получения размера буфера
    status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
                               &mapSize, NULL, &mapKey, &descriptorSize, &descriptorVersion);
    if (status != EFI_BUFFER_TOO_SMALL) {
        Print(L"Error: GetMemoryMap sizing failed: %r\n", status);
        return;
    }

    // Небольшой запас на случай изменения карты между вызовами
    mapSize += descriptorSize * 2;
    status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                               EfiBootServicesData, mapSize, (VOID **)&memoryMap);
    if (EFI_ERROR(status)) {
        Print(L"Error: AllocatePool for memory map failed: %r\n", status);
        return;
    }

    // Второй вызов — получение карты
    status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
                               &mapSize, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
    if (EFI_ERROR(status)) {
        Print(L"Error: GetMemoryMap failed: %r\n", status);
        uefi_call_wrapper(SystemTable->BootServices->FreePool, 1, memoryMap);
        return;
    }

    UINTN numDescriptors = mapSize / descriptorSize;
    EFI_MEMORY_DESCRIPTOR *desc;

    Print(L"\n--- UEFI Memory Map ---\n");
    Print(L"Type                 PhysStart           PhysEnd             Pages       Attr\n");

    for (UINTN i = 0; i < numDescriptors; i++) {
        desc = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)memoryMap + i * descriptorSize);

        // Перепаковка в собственную структуру
        MY_MEMORY_REGION region;
        region.type       = desc->Type;
        region.phys_start = desc->PhysicalStart;
        region.num_pages  = desc->NumberOfPages;
        region.attribute  = desc->Attribute;
        region.phys_end   = region.phys_start + (region.num_pages * 4096) - 1;

        Print(L"%-20s 0x%016llx-0x%016llx %10lld KiB 0x%016llx\n",
              memory_type_to_str(region.type),
              region.phys_start,
              region.phys_end,
              (region.num_pages * 4096) / 1024,
              region.attribute);
    }

    uefi_call_wrapper(SystemTable->BootServices->FreePool, 1, memoryMap);
}
