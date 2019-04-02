# QT
QT += core
QT += widgets
QT += charts

QMAKE_CXXFLAGS += -std=c++0x

# ICON
DISTFILES += \
    app_icon.rc \
    resetB.png

RC_FILE = app_icon.rc

# DLL RTAudio
DEPENDPATH += . ../3rdparty/rtaudio
INCLUDEPATH += ../3rdparty/rtaudio
LIBS += -L../3rdparty/rtaudio/debug -lrtaudio

# DLL RTMidi
DEPENDPATH += . ../3rdparty/rtmidi
INCLUDEPATH += ../3rdparty/rtmidi
LIBS += -L../3rdparty/rtmidi/debug -lrtmidi

# DLL FFTReal
DEPENDPATH += . ../3rdparty/fftreal
INCLUDEPATH += ../3rdparty/fftreal
LIBS += -L../3rdparty/fftreal/debug -lfftreal

# DLL SNDFile
INCLUDEPATH += $$PWD/../3rdparty/libsndfile/include
INCLUDEPATH += $$PWD/../3rdparty/libsndfile/lib
DEPENDPATH += $$PWD/../3rdparty/libsndfile/include
DEPENDPATH += $$PWD/../3rdparty/libsndfile/lib
LIBS += -L$$PWD/../3rdparty/libsndfile/lib "-llibsndfile-1"


# Fichiers
SOURCES += \
    main.cpp \
    widgets/mainwindow.cpp \
    core/engine.cpp \
    filter/filter.cpp \
    widgets/filterwidget.cpp \
    widgets/waveformwidget.cpp \
    tools/ringbuffer.cpp \
    core/spectrumanalyser.cpp \
    widgets/spectrumwidget.cpp \
    widgets/progresswidget.cpp \
    tools/waveformpixmap.cpp \
    tools/frequenciesmanager.cpp \
    core/basicsignalmodule.cpp \
    core/enginemodule.cpp \
    core/externalsourcemodule.cpp \
    core/filemodule.cpp \
    qtwidgets/myqgroupbox.cpp \
    qtwidgets/myqradiobutton.cpp \
    qtwidgets/myqslider.cpp \
    qtwidgets/myqpushbutton.cpp \
    qtwidgets/myqcheckbox.cpp \
    qtwidgets/myqdial.cpp \
    widgets/sourceselector.cpp \
    widgets/doubleslider.cpp \
    tools/coeffsmanager.cpp \
    core/samplermodule.cpp \
    widgets/tristateselector.cpp \
    widgets/piano.cpp \
    midi/mididialog.cpp \
    midi/midiengine.cpp \
    widgets/instrumentselector.cpp \
    widgets/chordselector.cpp \
    qtwidgets/loopcheckbox.cpp \
    qtwidgets/timeslider.cpp \
    tools/qcustomplot.cpp

FORMS += \
    mainwindow.ui \
    core/basicsignalmodule.ui \
    core/externalsourcemodule.ui \
    core/filemodule.ui \
    midi/mididialog.ui \
    core/samplermodule.ui \
    core/equalizer.ui

HEADERS += \
    widgets/mainwindow.h \
    core/engine.h \
    filter/filter.h \
    widgets/filterwidget.h \
    widgets/waveformwidget.h \
    constants.h \
    tools/ringbuffer.h \
    core/spectrumanalyser.h \
    widgets/spectrumwidget.h \
    widgets/progresswidget.h \
    tools/waveformpixmap.h \
    tools/frequenciesmanager.h \
    core/basicsignalmodule.h \
    core/enginemodule.h \
    core/externalsourcemodule.h \
    core/filemodule.h \
    qtwidgets/myqgroupbox.h \
    qtwidgets/myqradiobutton.h \
    qtwidgets/myqslider.h \
    qtwidgets/myqpushbutton.h \
    qtwidgets/myqcheckbox.h \
    qtwidgets/myqdial.h \
    widgets/sourceselector.h \
    widgets/doubleslider.h \
    tools/coeffsmanager.h \
    core/samplermodule.h \
    widgets/tristateselector.h \
    widgets/piano.h \
    midi/mididialog.h \
    midi/midiengine.h \
    widgets/instrumentselector.h \
    widgets/chordselector.h \
    qtwidgets/loopcheckbox.h \
    qtwidgets/timeslider.h \
    tools/qcustomplot.h

RESOURCES += \
    images.qrc

DISTFILES += \
    WholeApp.qmodel

