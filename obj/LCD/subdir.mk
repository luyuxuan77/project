################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LCD/flash.c \
../LCD/lcd.c \
../LCD/spi.c 

C_DEPS += \
./LCD/flash.d \
./LCD/lcd.d \
./LCD/spi.d 

OBJS += \
./LCD/flash.o \
./LCD/lcd.o \
./LCD/spi.o 


EXPANDS += \
./LCD/flash.c.234r.expand \
./LCD/lcd.c.234r.expand \
./LCD/spi.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
LCD/%.o: ../LCD/%.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -fdump-rtl-expand -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/DELL/Downloads/CH32V20xEVT/EVT/EXAM/SRC/Debug" -I"c:/Users/DELL/Downloads/CH32V20xEVT/EVT/EXAM/SPI/SPI_LCD/LCD" -I"c:/Users/DELL/Downloads/CH32V20xEVT/EVT/EXAM/SRC/Core" -I"c:/Users/DELL/Downloads/CH32V20xEVT/EVT/EXAM/SPI/SPI_LCD/User" -I"c:/Users/DELL/Downloads/CH32V20xEVT/EVT/EXAM/SRC/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

