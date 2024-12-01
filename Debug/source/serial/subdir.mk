################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/serial/serial.c 

C_DEPS += \
./source/serial/serial.d 

OBJS += \
./source/serial/serial.o 


# Each subdirectory must supply rules for building sources it contributes
source/serial/%.o: ../source/serial/%.c source/serial/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\ucosiii_config" -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\rtos\uCOSIII\src\uC-CPU\ARM-Cortex-M4\GNU" -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\rtos\uCOSIII\src\uC-CPU" -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\rtos\uCOSIII\src\uC-LIB" -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\rtos\uCOSIII\src\uCOS-III\Ports\ARM-Cortex-M4\Generic\GNU" -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\rtos\uCOSIII\src\uCOS-III\Source" -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-serial

clean-source-2f-serial:
	-$(RM) ./source/serial/serial.d ./source/serial/serial.o

.PHONY: clean-source-2f-serial

