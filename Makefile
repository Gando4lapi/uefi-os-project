# Выполняется на хосте (Mac M3)
.PHONY: all build-env build run clean

IMAGE_NAME = uefi-builder
WORKSPACE = $(shell pwd)

# Путь к OVMF (UEFI прошивке) в Homebrew на Apple Silicon
OVMF_FD = /opt/homebrew/share/qemu/edk2-x86_64-code.fd

all: build run

# Собираем Docker-образ, если он еще не собран
build-env:
	docker build -t $(IMAGE_NAME) .

# Запускаем сборку внутри контейнера
build: build-env
	docker run --rm -v $(WORKSPACE):/workspace $(IMAGE_NAME) make -f Makefile.uefi

# Запускаем QEMU на Mac
run: build
	@if [ ! -f $(OVMF_FD) ]; then \
		echo "Ошибка: OVMF не найден по пути $(OVMF_FD). Убедись, что выполнил 'brew install ovmf'."; \
		exit 1; \
	fi
	@echo "==> Запуск QEMU..."
	qemu-system-x86_64 \
		-machine q35 \
		-drive if=pflash,format=raw,readonly=on,file=$(OVMF_FD) \
		-drive format=raw,file=fat:rw:fat_disk \
		-m 512M \
		-vga std \
		-serial stdio

clean:
	rm -rf build/ fat_disk/
