################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../util/integers.c \
../util/strings.c 

C_DEPS += \
./util/integers.d \
./util/strings.d 

OBJS += \
./util/integers.o \
./util/strings.o 


# Each subdirectory must supply rules for building sources it contributes
util/%.o: ../util/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


