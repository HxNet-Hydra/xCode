#-------------------------------------------------
#
# Project created by QtCreator 2019-01-25T12:53:03
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testOpenCV
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += C:\Silkron\Projects\Vendron4\Core\opencv\release\install\include
LIBS += C:\Silkron\Projects\Vendron4\Core\opencv\release\bin\libopencv_core400.dll
LIBS += C:\Silkron\Projects\Vendron4\Core\opencv\release\bin\libopencv_highgui400.dll
LIBS += C:\Silkron\Projects\Vendron4\Core\opencv\release\bin\libopencv_imgcodecs400.dll
LIBS += C:\Silkron\Projects\Vendron4\Core\opencv\release\bin\libopencv_imgproc400.dll
LIBS += C:\Silkron\Projects\Vendron4\Core\opencv\release\bin\libopencv_calib3d400.dll
LIBS += C:\Silkron\Projects\Vendron4\Core\opencv\release\bin\libopencv_features2d400.dll
LIBS += C:\Silkron\Projects\Vendron4\Core\opencv\release\bin\libopencv_dnn400.dll
LIBS += C:\Silkron\Projects\Vendron4\Core\opencv\release\bin\libopencv_videoio400.dll
LIBS += C:\Silkron\Projects\Vendron4\Core\opencv\release\bin\libopencv_objdetect400.dll

SOURCES += \
        main.cpp \
        widget.cpp

HEADERS += \
        widget.h

FORMS += \
        widget.ui
