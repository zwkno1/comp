TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    scanner.cpp \
    parser.cpp

HEADERS += \
    scanner.h \
    parser.h \
    token.h \
    treenode.h
