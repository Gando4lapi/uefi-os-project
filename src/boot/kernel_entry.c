void kernel_main(void) {
    CHAR16 *vga = (CHAR16 *)0xB8000;
    // Смещение на 4 строки, чтобы не затереть сообщение от ExitBootServices
    vga += 80 * 4;
    CHAR16 *msg = L"Hello World from Kernel!";
    for (int i = 0; msg[i] != 0; i++) {
        vga[i] = msg[i] | 0x0F00; // ярко-белый на чёрном
    }
    while (1) { __asm__ volatile("hlt"); }
}
