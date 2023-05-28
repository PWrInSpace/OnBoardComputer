# OBC STM32 Firmware [![Build STM32 and ESP32 binaries](https://github.com/PWrInSpace/OnBoardComputer/actions/workflows/test.yml/badge.svg?branch=STM32_SAC23_dev_suchy&event=push)](https://github.com/PWrInSpace/OnBoardComputer/actions/workflows/test.yml)
## This folder contains the firmware for the OBC module STM32 microcontroller. The code is written in C and is based on the [STM32CubeF1](https://www.st.com/en/embedded-software/stm32cubef1.html) HAL library. 

## Building
### Prerequisites
- [arm-none-eabi-gcc](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
- [arm-none-eabi-gdb](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
- [CMake](https://cmake.org/download/)
- [Make](https://www.gnu.org/software/make/)
- [JLink software pack](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack)
- [STM32 ST-LINK Utility (optionl)](https://www.st.com/en/development-tools/stsw-link004.html)
- [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)

### Building
1. Clone the repository
2. Run:  
 `mkdir build && cd build`  
  and then:  
 `cmake ..  -DCMAKE_TOOLCHAIN_FILE=CMake/arm-none-eabi-gcc.cmake -DCMAKE_BUILD_TYPE=Debug`  
  for debug build or:  
  `cmake ..  -DCMAKE_TOOLCHAIN_FILE=CMake/arm-none-eabi-gcc.cmake -DCMAKE_BUILD_TYPE=Release`  
   for Release build
3. Run `make -j12` or `cmake --build .` in the /build directory
4. The binary will be located in `build/OBC_STM32.bin`

For additional information on building the firmware, contact @JPerlinski.

