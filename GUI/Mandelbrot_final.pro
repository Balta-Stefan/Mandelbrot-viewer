QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    canvas.cpp \
    main.cpp \
    mainwindow.cpp \
    mandelbrotCalculator.cpp

HEADERS += \
    canvas.h \
    mainwindow.h \
    mandelbrotCalculator.h

FORMS += \
    mainwindow.ui

QMAKE_CXXFLAGS += -openmp
QMAKE_CXXFLAGS += /arch:AVX2
QMAKE_CXXFLAGS += /Oi
QMAKE_CXXFLAGS += /O2


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/'../../../../../Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.2/lib/x64/' -lOpenCL

INCLUDEPATH += $$PWD/'../../../../../Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.2/include'
DEPENDPATH += $$PWD/'../../../../../Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.2/include'

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/'../../../../../Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.2/lib/x64/OpenCL.lib'
else:win32-g++: PRE_TARGETDEPS += $$PWD/'../../../../../Program Files/NVIDIA GPU Computing Toolkit/CUDA/v11.2/lib/x64/libOpenCL.a'

DISTFILES += \
    Mandelbrot.cl


