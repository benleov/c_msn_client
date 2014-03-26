################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ssl/client.c \
../ssl/common.c \
../ssl/msnSSLAuth.c \
../ssl/read_write.c \
../ssl/sclient.c 

C_DEPS += \
./ssl/client.d \
./ssl/common.d \
./ssl/msnSSLAuth.d \
./ssl/read_write.d \
./ssl/sclient.d 

OBJS += \
./ssl/client.o \
./ssl/common.o \
./ssl/msnSSLAuth.o \
./ssl/read_write.o \
./ssl/sclient.o 


# Each subdirectory must supply rules for building sources it contributes
ssl/%.o: ../ssl/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


