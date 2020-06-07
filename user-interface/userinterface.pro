QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    ../panda-controller/cpp-utils/src/network.cpp \
    ../panda-controller/cpp-utils/src/conversion.cpp \
    ../panda-controller/cpp-utils/src/files.cpp \
    ../panda-controller/cpp-utils/src/json.cpp \
    ../panda-controller/cpp-utils/src/math.cpp \
    ../panda-controller/cpp-utils/src/output.cpp \
    ../panda-controller/cpp-utils/src/system.cpp \
    ../panda-controller/src/serialization.cpp \
    ../panda-controller/src/config_handler.cpp \
    external/qcustomplot/qcustomplot.cpp \
    src/cameracalibration.cpp \
    src/devicehandler.cpp \
    src/main.cpp \
    src/skillhandlerinterface.cpp \
    src/user-interface.cpp \
    src/marker.cpp \
    src/tcp_command.cpp \
    src/videostream.cpp \
    src/skillhandler.cpp \
    src/taskhandler.cpp


HEADERS += \
    ../panda-controller/include/serialization.h \
    ../panda-controller/include/config_handler.h \
    external/qcustomplot/qcustomplot.h \
    include/skillhandlerinterface.h \
    include/skillhandler.h \
    include/user-interface.h \
    include/marker.h \
    include/tcp_command.h \
    include/videostream.h \
    include/devicehandler.h \
    include/taskhandler.h

FORMS += \
    forms/userinterface.ui

INCLUDEPATH += /usr/local/include/opencv4



# INCLUDEPATH += /usr/local/include/
LIBS += $(shell pkg-config opencv4 --libs)

INCLUDEPATH += /usr/include/eigen3
INCLUDEPATH += ../panda-controller/include
INCLUDEPATH += ../panda-controller/cpp-utils/include
INCLUDEPATH += ./include

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

QT += network

# LIBS +=

LIBS += \
       -lboost_system\
        -lboost_serialization\
        -lfranka\
        -lyaml-cpp\
        -lstdc++fs\
        -lopencv_aruco

#-lcpp_utils\
