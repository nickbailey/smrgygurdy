bin_PROGRAMS  = smrgygurdy
AM_CXXFLAGS   = -pthread -Wall -std=c++0x
AM_LDFLAGS    = -pthread

smrgygurdy_SOURCES = \
        SoundModelMono.cpp SoundModelPoly.cpp StringAllocator.cpp \
        OutputDirect.cpp OutputMixer.cpp PlayoutThread.cpp \
        Pedal.cpp \
        Keyboard.cpp \
        Lock.cpp Thread.cpp \
        main.cpp Controller.cpp

EXTRA_smrgygurdy_SOURCES = MiniLAB1080.cpp Comedi.cpp

smrgygurdy_LDADD = $(PEDAL_CLASSES) \
                   $(ARTIFASTRING_LIBS) $(LIBCONFIGXX_LIBS) \
                   $(LIBHID_LIBS) $(BOGIO_LIBS)
smrgygurdy_DEPENDENCIES = $(PEDAL_CLASSES)
 
