unix {
MOC_DIR = moc

OBJECTS_DIR = obj

LIBS += \
    -L/usr/local/lib \
    -lqwt-qt5 \
    -liir \
    -lattyscomm \
    -lbluetooth \
    -lfir

INCLUDEPATH += /usr/include/qwt

TMAKE_CXXFLAGS += -fno-exceptions

SOURCES = \
    dataplot.cpp \
    main.cpp \
    attys-ecg.cpp \
    ecg_rr_det.cpp

HEADERS = \
    attys-ecg.h \
    dataplot.h \
    ecg_rr_det.h

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
	-lws2_32 \
	-L../AttysComm/cpp/Debug \
	-lattyscomm_static


INCLUDEPATH += /iir1
INCLUDEPATH += ../AttysComm/cpp/
INCLUDEPATH += /qwt/src

SOURCES = \
    dataplot.cpp \
    main.cpp \
    attys-ecg.cpp \
    ecg_rr_det.cpp

HEADERS = \
    attys-ecg.h \
    dataplot.h \
    ecg_rr_det.h

CONFIG		+= qt debug c++11

QT   	+= widgets

RESOURCES     = application.qrc

}
