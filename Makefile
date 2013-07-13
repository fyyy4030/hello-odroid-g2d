##
## Makefile for hello-world-g2d
##

CC = gcc
CXX = g++
AR = ar
LD = g++

INC = 
CFLAGS = -Wall -ggdb -DFIMGAPI_DEBUG_MESSAGE
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = -lpthread

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS)
CXXFLAGS_DEBUG = $(CFLAGS)
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = out/obj
DEP_DEBUG = 
OUT_DEBUG = hello-world-g2d

OBJ_DEBUG = $(OBJDIR_DEBUG)/hello-world-g2d.o $(OBJDIR_DEBUG)/fimglib.o $(OBJDIR_DEBUG)/fimg4x.o

all: debug

clean: clean_debug

before_debug: 
	test -d out/obj || mkdir -p out/obj
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/fimg4x.o: fimg4x.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c fimg4x.c -o $(OBJDIR_DEBUG)/fimg4x.o

$(OBJDIR_DEBUG)/fimglib.o: fimglib.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c fimglib.c -o $(OBJDIR_DEBUG)/fimglib.o

$(OBJDIR_DEBUG)/hello-world-g2d.o: hello-world-g2d.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c hello-world-g2d.c -o $(OBJDIR_DEBUG)/hello-world-g2d.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf $(OBJDIR_DEBUG)

.PHONY: before_debug after_debug clean_debug
