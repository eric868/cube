##### @version $Id$  #####
#######Makefile Begin#######################

TARGET_NAME := dbaccess
BUILD_TYPE := Debug
MODULE_TYPE := dynamic_lib


COMMON_PATH := ../../common
TARGET := $(TARGET_NAME)

GOV_FLG = 1


SRCDIRS   := ./src
SRCDIRS   += 


SRCEXTS   := .c .cpp

CFLAGS    :=
CPPFLAGS  := -m64 -DSYB_LP64 -Dlinuxamd64=1 

CPPFLAGS  += -I./src
CPPFLAGS  += -Duse_namespace -fPIC -I$(COMMON_PATH)/include/s_utility -I$(COMMON_PATH)/3rd/mysql/include

LDFLAGS   := -L -lclntsh -Bdynamic -ldl -lnsl -lm -L$(COMMON_PATH)/3rd/mysql/lib -lmysql -L$(COMMON_PATH)/lib/$(BUILD_TYPE)/ -l_utility


BUILD_PRE : 
	rm -rf bin/$(BUILD_TYPE)/*
	mkdir -p bin/$(BUILD_TYPE)	

BUILD_POST :
	rm -rf $(COMMON_PATH)/include/s_dbaccess
	@mkdir -p $(COMMON_PATH)/include/s_dbaccess
	cp -p ./src/*.h $(COMMON_PATH)/include/s_dbaccess
	@mkdir -p $(COMMON_PATH)/lib/$(BUILD_TYPE)
	cp -p ./bin/$(BUILD_TYPE)/*.* $(COMMON_PATH)/lib/$(BUILD_TYPE)/

include ../../build/build_module.mk
### End of the Makefile 
###############################################################################

