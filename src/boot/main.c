#include <efi.h>
#include <efilib.h>
#include "memory_map.h"
#include "file_loader.h"

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    // Инициализируем обертки gnu-efi
    InitializeLib(ImageHandle, SystemTable);

    // Очищаем экран (0 означает консоль, 1 означает очистку с установкой курсора в 0,0)
    uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);

    // Вывод текста. Строки в UEFI используют кодировку UTF-16, поэтому перед строкой ставится префикс L
    Print(L"Hello, UEFI World! Booting from Mac M3...\r\n");
    // Вывод карты памяти UEFI
    print_memory_map(SystemTable);

    // Демонстрация открытия файла (hello.txt пока не читаем, только открываем)
    EFI_FILE_PROTOCOL *demoFile = open_file(ImageHandle, SystemTable, L"hello.txt");
    if (demoFile) {
        Print(L"File hello.txt opened successfully!\r\n");
        // Пока просто закрываем, без чтения
        uefi_call_wrapper(demoFile->Close, 1, demoFile);
    }

    Print(L"Press any key to exit...\r\n");

    // Ожидание нажатия любой клавиши
    EFI_INPUT_KEY Key;
    while (uefi_call_wrapper(SystemTable->ConIn->ReadKeyStroke, 2, SystemTable->ConIn, &Key) == EFI_NOT_READY) {
        // Пустой цикл ожидания
    }

    return EFI_SUCCESS;
}
