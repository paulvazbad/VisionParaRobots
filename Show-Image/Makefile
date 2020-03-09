LXXLIBS+= -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lpthread

OBJS=$(patsubst %.cpp,%.o,$(shell find src/ | grep .cpp))
#OBJS+=$(patsubst %.c,%.o,$(wildcard src/*.c) $(wildcard src/*/*.c))
TARGETDIR = ./bin
ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

all: dirs $(OBJS)
	@echo Linking everything together...
	@g++ $(OBJS) -o $(TARGETDIR)/main $(LCCLIBS) $(LXXLIBS)
	@cp PlaceholderImage.jpg bin/
	@echo Done

dirs:
	@mkdir -p $(TARGETDIR)

#.c.o:	
#	gcc -c $<  -o $@  $(LCCLIBS)

.cpp.o:
	@echo Compiling $<...
	@g++ -c $<  -o $@  $(LXXLIBS) 

clean:
	@$(RM) -rf $(OBJS) bin
