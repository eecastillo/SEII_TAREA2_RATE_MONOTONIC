################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/board.c \
../board/clock_config.c \
../board/peripherals.c \
../board/pin_mux.c 

OBJS += \
./board/board.o \
./board/clock_config.o \
./board/peripherals.o \
./board/pin_mux.o 

C_DEPS += \
./board/board.d \
./board/clock_config.d \
./board/peripherals.d \
./board/pin_mux.d 


# Each subdirectory must supply rules for building sources it contributes
board/%.o: ../board/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK66FN2M0VMD18 -DCPU_MK66FN2M0VMD18_cm4 -DFSL_RTOS_BM -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"/home/ethan/Documents/SEI2/Tarea2/Tarea2_RateMonotonic/board" -I"/home/ethan/Documents/SEI2/Tarea2/Tarea2_RateMonotonic/source" -I"/home/ethan/Documents/SEI2/Tarea2/Tarea2_RateMonotonic" -I"/home/ethan/Documents/SEI2/Tarea2/Tarea2_RateMonotonic/drivers" -I"/home/ethan/Documents/SEI2/Tarea2/Tarea2_RateMonotonic/device" -I"/home/ethan/Documents/SEI2/Tarea2/Tarea2_RateMonotonic/CMSIS" -I"/home/ethan/Documents/SEI2/Tarea2/Tarea2_RateMonotonic/component/uart" -I"/home/ethan/Documents/SEI2/Tarea2/Tarea2_RateMonotonic/utilities" -I"/home/ethan/Documents/SEI2/Tarea2/Tarea2_RateMonotonic/component/serial_manager" -I"/home/ethan/Documents/SEI2/Tarea2/Tarea2_RateMonotonic/component/lists" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="../$(@D)/"=. -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


