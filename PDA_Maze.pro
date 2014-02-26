#-------------------------------------------------
#
# Project created by QtCreator 2014-02-26T04:03:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PDA_Maze
TEMPLATE = app

# For Debug
QMAKE_CFLAGS_DEBUG += -D_DEBUG
QMAKE_CXXFLAGS_DEBUG += -D_DEBUG

SOURCES += main.cpp \
    MainWindow.cpp \
    IniConfig.cpp \
    PlayField.cpp

HEADERS  += \
    MainWindow.h \
    IniConfig.h \
    PlayField.h

OTHER_FILES +=

RESOURCES += \
    PDA_Maze.qrc
