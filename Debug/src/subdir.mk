################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BipBuffer.cpp \
../src/Bytes.cpp \
../src/Cbor.cpp \
../src/CircBuf.cpp \
../src/EventSource.cpp \
../src/Handler.cpp \
../src/Json.cpp \
../src/Link.cpp \
../src/Log.cpp \
../src/Logger.cpp \
../src/Mqtt.cpp \
../src/MqttIn.cpp \
../src/MqttOut.cpp \
../src/Msg.cpp \
../src/Msgpack.cpp \
../src/Node.cpp \
../src/PIC32_stubs.cpp \
../src/Prop.cpp \
../src/Property.cpp \
../src/STM32_stubs.cpp \
../src/Str.cpp \
../src/Strpack.cpp 

C_SRCS += \
../src/jsmn.c \
../src/printf.c 

OBJS += \
./src/BipBuffer.o \
./src/Bytes.o \
./src/Cbor.o \
./src/CircBuf.o \
./src/EventSource.o \
./src/Handler.o \
./src/Json.o \
./src/Link.o \
./src/Log.o \
./src/Logger.o \
./src/Mqtt.o \
./src/MqttIn.o \
./src/MqttOut.o \
./src/Msg.o \
./src/Msgpack.o \
./src/Node.o \
./src/PIC32_stubs.o \
./src/Prop.o \
./src/Property.o \
./src/STM32_stubs.o \
./src/Str.o \
./src/Strpack.o \
./src/jsmn.o \
./src/printf.o 

C_DEPS += \
./src/jsmn.d \
./src/printf.d 

CPP_DEPS += \
./src/BipBuffer.d \
./src/Bytes.d \
./src/Cbor.d \
./src/CircBuf.d \
./src/EventSource.d \
./src/Handler.d \
./src/Json.d \
./src/Link.d \
./src/Log.d \
./src/Logger.d \
./src/Mqtt.d \
./src/MqttIn.d \
./src/MqttOut.d \
./src/Msg.d \
./src/Msgpack.d \
./src/Node.d \
./src/PIC32_stubs.d \
./src/Prop.d \
./src/Property.d \
./src/STM32_stubs.d \
./src/Str.d \
./src/Strpack.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	xtensa-lx106-elf-g++ -I"/home/lieven/workspace/Common/inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	xtensa-lx106-elf-gcc -I"/home/lieven/workspace/Common/inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


