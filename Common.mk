##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=Common
ConfigurationName      :=Debug
WorkspacePath          :=/home/lieven/linux/linux
ProjectPath            :=/home/lieven/linux/Common
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=lieven
Date                   :=12/11/16
CodeLitePath           :=/home/lieven/.codelite
LinkerName             :=/usr/bin/x86_64-linux-gnu-g++
SharedObjectLinkerName :=/usr/bin/x86_64-linux-gnu-g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/lib$(ProjectName).a
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="Common.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/x86_64-linux-gnu-ar rcu
CXX      := /usr/bin/x86_64-linux-gnu-g++
CC       := /usr/bin/x86_64-linux-gnu-gcc
CXXFLAGS :=  -g $(Preprocessors)
CFLAGS   :=  -g $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/x86_64-linux-gnu-as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Srcs=src/Actor.cpp src/Base64.cpp src/BipBuffer.cpp src/BufferedByteStream.cpp src/Bytes.cpp src/Cbor.cpp src/CborQueue.cpp src/CircBuf.cpp src/EventBus.cpp src/EventSource.cpp \
	src/Handler.cpp src/jsmn.c src/Json.cpp src/Link.cpp src/Log.cpp src/Logger.cpp src/Msg.cpp src/Msgpack.cpp src/Node.cpp src/PIC32_stubs.cpp \
	src/printf.c src/Slip.cpp src/SlipStream.cpp src/STM32_stubs.cpp src/Str.cpp src/Strpack.cpp src/Sys.cpp 

Objects0=$(IntermediateDirectory)/src_Actor.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Base64.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_BipBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_BufferedByteStream.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Bytes.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Cbor.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_CborQueue.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_CircBuf.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_EventBus.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_EventSource.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_Handler.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_jsmn.c$(ObjectSuffix) $(IntermediateDirectory)/src_Json.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Link.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Log.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Logger.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Msg.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Msgpack.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Node.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_PIC32_stubs.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_printf.c$(ObjectSuffix) $(IntermediateDirectory)/src_Slip.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_SlipStream.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_STM32_stubs.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Str.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Strpack.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_Sys.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(IntermediateDirectory) $(OutputFile)

$(OutputFile): $(Objects)
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(AR) $(ArchiveOutputSwitch)$(OutputFile) @$(ObjectsFileList) $(ArLibs)
	@$(MakeDirCommand) "/home/lieven/linux/linux/.build-debug"
	@echo rebuilt > "/home/lieven/linux/linux/.build-debug/Common"

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


./Debug:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:
##
## Clean
##
clean:
	$(RM) -r ./Debug/


