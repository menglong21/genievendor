DIRS := ./
SOURCE := $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))

CPP_SOURCE := 

OBJS   := $(patsubst %.c,%.o,$(SOURCE))
CPP_OBJS := $(patsubst %.cpp,%.o,$(CPP_SOURCE))
OBJ_DIR:= $(shell pwd)
LIB_DIR:= ./libs/ 

#target you can change test to what you want
TARGET  := genievendor
  
#compile and lib parameter
LIBS    := -lpthread -lrt -laglog

LDFLAGS := 
DEFINES :=
INCLUDE := -I include \


CFLAGS  := -O2 -Wall -MD $(DEFINES) $(INCLUDE) -D_GNU_SOURCE -std=c99 -D__STDC_LIMIT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_CONSTANT_MACROS -DOS_TYPE_LINUX -DPLATFORM_IS_BDP
CXXFLAGS:= $(CFLAGS) -DHAVE_CONFIG_H
  
#CROSS_COMPILE = ../toolchain/external-toolchain-C600/bin/arm-none-linux-gnueabi-
CROSS_COMPILE = arm-none-linux-gnueabi-
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
NM = $(CROSS_COMPILE)nm
STRIP = $(CROSS_COMPILE)strip
RANLIB = $(CROSS_COMPILE)ranlib
  
#i think you should do anything here

.PHONY : everything objs clean veryclean rebuild
  
everything : $(TARGET)
  
all : $(TARGET)
  
objs : $(OBJS)
  
rebuild: veryclean everything
                
clean :
	find . -name "*.o" | xargs rm -f
	find . -name "*.d" | xargs rm -f  
	find . -name "*~" | xargs rm -f
	rm -fr $(TARGET)
veryclean : clean
	rm -fr $(TARGET)
  
$(TARGET):$(OBJS) $(CPP_OBJS)
	$(CPP) $(CXXFLAGS) -o $@ $(OBJS) $(CPP_OBJS) $(LDFLAGS) $(LIBS) -L $(LIB_DIR)
	ls -lh $(TARGET)
	@echo "make strip $(TARGET)."
	$(STRIP) --strip-unneeded  $(TARGET)
	ls -lh $(TARGET)

$(OBJS):%.o:%.c
	$(CC) $(CXXFLAGS) -c $< -o $@  

$(CPP_OBJS):%.o:%.cpp
	$(CPP) $(CXXFLAGS) -c $< -o $@

install :
	[ ! -d $(dir ../out/bin) ] & mkdir -p ../out/bin/
	cp -f genievendor ../out/bin/
	@echo "install $(TARGET) to ../out/bin/$(TARGET)"
