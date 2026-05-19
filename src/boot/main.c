#include <efi.h>
#include <efilib.h>
#include "memory_map.h"
#include "file_loader.h"
#include "exit_boot.h"

extern void kernel_main(void);

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

    Print(L"Hello, UEFI World! Booting from Mac M3...\r\n");
    print_memory_map(SystemTable);

    EFI_FILE_PROTOCOL *file = open_file(ImageHandle, SystemTable, L"hello.txt");
    if (file) {
        VOID *buffer = NULL;
        UINTN size = 0;
        EFI_STATUS fstatus = load_file_to_memory(file, &buffer, &size, SystemTable);
        if (!EFI_ERROR(fstatus) && buffer) {
            Print(L"\n--- Contents of hello.txt ---\r\n");
            for (UINTN i = 0; i < size; i++) {
                CHAR8 ch = ((CHAR8 *)buffer)[i];
                if (ch >= 32 && ch < 127) {
                    CHAR16 wch[2] = {ch, 0};
                    Print(L"%s", wch);
                } else if (ch == '\n') {
                    Print(L"\r\n");
                }
            }
            Print(L"\n--- End of file ---\r\n");
            uefi_call_wrapper(SystemTable->BootServices->FreePool, 1, buffer);
        }
        uefi_call_wrapper(file->Close, 1, file);
    }

    Print(L"Press any key to exit UEFI Boot Services...\r\n");

    EFI_INPUT_KEY Key;
    uefi_call_wrapper(SystemTable->ConIn->Reset, 2, SystemTable->ConIn, FALSE);
    while (uefi_call_wrapper(SystemTable->ConIn->ReadKeyStroke, 2, SystemTable->ConIn, &Key) == EFI_NOT_READY);

    EFI_STATUS status = exit_boot_services(ImageHandle, SystemTable);
    if (!EFI_ERROR(status)) {
        kernel_main();
    }

    return EFI_SUCCESS;
}
