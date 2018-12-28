#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)
# COMPONENT_ADD_INCLUDEDIRS=.
# CXXFLAGS +="-DESP32_IDF=1"
# Component makefile for extras/Common

# expected anyone using bmp driver includes it as 'Common/MQTT*.h'
INC_DIRS += $(Common_ROOT)..  $(Common_ROOT)

# args for passing into compile rule generation
Common_SRC_DIR =  $(Common_ROOT)
Common_CFLAGS= -DESP_OPEN_RTOS
Common_CXXFLAGS= -DESP_OPEN_RTOS -fno-rtti -std=gnu++11 -fno-exceptions -I../../ArduinoJson

$(eval $(call component_compile_rules,Common))

#ESP32 COMPONENT.MK
COMPONENT_SRCDIRS := .

COMPONENT_ADD_INCLUDEDIRS := .

COMPONENT_ADD_LDFLAGS += -lpthread
