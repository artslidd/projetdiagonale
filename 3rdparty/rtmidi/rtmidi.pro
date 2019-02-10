TEMPLATE = lib
TARGET   = rtmidi

# Fichiers

# Les libs et defines pour RTMidi (ne pas oublier d'ajouter le chemin du SDK dans les libs folders du projet (sauf avec MinGW)
DEFINES += __WINDOWS_MM__
LIBS += \
   -lwinmm

HEADERS += \
    RtMidi.h \
    rtmidi_c.h

SOURCES += \
    RtMidi.cpp \
    rtmidi_c.cpp

