TEMPLATE = lib
TARGET   = rtaudio

# Les libs et defines pour RTAudio (ne pas oublier d'ajouter le chemin du SDK dans les libs folders du projet (sauf avec MinGW)
DEFINES += __WINDOWS_WASAPI__
LIBS += \
   -lksuser \
   -lOle32 \
   -lUser32 \
   -lAdvapi32

# Fichiers

HEADERS += \
    asio.h \
    asiodrivers.h \
    asiodrvr.h \
    asiolist.h \
    asiosys.h \
    dsound.h \
    FunctionDiscoveryKeys_devpkey.h \
    ginclude.h \
    iasiodrv.h \
    iasiothiscallresolver.h \
    soundcard.h \
    RtAudio.h

SOURCES += \
    asio.cpp \
    asiodrivers.cpp \
    asiolist.cpp \
    iasiothiscallresolver.cpp \
    RtAudio.cpp

