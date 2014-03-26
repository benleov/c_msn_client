################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../commandline.c \
../commandparser.c \
../connection.c \
../connectionlistener.c \
../debug.c \
../main.c \
../msn.c 

C_DEPS += \
./commandline.d \
./commandparser.d \
./connection.d \
./connectionlistener.d \
./debug.d \
./main.d \
./msn.d 

OBJS += \
./commandline.o \
./commandparser.o \
./connection.o \
./connectionlistener.o \
./debug.o \
./main.o \
./msn.o 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


