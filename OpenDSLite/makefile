# Makefile for building DTPLink based applications
# seguesoft2010@gmail.com

#C file foder
C_FILES_DIR := .

# Our working directory
ROOTDIR := $(shell pwd)

# Set build target name
TARGET_SO := libOpenDSLite.so
TARGET_EXE := ds.exe

#DEBUG_FLAGS += -DSPI_CC_SIMULATOR=1
XCC           = gcc
CFLAGS       = -Os -Wno-inline -I. -I./platform/linux

OBJS_SO := dtplink_buffer.o \
	dtplink_ds.o \
	dtplink_io.o \
	dtplink_util.o 

OBJS_EXE := dtplink_buffer.o \
	dtplink_ds.o \
	dtplink_io.o \
	dtplink_util.o \
	dsapp.o \
	ds.o 

$(TARGET_SO):  $(OBJS_SO)
	gcc $(OBJS_SO) -fPIC -shared -o $(TARGET_SO) -lws2_32 -lpthread 

$(TARGET_EXE):  $(OBJS_EXE)
	gcc $(OBJS_EXE) -o $(TARGET_EXE) -lws2_32 -lpthread 

dtplink_buffer.o: $(C_FILES_DIR)/dtplink_buffer.c
	gcc $(CFLAGS) -c -o $*.o $<

dtplink_ds.o: $(C_FILES_DIR)/dtplink_ds.c
	gcc $(CFLAGS) -c -o $*.o $<

dtplink_io.o: $(C_FILES_DIR)/dtplink_io.c
	gcc $(CFLAGS) -c -o $*.o $<

dtplink_util.o: $(C_FILES_DIR)/dtplink_util.c
	gcc $(CFLAGS) -c -o $*.o $<

ds.o: $(C_FILES_DIR)/ds.c
	gcc $(CFLAGS) -c -o $*.o $<

dsapp.o: $(C_FILES_DIR)/dsapp.c
	gcc $(CFLAGS) -c -o $*.o $<

clean:
	@-rm -rf *.o *.bak *.so *.exe
