unix {
MOC_DIR = moc

OBJECTS_DIR = obj

LIBS += \
    -L/usr/local/lib \
    -lqwt-qt5 \
    -liir \
    -lbluetooth

INCLUDEPATH += ../AttysComm/c/
INCLUDEPATH += /usr/include/qwt

TMAKE_CXXFLAGS += -fno-exceptions

SOURCES = \
    dataplot.cpp \
    main.cpp \
    attys-ecg.cpp \
    ecg_rr_det.cpp \
    ../AttysComm/c/AttysComm.cpp \
    ../AttysComm/c/AttysScan.cpp \
    ../AttysComm/c/base64.c

HEADERS = \
    attys-ecg.h \
    dataplot.h \
    ecg_rr_det.h \
    ../AttysComm/c/AttysComm.h \
    ../AttysComm/c/AttysScan.h \
    ../AttysComm/c/base64.h

CONFIG		+= qt release c++11

QT             	+= widgets

RESOURCES     = application.qrc

}






win32 {

MOC_DIR = moc

OBJECTS_DIR = obj

LIBS += \
    -L/qwt/lib \
    -lqwtd \
	-L/iir1/Debug \
    -liir_static \
	-lws2_32

INCLUDEPATH += /iir1
INCLUDEPATH += ../AttysComm/c/
INCLUDEPATH += /qwt/src

SOURCES = \
    vepplot.cpp \
    dataplot.cpp \
    main.cpp \
    attys-ep.cpp \
    stim.cpp \
    ../AttysComm/c/AttysComm.cpp \
    ../AttysComm/c/AttysScan.cpp \
    ../AttysComm/c/base64.c

HEADERS = \
    attys-ep.h \
    vepplot.h \
    dataplot.h \
    stim.h \
    ../AttysComm/c/AttysComm.h \
    ../AttysComm/c/AttysScan.h \
    ../AttysComm/c/base64.h

CONFIG		+= qt debug c++11

QT   	+= widgets

RESOURCES     = application.qrc

}
