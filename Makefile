# Makefile for PokeControllerForPico

# --- Variables ---
# This allows for `make build pico` or `make pico`
# It finds the first non-`build`/`clean`/`setup-ci`/`help` target and uses it as the SKU
SKU := $(or $(filter-out build clean setup-ci help,$(MAKECMDGOALS)),pico)
SKU := $(firstword $(SKU))

# Map SKU to board name
ifeq ($(SKU), pico)
  BOARD = rpipico
else ifeq ($(SKU), pico2)
  BOARD = rpipico2
endif

.PHONY: all build setup-ci clean help pico pico2

# Default goal
.DEFAULT_GOAL := help

# --- Targets ---

# These targets exist so `make pico` works and make doesn't complain when `make build pico` is called
pico: build
pico2: build

help:
	@echo "Usage: make [target...]"
	@echo ""
	@echo "Examples:"
	@echo "  make                (shows this help)"
	@echo "  make build          (builds for default sku 'pico')"
	@echo "  make pico           (builds for sku 'pico')"
	@echo "  make build pico     (builds for sku 'pico')"
	@echo "  make pico2"
	@echo "  make build pico2"
	@echo ""
	@echo "Other targets:"
	@echo "  setup-ci          Install and configure Arduino CLI for CI environments."
	@echo "  clean             Remove build artifacts."
	@echo "  help              Show this help message."

setup-ci:
	@echo "--- Setting up Arduino environment for CI ---"
	arduino-cli config init
	arduino-cli config set board_manager.additional_urls https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
	arduino-cli core update-index
	arduino-cli core install rp2040:rp2040@4.6.1
	@echo "--- CI setup complete ---"

build:
ifeq ($(filter pico pico2,$(SKU)),)
	$(error Invalid SKU '$(SKU)'. Supported SKUs are: pico, pico2. Use 'make pico' or 'make build pico')
endif
	@echo "--- Building firmware for $(SKU) (board: $(BOARD)) ---"
	arduino-cli compile \
		--fqbn rp2040:rp2040:$(BOARD):usbstack=nousb \
		--build-property 'compiler.cpp.extra_flags=-DARDUINO_FQBN="{build.fqbn}"' \
		--export-binaries \
		--output-dir ./build \
		./PokeControllerForPico/
	@echo "--- Renaming artifact ---"
	mv ./build/PokeControllerForPico.ino.uf2 ./PokeControllerForPico-$(SKU).uf2
	@echo "--- Build successful: ./PokeControllerForPico-$(SKU).uf2 ---"

clean:
	@echo "--- Cleaning build artifacts ---"
	rm -rf ./build
	rm -f ./PokeControllerForPico-*.uf2
	@echo "--- Clean complete ---"
