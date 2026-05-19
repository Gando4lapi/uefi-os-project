#include "file_loader.h"

EFI_FILE_PROTOCOL* open_file(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable, CHAR16 *filename) {
    EFI_STATUS status;
    EFI_GUID sfspGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfsp = NULL;
    EFI_FILE_PROTOCOL *root = NULL, *file = NULL;

    // 1. LocateProtocol для поиска файловой системы
    status = uefi_call_wrapper(SystemTable->BootServices->LocateProtocol, 3,
                               &sfspGuid, NULL, (VOID **)&sfsp);
    if (EFI_ERROR(status) || sfsp == NULL) {
        Print(L"Error: Simple File System Protocol not found: %r\n", status);
        return NULL;
    }

    // 2. Открытие тома (корневого каталога)
    status = uefi_call_wrapper(sfsp->OpenVolume, 2, sfsp, &root);
    if (EFI_ERROR(status) || root == NULL) {
        Print(L"Error: Cannot open volume: %r\n", status);
        return NULL;
    }

    // 3. Открытие файла
    status = uefi_call_wrapper(root->Open, 5, root, &file, filename,
                               EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (EFI_ERROR(status) || file == NULL) {
        Print(L"Error: Cannot open file '%s': %r\n", filename, status);
        uefi_call_wrapper(root->Close, 1, root);
        return NULL;
    }

    // Закрываем корневой каталог, он больше не нужен
    uefi_call_wrapper(root->Close, 1, root);

    return file;
}
