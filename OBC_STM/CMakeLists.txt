cmake_minimum_required(VERSION 3.15.3)

project(OBC_STM32)

enable_language(CXX C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(STM32CUBEMX_GENERATED_FILES

  Core/Src/main.c 
  Core/Src/gpio.c 
  Core/Src/adc.c 
  Core/Src/dma.c 
  Core/Src/i2c.c 
  Core/Src/Functions.c 
  Core/Src/stm32f1xx_it.c 
  Core/Src/stm32f1xx_hal_msp.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c 
  Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c 
  Core/Src/system_stm32f1xx.c

  startup_stm32f103xb.s
)

set(EXECUTABLE ${PROJECT_NAME}.out)

add_executable(${EXECUTABLE} ${STM32CUBEMX_GENERATED_FILES})

target_compile_definitions(${EXECUTABLE} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32F103xB
        )

target_include_directories(${EXECUTABLE} PRIVATE
  Core/Inc 
  Drivers/STM32F1xx_HAL_Driver/Inc 
  Drivers/STM32F1xx_HAL_Driver/Inc/Legacy 
  Drivers/CMSIS/Device/ST/STM32F1xx/Include 
  Drivers/CMSIS/Include
        )

target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m3
        -mthumb
     


        -fdata-sections
        -ffunction-sections
        -Wall

        $<$<CONFIG:Debug>:-Og>
        )

target_link_options(${EXECUTABLE} PRIVATE
        -T${CMAKE_SOURCE_DIR}/STM32F103C8Tx_FLASH.ld
        -mcpu=cortex-m3
        -mthumb
        -mfloat-abi=hard
        -specs=nano.specs
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        -Wl,--print-memory-usage
        )

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE})

# Create hex file
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)
