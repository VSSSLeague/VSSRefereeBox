TEMPLATE = app
DESTDIR  = ../bin
TARGET   = VSSReferee
VERSION  = 1.0.0

# Temporary dirs
OBJECTS_DIR = tmp/obj
MOC_DIR = tmp/moc
UI_DIR = tmp/moc
RCC_DIR = tmp/rc

CONFIG += c++17 console
CONFIG -= app_bundle
QT += core network gui widgets opengl

DEFINES += QT_DEPRECATED_WARNINGS
LIBS += -lQt5Core -lprotobuf -lgrpc -lgrpc++ -lGLU -lfmt -lArmorial

system(echo "Generating referee proto headers" && cd proto && cmake . && make && cd ..)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APP_NAME=\\\"$$TARGET\\\"
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += PROJECT_PATH=\\\"$${PWD}\\\"

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    proto/vssref_command.pb.cc \
    proto/vssref_common.pb.cc \
    proto/vssref_placement.pb.cc \
    src/constants/constants.cpp \
    src/gui/refereegui.cpp \
    src/referee/referee.cpp \
    src/referee/scoreboard/scoreboard.cpp

HEADERS += \
    proto/vssref_command.pb.h \
    proto/vssref_common.pb.h \
    proto/vssref_placement.pb.h \
    src/constants/constants.h \
    src/gui/refereegui.h \
    src/referee/referee.h \
    src/referee/scoreboard/scoreboard.h

FORMS += \
    src/gui/refereegui.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    proto/README.md

RESOURCES += \
    rsc/resources.qrc
