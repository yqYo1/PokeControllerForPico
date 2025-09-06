# Makefile for PokeControllerForPico with Pico SDK

# --- Variables ---
# The path to the Pico SDK. It's recommended to set this in your environment
# export PICO_SDK_PATH=/path/to/pico-sdk
# PICO_SDK_PATH ?= ../../pico-sdk

# Name of the build directory
BUILD_DIR = build

# Name of the final artifact
TARGET = PokeControllerForPico
TARGET_UF2 = $(TARGET).uf2

# --- Targets ---
.PHONY: all build clean help pico pico2

# Default goal
all: build

# These targets exist for compatibility with the old Makefile
pico: build
pico2: build

help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all/build         Build the firmware (default)."
	@echo "  pico              Build for Raspberry Pi Pico."
	@echo "  pico2             Build for Raspberry Pi Pico (same as pico)."
	@echo "  clean             Remove build artifacts."
	@echo "  help              Show this help message."
	@echo ""
	@echo "Note: Ensure PICO_SDK_PATH is set in your environment."


build:
	@echo "--- Configuring build with CMake ---"
	mkdir -p $(BUILD_DIR)
	@echo "PICO_SDK_PATH is $(PICO_SDK_PATH)"
	cmake -B $(BUILD_DIR) -S .
	@echo "--- Building firmware with make ---"
	$(MAKE) -C $(BUILD_DIR)
	@echo "--- Copying artifact ---"
	cp $(BUILD_DIR)/$(TARGET_UF2) .
	@echo "--- Build successful: ./$(TARGET_UF2) ---"


clean:
	@echo "--- Cleaning build artifacts ---"
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET_UF2)
	@echo "--- Clean complete ---"
