################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/gpio.c \
../Src/i2c.c \
../Src/spi.c \
../Src/stm32f3xx_hal_msp.c \
../Src/stm32f3xx_it.c \
../Src/system_stm32f3xx.c 

CPP_SRCS += \
../Src/DisplayStrategy.cpp \
../Src/LCD_Display.cpp \
../Src/Nokia_Display.cpp \
../Src/main.cpp 

OBJS += \
./Src/DisplayStrategy.o \
./Src/LCD_Display.o \
./Src/Nokia_Display.o \
./Src/gpio.o \
./Src/i2c.o \
./Src/main.o \
./Src/spi.o \
./Src/stm32f3xx_hal_msp.o \
./Src/stm32f3xx_it.o \
./Src/system_stm32f3xx.o 

C_DEPS += \
./Src/gpio.d \
./Src/i2c.d \
./Src/spi.d \
./Src/stm32f3xx_hal_msp.d \
./Src/stm32f3xx_it.d \
./Src/system_stm32f3xx.d 

CPP_DEPS += \
./Src/DisplayStrategy.d \
./Src/LCD_Display.d \
./Src/Nokia_Display.d \
./Src/main.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU G++ Compiler'
	@echo $(PWD)
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F303xC -I"/home/popenyuk/Projects/Second_year/First_semester/POC/Embeded/Additional_task/Inc" -I"/home/popenyuk/Projects/Second_year/First_semester/POC/Embeded/Additional_task/Drivers/STM32F3xx_HAL_Driver/Inc" -I"/home/popenyuk/Projects/Second_year/First_semester/POC/Embeded/Additional_task/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy" -I"/home/popenyuk/Projects/Second_year/First_semester/POC/Embeded/Additional_task/Drivers/CMSIS/Device/ST/STM32F3xx/Include" -I"/home/popenyuk/Projects/Second_year/First_semester/POC/Embeded/Additional_task/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Src/%.o: ../Src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 '-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DUSE_HAL_DRIVER -DSTM32F303xC -I"/home/popenyuk/Projects/Second_year/First_semester/POC/Embeded/Additional_task/Inc" -I"/home/popenyuk/Projects/Second_year/First_semester/POC/Embeded/Additional_task/Drivers/STM32F3xx_HAL_Driver/Inc" -I"/home/popenyuk/Projects/Second_year/First_semester/POC/Embeded/Additional_task/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy" -I"/home/popenyuk/Projects/Second_year/First_semester/POC/Embeded/Additional_task/Drivers/CMSIS/Device/ST/STM32F3xx/Include" -I"/home/popenyuk/Projects/Second_year/First_semester/POC/Embeded/Additional_task/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


