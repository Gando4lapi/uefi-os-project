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

EFI_STATUS load_file_to_memory(EFI_FILE_PROTOCOL *file, VOID **buffer, UINTN *size, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS status;
    UINTN infoSize = 0;
    EFI_FILE_INFO *fileInfo = NULL;

    // Получаем размер структуры EFI_FILE_INFO
    status = uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &infoSize, NULL);
    if (status != EFI_BUFFER_TOO_SMALL) {
        Print(L"Error: GetInfo sizing failed: %r\n", status);
        return status;
    }

    status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                               EfiBootServicesData, infoSize, (VOID **)&fileInfo);
    if (EFI_ERROR(status)) {
        Print(L"Error: AllocatePool for file info failed: %r\n", status);
        return status;
    }

    status = uefi_call_wrapper(file->GetInfo, 4, file, &gEfiFileInfoGuid, &infoSize, fileInfo);
    if (EFI_ERROR(status)) {
        Print(L"Error: GetInfo failed: %r\n", status);
        uefi_call_wrapper(SystemTable->BootServices->FreePool, 1, fileInfo);
        return status;
    }

    *size = (UINTN)fileInfo->FileSize;
    uefi_call_wrapper(SystemTable->BootServices->FreePool, 1, fileInfo);

    // Выделяем буфер
    status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3,
                               EfiBootServicesData, *size, buffer);
    if (EFI_ERROR(status)) {
        Print(L"Error: AllocatePool for file buffer failed: %r\n", status);
        return status;
    }

    // Читаем файл
    status = uefi_call_wrapper(file->Read, 3, file, size, *buffer);
    if (EFI_ERROR(status)) {
        Print(L"Error: Read file failed: %r\n", status);
        uefi_call_wrapper(SystemTable->BootServices->FreePool, 1, *buffer);
        *buffer = NULL;
        return status;
    }

    return EFI_SUCCESS;
}
