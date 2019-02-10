TEMPLATE = subdirs

# Ensure that library is built before application
CONFIG  += ordered
CONFIG  += console
CONFIG  += c++11

SUBDIRS += 3rdparty/fftreal
SUBDIRS += 3rdparty/rtaudio
SUBDIRS += 3rdparty/rtmidi
SUBDIRS += app
