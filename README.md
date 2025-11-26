# LVGL ported to STM32H7S78-DK

## Overview

This is an H7 series board, touted for "MPU UIs on an MCU". It has a
600MHz ARM Cortex-M7 MCU with a memory protection unit. There is an
unusually small internal flash and SRAM for such a powerful MCU, but
it's compensated by 128MB flash and 32MB PSRAM. A very simple
bootloader is needed to jump from the internal flash to the external
flash at reset.

## Buy

You can purchase an STM32H7S78-DK from https://www.st.com/en/evaluation-tools/stm32h7s78-dk.html

## Benchmark

<!-- <a href="https://www.youtube.com/watch?v=8TXbeBs7hy8">
    <img src="https://github.com/user-attachments/assets/5a410a4a-1270-4606-a0b0-444c8805af88" width="75%">
</a> -->

The configuration is double-buffered direct mode with the LTDC driver and NeoChrom.

| Name                      | Avg. CPU | Avg. FPS | Avg. time | render time | flush time |
| :------------------------ | -------: | -------: | --------: | ----------: | ---------: |
| Empty screen              | 41%      | 59       | 4         | 2           | 2          |
| Moving wallpaper          | 60%      | 59       | 10        | 6           | 4          |
| Single rectangle          | 69%      | 59       | 10        | 0           | 10         |
| Multiple rectangles       | 37%      | 59       | 5         | 2           | 3          |
| Multiple RGB images       | 65%      | 59       | 7         | 2           | 5          |
| Multiple ARGB images      | 81%      | 60       | 9         | 2           | 7          |
| Rotated ARGB images       | 58%      | 59       | 6         | 1           | 5          |
| Multiple labels           | 85%      | 43       | 18        | 14          | 4          |
| Screen sized text         | 93%      | 29       | 30        | 18          | 12         |
| Multiple arcs             | 56%      | 60       | 8         | 3           | 5          |
| Containers                | 46%      | 60       | 5         | 3           | 2          |
| Containers with overlay   | 88%      | 30       | 30        | 22          | 8          |
| Containers with opa       | 50%      | 59       | 6         | 4           | 2          |
| Containers with opa_layer | 50%      | 59       | 7         | 6           | 1          |
| Containers with scrolling | 75%      | 59       | 11        | 11          | 0          |
| Widgets demo              | 92%      | 33       | 19        | 18          | 1          |
| All scenes avg.           | 65%      | 52       | 11        | 7           | 4          |

## Specification

### CPU and Memory
- **MCU:** Arm Cortex-M7 @600MHz
- **RAM:** 620KB internal, 32MB external
- **Flash:** 64KB internal, 128MB External
- **GPU:** NeoChrom (GPU2D), ChromArt (DMA2D)

### Display and Touch
- **Resolution:** 800x480
- **Display Size:** 5"
- **Interface:** Parallel RGB
- **Color Depth:** 24-bit (this project uses 16 by default)
- **Technology:** LCD
- **DPI:** 187 px/inch
- **Touch Pad:** Capacitive

### Connectivity
- Arduino-compatible pin headers
- Application USB-C
- Micro SD Card slot
- Ethernet port

## Getting started

### Hardware setup
- Connect a USB-C cable to the USB-C port labeled
  STLINK USB CN7 and your PC.

### Software setup
- Install [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html).

### Run the project
- Clone this repository repository:
  ```shell
  git clone --recurse-submodules https://github.com/lvgl/lv_port_stm32h7s78-dk.git
  ```
- In STM32CubeIDE, click **File > Open Projects from File System**, click **Directory**,
  choose the cloned repo, and then click **Finish**.
- You will need to build and upload the bootloader just once, unless you modify it.
  - Click to highlight STM32H7S78-DK_Boot in the Project Explorer side panel
  - Click the hammer icon to build the bootloader.
  - Click the play icon to upload the bootloader to the board.
- Click to highlight STM32H7S78-DK_Appli in the Project Explorer side panel
- Click the hammer icon to build the application.
  The "Debug" build mode is used, even though full optimization is enabled.
- Click the play icon to upload the application to the board.
- After the application has been uploaded, press the black reset button (B1/U6)
  and the application will run.

### Debugging

Interactive debugging is available over USB-C

- Click the bug icon to start the debugger.
- It is necessary to manually reset the board _after_ the debugger has started to begin debugging.
  Press the black reset button (B1/U6) and then press the "continue" button in STM32CubeIDE.
- For the best debugging experience, change the optimization level to `-O0` instead
  of `-O3`. The program execution will be more similar to the source code.
  Right click the project in the **Project Explorer** sidebar, click **Properties**.
  Go to **C/C++ Build > Settings > MCU/MPU GCC Compiler > Optimization**.
  Choose **None (-O0)** Under **Optimization level**.

## Notes

`Appli/Core/Src/lvgl_port.c` is where the LVGL display is created.
You can change the buffering configuration there.

The framebuffers are the only thing kept in external RAM.
`STM32CubeIDE/Appli/STM32H7S7L8HXH_RAMxspi1_ROMxspi2.ld` is the
relevant linker script. You can see in `lvgl_port.c` that the
buffers are placed in the linker section called "Framebuffer"
which is in external RAM.

The display is capable of 24 bit color depth but LTDC is configured as 16 bits for
reduced memory usage.

`USE_HAL_LTDC_REGISTER_CALLBACKS` has been enabled in `Core/Inc/stm32u7xx_hal_conf.h` to support the
LVGL LTDC driver.

## Contribution and Support

If you find any issues with the development board feel free to open an Issue in this repository. For LVGL related issues (features, bugs, etc) please use the main [lvgl repository](https://github.com/lvgl/lvgl).

If you found a bug and found a solution too please send a Pull request. If you are new to Pull requests refer to [Our Guide](https://docs.lvgl.io/master/CONTRIBUTING.html#pull-request) to learn the basics.

