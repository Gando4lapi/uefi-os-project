#include <stdint.h>

void kernel_main(void) {
    uint16_t *vga = (uint16_t *)0xB8000;
    // Сместимся на 4 строки (80 колонок × 4)
    vga += 80 * 4;

    // "Hello World from Kernel!" вручную как массив 16-битных символов + атрибут 0x0F (яркий белый на чёрном)
    const char *text = "Hello World from Kernel!";
    for (int i = 0; text[i] != '\0'; i++) {
        vga[i] = (uint16_t)text[i] | 0x0F00;
    }

    // Бесконечный цикл
    while (1) {
        __asm__ volatile("hlt");
    }
}
