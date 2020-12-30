##### @version $Id: Makefile 1084 2012-02-15 08:10:28Z liuzq $  #####
#######Makefile Begin#######################

TARGET_NAME := utility_test 

#Debug or Release
BUILD_TYPE := Debug

#dynamic_lib/static_lib/executable
MODULE_TYPE := executable
COMMON_PATH := ../..

TARGET := $(TARGET_NAME)

SRCDIRS   := ./src
SRCDIRS   += 

CPPFLAGS  := -I$(COMMON_PATH)/include
CPPFLAGS  += -std=c++0x

LDFLAGS   := -L$(COMMON_PATH)/lib/$(BUILD_TYPE)/ -l_utility

BUILD_PRE : 
	rm -rf bin/$(BUILD_TYPE)/*

BUILD_POST :
	
include ../../build/build_module.mk

### End of the Makefile 
###############################################################################

