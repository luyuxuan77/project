################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/ch32v20x_it.c \
../User/main.c \
../User/system_ch32v20x.c 

C_DEPS += \
./User/ch32v20x_it.d \
./User/main.d \
./User/system_ch32v20x.d 

OBJS += \
./User/ch32v20x_it.o \
./User/main.o \
./User/system_ch32v20x.o 


EXPANDS += \
./User/ch32v20x_it.c.234r.expand \
./User/main.c.234r.expand \
./User/system_ch32v20x.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/luka/Desktop/shenzhenproject/CH32V203RBT6/Debug" -I"c:/Users/luka/Desktop/shenzhenproject/CH32V203RBT6/Core" -I"c:/Users/luka/Desktop/shenzhenproject/CH32V203RBT6/User" -I"c:/Users/luka/Desktop/shenzhenproject/CH32V203RBT6/Peripheral/inc" -I"c:/Users/luka/Desktop/shenzhenproject/CH32V203RBT6/HARDWARE/ADC" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

