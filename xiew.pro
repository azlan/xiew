#-------------------------------------------------
#
# Project created by QtCreator 2016-01-01T11:15:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = xiew
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    BasicView/AbstractTableView.cpp \
    BasicView/HexDump.cpp \
    Utils/Configuration.cpp \
    Utils/RichTextPainter.cpp \
    Memory/MemoryPage.cpp \
    mydump.cpp \
    BasicView/Disassembly.cpp \
    capstone_wrapper/capstone_wrapper.cpp \
    Disassembler/capstone_gui.cpp \
    Disassembler/QBeaEngine.cpp \
    mydisassembly.cpp

HEADERS  += mainwindow.h \
    BasicView/AbstractTableView.h \
    BasicView/HexDump.h \
    Utils/Configuration.h \
    Utils/RichTextPainter.h \
    Utils/MenuBuilder.h \
    Utils/QActionLambda.h \
    Utils/StringUtil.h \
    dbg_types.h \
    Imports.h \
    Memory/MemoryPage.h \
    mydump.h \
    BasicView/Disassembly.h \
    capstone_wrapper/capstone_wrapper.h \
    Disassembler/capstone_gui.h \
    Disassembler/QBeaEngine.h \
    mydisassembly.h

FORMS    += mainwindow.ui

INCLUDEPATH += \
    Gui \
    BasicView \
    Disassembler \
    ThirdPartyLibs/capstone \
    ThirdPartyLibs/snowman \
    Memory \
    Bridge \
    Global \
    Utils \
    capstone_wrapper \
    capstone\\include

##
## Libraries
##
LIBS += -luser32

!contains(QMAKE_HOST.arch, x86_64) {
    # Windows x86 (32bit) specific build
#capstone\msvc\Debug\capstone.lib
    #LIBS += -L"$$PWD/capstone/msvc/Release/" -lcapstone
    LIBS += -L"$$PWD/libs/" -lcapstone_x86
#    LIBS += -L"$$PWD/Src/ThirdPartyLibs/snowman/" -lsnowman_x86

} else {
    # Windows x64 (64bit) specific build
    LIBS += -L"$$PWD/libs/" -lcapstone_x64
#    LIBS += -L"$$PWD/Src/ThirdPartyLibs/snowman/" -lsnowman_x64
}
