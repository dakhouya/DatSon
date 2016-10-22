# export compile path
export PATH:=../toolchain/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin:$(PATH)

# compiler and flags
CC = arm-linux-gnueabihf-gcc
CFLAGS = -Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s

# path to include files
INCPATH = ../src
INC = $(INCPATH:%=-I%)

# path to source files
SRCPATH = ../src
 
COMPILE = $(CC) $(CFLAGS) -MMD -MD -Wall

APP = test.bin

all: $(APP)

$(APP): test.o
	$(COMPILE) -o $@ $^

clean:
	rm -f *.o *.d $(APP)

-include $(SRCS:.c=.d)

# to build a .o file, use the .c file in the current dir...
.c.o :
	$(COMPILE) -c $<

# ...or in common samples directory...
%.o : ./%.c
	$(COMPILE) -c $<

