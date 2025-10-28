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

# This allows for `make build pico` or `make pico`
# It finds the first non-`build`/`clean`/`help` target and uses it as the SKU
SKU := $(or $(filter-out build clean help,$(MAKECMDGOALS)),pico)
SKU := $(firstword $(SKU))

# Map SKU to board name
ifeq ($(SKU), pico)
  BOARD = pico
else ifeq ($(SKU), pico2)
  BOARD = pico2
else ifeq ($(SKU), waveshare_rp2350_usb_a)
  BOARD = waveshare_rp2350_usb_a
endif

# Name of the build directory
BUILD_DIR = build

# Name of the final artifact
TARGET = PokeControllerForPico
TARGET_UF2 = $(TARGET).uf2
SKU_TARGET_UF2 = $(TARGET)-$(SKU).uf2


# --- Targets ---
.PHONY: all build clean help pico pico2 waveshare_rp2350_usb_a

# Default goal
all: build

# These targets exist for compatibility with the old Makefile
pico: build
pico2: build
waveshare_rp2350_usb_a: build

help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all/build                Build the firmware (default)."
	@echo "  pico                     Build for Raspberry Pi Pico."
	@echo "  pico2                    Build for Raspberry Pi Pico (same as pico)."
	@echo "  waveshare_rp2350_usb_a   Build for Waveshare RP2350 USB-A."
	@echo "  clean                    Remove build artifacts."
	@echo "  help              Show this help message."
	@echo ""
	@echo "Note: Ensure PICO_SDK_PATH is set in your environment."


build:
	@if [ -z "$(PICO_SDK_PATH)" ]; then \
		echo "Error: PICO_SDK_PATH is not set." >&2; \
		echo "Please either set the PICO_SDK_PATH environment variable to your Pico SDK directory," >&2; \
		echo "or install ghq and place the SDK in your ghq root (e.g., \`ghq get raspberrypi/pico-sdk\`)." >&2; \
		exit 1; \
	fi
	@echo "--- Configuring build with CMake ---"
	mkdir -p $(BUILD_DIR)/$(SKU)
	@echo "PICO_SDK_PATH is $(PICO_SDK_PATH)"
	cmake -B $(BUILD_DIR)/$(SKU) -S . -DPICO_BOARD=$(BOARD)
	@echo "--- Building firmware with make ---"
	$(MAKE) -C $(BUILD_DIR)/$(SKU)
	@echo "--- Renaming artifact ---"
	mv $(BUILD_DIR)/$(SKU)/$(TARGET_UF2) $(BUILD_DIR)/$(SKU_TARGET_UF2)
	@echo "--- Build successful: $(BUILD_DIR)/$(SKU_TARGET_UF2) ---"


clean:
	@echo "--- Cleaning build artifacts ---"
	rm -rf $(BUILD_DIR)
	@echo "--- Clean complete ---"
