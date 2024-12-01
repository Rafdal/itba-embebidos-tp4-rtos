################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/frontend/call_stack.c \
../source/frontend/input_menu.c \
../source/frontend/list_menu.c \
../source/frontend/menu_base.c \
../source/frontend/message_box.c 

C_DEPS += \
./source/frontend/call_stack.d \
./source/frontend/input_menu.d \
./source/frontend/list_menu.d \
./source/frontend/menu_base.d \
./source/frontend/message_box.d 

OBJS += \
./source/frontend/call_stack.o \
./source/frontend/input_menu.o \
./source/frontend/list_menu.o \
./source/frontend/menu_base.o \
./source/frontend/message_box.o 


# Each subdirectory must supply rules for building sources it contributes
source/frontend/%.o: ../source/frontend/%.c source/frontend/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\ucosiii_config" -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\rtos\uCOSIII\src\uC-CPU\ARM-Cortex-M4\GNU" -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\rtos\uCOSIII\src\uC-CPU" -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\rtos\uCOSIII\src\uC-LIB" -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\rtos\uCOSIII\src\uCOS-III\Ports\ARM-Cortex-M4\Generic\GNU" -I"D:\VSCode_Proyects\ucosiii_labo_de_micros_project\source\rtos\uCOSIII\src\uCOS-III\Source" -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-frontend

clean-source-2f-frontend:
	-$(RM) ./source/frontend/call_stack.d ./source/frontend/call_stack.o ./source/frontend/input_menu.d ./source/frontend/input_menu.o ./source/frontend/list_menu.d ./source/frontend/list_menu.o ./source/frontend/menu_base.d ./source/frontend/menu_base.o ./source/frontend/message_box.d ./source/frontend/message_box.o

.PHONY: clean-source-2f-frontend

