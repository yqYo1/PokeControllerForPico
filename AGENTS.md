# Agent Instructions for PokeControllerForPico

This document contains important information for AI agents working on this repository. Please read this before making any changes.

## 1. Critical: .gitignore Configuration

This repository uses a non-standard `.gitignore` configuration that **ignores all files by default**.

The pattern is:
1.  Ignore everything with `/*` and `/**`.
2.  Explicitly un-ignore files and directories that should be tracked by git using the `!` prefix.

**ACTION REQUIRED:** If you add or rename a file that needs to be tracked by Git, you **MUST** add a corresponding entry to the `.gitignore` file to un-ignore it.

For example, to track a new file `src/new_feature.c`, you would need to add `!/src/new_feature.c` to `.gitignore`.

Failure to do this will result in the file not being included in commits, which will break the CI/CD pipeline and cause confusion.

## 2. Building the Project (Pico SDK)

This project uses the Raspberry Pi Pico SDK and a CMake-based build system. Do not use `arduino-cli`.

### Dependencies

The following packages must be installed on a Debian-based system (like Ubuntu):
- `cmake`
- `make`
- `gcc-arm-none-eabi`

You can install them with:
```bash
sudo apt-get update && sudo apt-get install -y cmake make gcc-arm-none-eabi
```

### Setup and Build Process

To build the firmware, follow these steps:

1.  **Clone the Pico SDK:** Clone a specific, versioned tag of the SDK for reproducible builds. The repository should be placed alongside this project directory or in a known location. The CI uses version `2.2.0`.

    ```bash
    # In the parent directory of the project, or any other location
    git clone --branch 2.2.0 --depth 1 https://github.com/raspberrypi/pico-sdk.git
    ```

2.  **Initialize SDK Submodules:** The SDK requires its submodules (especially TinyUSB) to be initialized.

    ```bash
    cd pico-sdk
    git submodule update --init
    cd ..
    ```

3.  **Set Environment Variable:** The build system requires the `PICO_SDK_PATH` environment variable to be set to the absolute path of the SDK directory.

    ```bash
    export PICO_SDK_PATH=/path/to/your/pico-sdk
    ```

4.  **Build the Firmware:** Run `make` from the root of this project repository.

    ```bash
    make build
    ```
    This will create the `.uf2` file inside the `build/` directory, named according to the SKU (e.g., `PokeControllerForPico-pico.uf2`).
