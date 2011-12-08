bin_PROGRAMS = smrgygurdy
AM_CXXFLAGS  = -pthread -Wall -std=c++0x
AM_LDFLAGS   = -pthread

smrgygurdy_SOURCES = \
        SoundModelMono.cpp SoundModelPoly.cpp StringAllocator.cpp Factory.cpp \
        OutputDirect.cpp OutputMixer.cpp PlayoutThread.cpp \
        Pedal.cpp MiniLAB1008.cpp \
        Keyboard.cpp \
        Lock.cpp Thread.cpp \
        main.cpp Controller.cpp

smrgygurdy_LDADD = $(ARTIFASTRING_LIBS) $(LIBHID_LIBS)
 
