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
    Disassembler/QBeaEngine.cpp

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
    Disassembler/QBeaEngine.h

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
