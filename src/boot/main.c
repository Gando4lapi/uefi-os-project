#include <efi.h>
#include <efilib.h>
#include "memory_map.h"
#include "file_loader.h"
#include "exit_boot.h"

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    // Инициализируем обертки gnu-efi
    InitializeLib(ImageHandle, SystemTable);

    // Очищаем экран
    uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

    // Вывод текста
    Print(L"Hello, UEFI World! Booting from Mac M3...\r\n");
    // Вывод карты памяти UEFI
    print_memory_map(SystemTable);

    // Загрузка файла hello.txt и вывод его содержимого
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
                    CHAR16 wch[2];
                    wch[0] = ch;
                    wch[1] = 0;
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

    // Ожидание нажатия клавиши
    EFI_INPUT_KEY Key;
    uefi_call_wrapper(SystemTable->ConIn->Reset, 2, SystemTable->ConIn, FALSE);
    while (uefi_call_wrapper(SystemTable->ConIn->ReadKeyStroke, 2, SystemTable->ConIn, &Key) == EFI_NOT_READY) {
        // ожидание
    }

    // Выход из Boot Services
    exit_boot_services(ImageHandle, SystemTable);

    // Сюда управление не вернётся
    return EFI_SUCCESS;
}
