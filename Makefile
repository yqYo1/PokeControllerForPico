# Makefile for PokeControllerForPico with Pico SDK

# --- Variables ---
# The path to the Pico SDK. It's recommended to set this in your environment
# export PICO_SDK_PATH=/path/to/pico-sdk
# PICO_SDK_PATH ?= ../../pico-sdk

# --- PICO_SDK_PATH auto-detection ---
# If PICO_SDK_PATH is not set, try to find it with ghq
ifndef PICO_SDK_PATH
  # Check if ghq is installed
  ifneq (, $(shell which ghq))
    $(info ghq found)
    GHQ_ROOT := $(shell ghq root)
    # Check if pico-sdk exists in the ghq root
    ifneq (, $(wildcard $(GHQ_ROOT)/github.com/raspberrypi/pico-sdk))
      export PICO_SDK_PATH := $(GHQ_ROOT)/github.com/raspberrypi/pico-sdk
      # Use an info message to let the user know we found it
      $(info PICO_SDK_PATH not set, using ghq path: $(PICO_SDK_PATH))
    endif
  endif
endif
# ---

# --- SKUs ---
# List of all SKUs to build with `make all`
ALL_SKUS = pico pico2 waveshare_rp2040_zero

# Name of the build directory
BUILD_DIR = build

# Name of the final artifact
TARGET = PokeControllerForPico
TARGET_UF2 = $(TARGET).uf2


# --- Targets ---
.PHONY: all build clean help $(ALL_SKUS)

# Default goal
all: $(ALL_SKUS)

# 'build' is an alias for 'all'
build: all

help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all/build         Build all firmware variants (pico, pico2, waveshare_rp2040_zero) (default)."
	@echo "  pico              Build for Raspberry Pi Pico."
	@echo "  pico2             Build for Raspberry Pi Pico 2."
	@echo "  waveshare_rp2040_zero Build for Waveshare RP2040-Zero."
	@echo "  clean             Remove build artifacts."
	@echo "  help              Show this help message."
	@echo ""
	@echo "Note: Ensure PICO_SDK_PATH is set in your environment."


# Build rule for each SKU
$(ALL_SKUS):
	@if [ -z "$(PICO_SDK_PATH)" ]; then \
		echo "Error: PICO_SDK_PATH is not set." >&2; \
		echo "Please either set the PICO_SDK_PATH environment variable to your Pico SDK directory," >&2; \
		echo "or install ghq and place the SDK in your ghq root (e.g., \`ghq get raspberrypi/pico-sdk\`)." >&2; \
		exit 1; \
	fi
	@echo "--- Building SKU: $@ ---"
	$(eval BOARD := $@)
	@echo "--- Configuring build with CMake for board $(BOARD) ---"
	mkdir -p $(BUILD_DIR)/$@
	@echo "PICO_SDK_PATH is $(PICO_SDK_PATH)"
	cmake -B $(BUILD_DIR)/$@ -S . -DPICO_BOARD=$(BOARD)
	@echo "--- Building firmware with make ---"
	$(MAKE) -C $(BUILD_DIR)/$@
	@echo "--- Renaming artifact ---"
	mv $(BUILD_DIR)/$@/$(TARGET_UF2) $(BUILD_DIR)/$(TARGET)-$@.uf2
	@echo "--- Build successful: $(BUILD_DIR)/$(TARGET)-$@.uf2 ---"


clean:
	@echo "--- Cleaning build artifacts ---"
	rm -rf $(BUILD_DIR)
	@echo "--- Clean complete ---"