# Agent Instructions for PokeControllerForPico

This document contains important information for AI agents working on this repository. Please read this before making any changes.

## 1. Critical: .gitignore Configuration

This repository uses a non-standard `.gitignore` configuration that **ignores all files by default**, and then explicitly un-ignores the necessary ones using the `!` prefix.

The configuration has been set up to use wildcards for the `src` directory (e.g., `!/src/*.c`, `!/src/*.h`). This means that any new source files with standard extensions that you add to the `src` directory will be **automatically tracked** by Git.

You generally **do not** need to edit the `.gitignore` file when adding new source files to the `src` directory.

However, if you add a new file or directory at the **root level** of the repository, or a file with a new extension, you **MUST** add a corresponding entry to the `.gitignore` file to un-ignore it (e.g., `!/new_directory/` or `!/new_file.txt`).

## 2. Project Documentation (`docs/`)

When modifying the logic, communication protocol, or MCU commands of this project, you **MUST** review and update the corresponding developer documentation in the `docs/` directory:

- `docs/internal_processing.md`: Explains the main loop, state machines (`ProgState`, `proc_state`), and general architecture.
- `docs/serial_protocol.md`: Describes the serial communication format expected by the MCU, including exactly how the bit-shifting logic works for button/stick inputs.
- `docs/mcu_commands.md`: Lists built-in macro commands (e.g., `mash_a`, `auto_league`) and provides instructions on how developers/users can define and add their own custom `SetCommand` arrays.

Before making any changes to `PokeControllerForPico_Func.cpp` or how UART inputs are parsed, **read these documents to understand the expected behavior.** If you add a feature, you must document it in `docs/`.

## 3. Building the Project

For build instructions (CMake, Pico SDK, etc.), please refer directly to the user guide in `README.md`. It explains the standard process of fetching the 2.2.0 `pico-sdk` and running `make pico`.
