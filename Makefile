CXXFLAGS += -W -Wall -I/usr/local/include -std=c++0x
LD_FLAGS += -L/usr/local/lib
LD_LIBS  += -lasound -lpthread -lartifastring -lmcchid -lhid -lusb -lm

APP  := smurgygurdy

SRCS := SoundModelMono.cpp SoundModelPoly.cpp StringAllocator.cpp Factory.cpp \
        OutputDirect.cpp OutputMixer.cpp PlayoutThread.cpp \
        Pedal.cpp Keyboard.cpp \
        Lock.cpp Thread.cpp \
        main.cpp Controller.cpp

OBJS := $(SRCS:.cpp=.o)
ALL := .depends $(APP)

all: $(ALL)

$(APP): $(OBJS)
	$(LINK.cc) $(LD_FLAGS) -o $(APP) $(OBJS) $(LD_LIBS)

.depends: $(SRCS)
	g++ -M $(SRCS) > .depends

# clean and tidy rules
tidy:				
	rm -f *.o *~ \#*\# 

clean: tidy
	rm -f $(ALL)

include .depends

