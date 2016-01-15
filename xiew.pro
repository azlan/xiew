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
    BasicView/Disassembly.cpp \
    BasicView/HexDump.cpp \
    BasicView/HistoryLineEdit.cpp \
    Disassembler/QBeaEngine.cpp \
    Disassembler/capstone_gui.cpp \
    GleeBug/Static.Pe.cpp \
    Gui/GotoDialog.cpp \
    Gui/OpenFileDialog.cpp \
    Gui/PeHeaderDialog.cpp \
    Memory/MemoryPage.cpp \
    Utils/Configuration.cpp \
    Utils/RichTextPainter.cpp \
    Xiew/XiewDisassembly.cpp \
    Xiew/XiewFile.cpp \
    Xiew/XiewHexDump.cpp \
    capstone_wrapper/capstone_wrapper.cpp

HEADERS  += mainwindow.h \
    BasicView/AbstractTableView.h \
    BasicView/Disassembly.h \
    BasicView/HexDump.h \
    BasicView/HistoryLineEdit.h \
    Capstone_wrapper/capstone_wrapper.h \
    Disassembler/QBeaEngine.h \
    Disassembler/capstone_gui.h \
    GleeBug/GleeBug.h \
    GleeBug/Static.Global.h \
    GleeBug/Static.Pe.h \
    GleeBug/Static.Region.h \
    Gui/GotoDialog.h \
    Gui/OpenFileDialog.h \
    Gui/PeHeaderDialog.h \
    Imports.h \
    Memory/MemoryPage.h \
    Utils/Configuration.h \
    Utils/MenuBuilder.h \
    Utils/QActionLambda.h \
    Utils/RichTextPainter.h \
    Utils/StringUtil.h \
    Xiew/XiewDisassembly.h \
    Xiew/XiewFile.h \
    Xiew/XiewHexDump.h \
    dbg_types.h

FORMS    += mainwindow.ui \
    Gui/GotoDialog.ui \
    Gui/OpenFileDialog.ui \
    Gui/PeHeaderDialog.ui

INCLUDEPATH += \
    BasicView \
    Bridge \
    Disassembler \
    GleeBug \
    Global \
    Gui \
    Memory \
    ThirdPartyLibs/capstone \
    ThirdPartyLibs/snowman \
    Utils \
    Xiew \
    capstone\\include \
    capstone_wrapper 

##
## Libraries
##
LIBS += -luser32

!contains(QMAKE_HOST.arch, x86_64) {
    # Windows x86 (32bit) specific build
#capstone\msvc\Debug\capstone.lib
    LIBS += -L"$$PWD/capstone/msvc/Release/" -lcapstone_dll
    #LIBS += -L"$$PWD/libs/" -lcapstone_x86
#    LIBS += -L"$$PWD/Src/ThirdPartyLibs/snowman/" -lsnowman_x86

} else {
    # Windows x64 (64bit) specific build
    LIBS += -L"$$PWD/libs/" -lcapstone_x64
#    LIBS += -L"$$PWD/Src/ThirdPartyLibs/snowman/" -lsnowman_x64
}
