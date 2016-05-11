TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    scanner.cpp \
    parser.cpp \
    analyzer.cpp \
    generator.cpp

HEADERS += \
    scanner.h \
    parser.h \
    token.h \
    treenode.h \
    analyzer.h \
    generator.h
