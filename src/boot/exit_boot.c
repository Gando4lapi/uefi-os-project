#include "exit_boot.h"

EFI_STATUS exit_boot_services(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS status;
    UINTN mapSize = 0, mapKey, descriptorSize;
    UINT32 descriptorVersion;
    EFI_MEMORY_DESCRIPTOR *memoryMap = NULL;

    status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
                               &mapSize, NULL, &mapKey, &descriptorSize, &descriptorVersion);
    if (status != EFI_BUFFER_TOO_SMALL) {
        Print(L"Error: GetMemoryMap sizing failed: %r\n", status);
        return status;
    }

    mapSize += descriptorSize * 2;
    status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                               EfiBootServicesData, mapSize, (VOID **)&memoryMap);
    if (EFI_ERROR(status)) {
        Print(L"Error: AllocatePool failed: %r\n", status);
        return status;
    }

    status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
                               &mapSize, memoryMap, &mapKey, &descriptorSize, &descriptorVersion);
    if (EFI_ERROR(status)) {
        Print(L"Error: GetMemoryMap failed: %r\n", status);
        uefi_call_wrapper(SystemTable->BootServices->FreePool, 1, memoryMap);
        return status;
    }

    status = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2,
                               ImageHandle, mapKey);

    CHAR16 *vga = (CHAR16 *)0xB8000;
    if (!EFI_ERROR(status)) {
        CHAR16 *msg = L"ExitBootServices succeeded!";
        for (int i = 0; msg[i] != 0; i++) vga[i] = msg[i] | 0x0700;
        return EFI_SUCCESS;  // управление вернётся
    } else {
        CHAR16 *msg = L"ExitBootServices failed!";
        for (int i = 0; msg[i] != 0; i++) vga[i] = msg[i] | 0x0400;
        while (1) { __asm__ volatile("hlt"); }
        return status;
    }
}
