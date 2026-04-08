#include <efi.h>
#include <efilib.h>

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
    Print(L"Press any key to exit...\r\n");

    // Ожидание нажатия любой клавиши
    EFI_INPUT_KEY Key;
    while (uefi_call_wrapper(SystemTable->ConIn->ReadKeyStroke, 2, SystemTable->ConIn, &Key) == EFI_NOT_READY) {
        // Пустой цикл ожидания
    }

    return EFI_SUCCESS;
}
