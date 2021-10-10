QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Colorers/DefaultColorer.cpp \
    Colorers/LinearColorer.cpp \
    Colorers/SineColorer.cpp \
    MandelbrotCalculatorOLD.cpp \
    Colorers/iterationtocolorinterface.cpp \
    GUI/canvas.cpp \
    GUI/main.cpp \
    GUI/mainwindow.cpp \
    Mandelbrotters/mandelbrot_avx_parallel.cpp \
    Mandelbrotters/mandelbrot_avx_serial.cpp \
    Mandelbrotters/mandelbrot_cpu_parallel.cpp \
    Mandelbrotters/mandelbrot_cpu_serial.cpp \
    Mandelbrotters/mandelbrot_gpu.cpp \
    Mandelbrotters/mandelbrotcalculator.cpp \

HEADERS += \
    Colorers/DefaultColorer.h \
    Colorers/LinearColorer.h \
    Colorers/SineColorer.h \
    Colorers/iterationtocolorinterface.h \
    MandelbrotCalculatorOLD.h \
    GUI/canvas.h \
    GUI/mainwindow.h \
    Mandelbrotters/mandelbrot_avx_parallel.h \
    Mandelbrotters/mandelbrot_avx_serial.h \
    Mandelbrotters/mandelbrot_cpu_parallel.h \
    Mandelbrotters/mandelbrot_cpu_serial.h \
    Mandelbrotters/mandelbrot_gpu.h \
    Mandelbrotters/mandelbrotcalculator.h \

FORMS += \
    GUI\mainwindow.ui

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
    Resources\Mandelbrot.cl



