#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include <efi.h>
#include <efilib.h>

EFI_FILE_PROTOCOL* open_file(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, CHAR16 *filename);

#endif
