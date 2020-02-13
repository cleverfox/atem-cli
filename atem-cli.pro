QT += core gui network widgets
INCLUDEPATH += ./libqatemcontrol

TARGET = atem-cli
TEMPLATE = app

CONFIG += console
SOURCES += libqatemcontrol/*.cpp \
    main.cpp \
    cliapp.cpp \
    kbdreader.cpp \
    cmddict.cpp

HEADERS += libqatemcontrol/*.h \
    cliapp.h \
    kbdreader.h \
    cmddict.h

unix {
    target.path = /usr/lib
    INSTALLS += target

    header_files.files = $$HEADERS
    header_files.path = /usr/include
    INSTALLS += header_files
}
