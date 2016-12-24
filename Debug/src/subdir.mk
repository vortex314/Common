################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Actor.cpp \
../src/Base64.cpp \
../src/BipBuffer.cpp \
../src/BufferedByteStream.cpp \
../src/Bytes.cpp \
../src/Cbor.cpp \
../src/CborQueue.cpp \
../src/CircBuf.cpp \
../src/EventBus.cpp \
../src/EventSource.cpp \
../src/Handler.cpp \
../src/Json.cpp \
../src/Link.cpp \
../src/Log.cpp \
../src/Logger.cpp \
../src/Msg.cpp \
../src/Msgpack.cpp \
../src/Node.cpp \
../src/PIC32_stubs.cpp \
../src/STM32_stubs.cpp \
../src/Slip.cpp \
../src/SlipStream.cpp \
../src/Str.cpp \
../src/Strpack.cpp \
../src/Sys.cpp 

C_SRCS += \
../src/jsmn.c \
../src/printf.c 

OBJS += \
./src/Actor.o \
./src/Base64.o \
./src/BipBuffer.o \
./src/BufferedByteStream.o \
./src/Bytes.o \
./src/Cbor.o \
./src/CborQueue.o \
./src/CircBuf.o \
./src/EventBus.o \
./src/EventSource.o \
./src/Handler.o \
./src/Json.o \
./src/Link.o \
./src/Log.o \
./src/Logger.o \
./src/Msg.o \
./src/Msgpack.o \
./src/Node.o \
./src/PIC32_stubs.o \
./src/STM32_stubs.o \
./src/Slip.o \
./src/SlipStream.o \
./src/Str.o \
./src/Strpack.o \
./src/Sys.o \
./src/jsmn.o \
./src/printf.o 

CPP_DEPS += \
./src/Actor.d \
./src/Base64.d \
./src/BipBuffer.d \
./src/BufferedByteStream.d \
./src/Bytes.d \
./src/Cbor.d \
./src/CborQueue.d \
./src/CircBuf.d \
./src/EventBus.d \
./src/EventSource.d \
./src/Handler.d \
./src/Json.d \
./src/Link.d \
./src/Log.d \
./src/Logger.d \
./src/Msg.d \
./src/Msgpack.d \
./src/Node.d \
./src/PIC32_stubs.d \
./src/STM32_stubs.d \
./src/Slip.d \
./src/SlipStream.d \
./src/Str.d \
./src/Strpack.d \
./src/Sys.d 

C_DEPS += \
./src/jsmn.d \
./src/printf.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D__GXX_EXPERIMENTAL_CXX0X__ -I"/home/lieven/linux/Common/inc" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -D__GXX_EXPERIMENTAL_CXX0X__ -I"/home/lieven/linux/Common/inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


