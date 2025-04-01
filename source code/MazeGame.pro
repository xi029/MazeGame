QT += widgets
CONFIG += c++11

TARGET = MazeGame
TEMPLATE = app
RC_ICONS = maze.ico
SOURCES += main.cpp \
           MainWindow.cpp \
           MazeWidget.cpp

HEADERS += MainWindow.h \
           MazeWidget.h

