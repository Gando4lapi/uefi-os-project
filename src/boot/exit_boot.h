#ifndef EXIT_BOOT_H
#define EXIT_BOOT_H

#include <efi.h>
#include <efilib.h>

EFI_STATUS exit_boot_services(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);

#endif
